"""Try the decompiler's own output as source: compile each function in src/
with the game's compiler and keep the ones that come out as the game's bytes.

    python3 -I tools/match/auto_match.py [--max-size N] [--limit N] [--out FILE] [--skip-done FILE] [--retry FILE]

Each candidate is a function in src/ that is not matched yet (no "matched:"
line), is not assembly (tools/match/delay_slots.py), and uses nothing the
decompiler writes that C cannot hold (CONCAT / SUBn / ZEXT..., in_ / unaff_ /
extraout_ registers, code-pointer calls). Its
flags follow its range: -fno-delayed-branch where the game's C leaves every
delay slot empty, the common flags where the libraries fill them. Functions
are compiled in batches with vf4ft.h and vf4ft_data.h (the unnamed globals
as Ghidra types them); a batch that does not compile is split until the
function that does not is found. Every compiled function is then checked by
match.py's run_checks (bytes and relocations).

Writes FILE (default build/automatch/results.tsv): address, name, size, src
file, flags, result (match, match*, DIFF, no compile, skipped: reason).
--skip-done FILE leaves out what an earlier results file already tried;
--retry FILE tries only what it found DIFF or not compiling (after a change
to types or headers).
The decompiler's warnings are not a reason to skip: a function whose
decompile lost something simply does not match.
"""
import os, re, sys, tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import program
import match
import delay_slots

ROOT = match.ROOT
HEAD = re.compile(r"^/\* 0x(0c[0-9a-f]{6})  size 0x([0-9a-f]+)  [^\n]*\n(.*?)^ \*/\n(.*?)(?=^/\* 0x0c[0-9a-f]{6}  size |\Z)",
                  re.M | re.S)
NOT_C = [("CONCAT", r"\bCONCAT\d+\("), ("SUBn", r"\bSUB\d+\("), ("ZEXT/SEXT", r"\b[ZS]EXT\d+\("),
         ("carry/borrow", r"\b(CARRY|SCARRY|SBORROW)\d*\("), ("bad data", r"halt_baddata|halt_unimplemented"),
         ("register placeholder", r"\b(in|unaff|extraout)_[A-Za-z0-9_]+"),
         ("stack address", r"&stack0x|\bregister0x|\bin_stack"),
         ("decompile failed", r"decompile failed"), ("FPSCR", r"FPSCR"),
         ("float conversion", r"\bTRUNC\(|\bROUND\(|\bNAN\(|\bSQRT\(|\bABS\(")]


def candidates():
    out = []
    for d, _, fs in os.walk(program.SRC):
        for f in sorted(fs):
            if not f.endswith(".c"):
                continue
            path = os.path.relpath(os.path.join(d, f), program.SRC)
            for m in HEAD.finditer(open(os.path.join(d, f), errors="replace").read()):
                if re.search(r"(?m)^ \* matched: ", m.group(3)):
                    continue
                # the decompiler's own comments before the signature ("WARNING: Removing
                # unreachable block ...") are not part of the function
                text = re.sub(r"^(?:/\*.*?\*/\s*)+", "", m.group(4).strip(), flags=re.S)
                name = re.search(r"\b([A-Za-z_]\w*)\s*\(", text)
                # the span, where the body has gaps (" * extent:"): what the game's
                # function occupies, which comparisons with it need
                ext = re.search(r"(?m)^ \* extent: 0x([0-9a-f]+)$", m.group(3))
                size = int(ext.group(1), 16) if ext else int(m.group(2), 16)
                out.append((int(m.group(1), 16), size, path, name.group(1) if name else "?", text))
    return out


def kind_of(groups):
    """address -> 'empty' / 'filled' / 'assembly' from delay_slots' ranges."""
    def kind(a):
        for lo, hi, k, _ in groups:
            if lo <= a <= hi:
                return "assembly" if k == "assembly" else "empty" if "empty" in k else "filled"
        return None
    return kind


def compile_ok(funcs, flags, tmp):
    """Does this set of function texts compile (and assemble) together?"""
    f = os.path.join(tmp, "batch.c")
    write_batch(f, funcs, flags)
    try:
        match.compile_c(f, tmp)
        return True
    except SystemExit:
        return False


NOPROTO = re.compile(r"^#define (?:NO|KR)PROTO_\w+\n", re.M)  # no / old-style declaration


def write_batch(f, funcs, flags):
    """A "#define NOPROTO_<name>" line in a function's text (variants.py's
    noproto rule: the file had no declaration of <name>) goes before the
    includes, for the whole batch."""
    defines = sorted({d for c in funcs for d in NOPROTO.findall(c[4])})
    with open(f, "w") as w:
        w.write(f"/* match-flags: {' '.join(flags)} */\n" + "".join(defines)
                + "#include \"vf4ft.h\"\n#include \"vf4ft_data.h\"\n\n")
        for c in funcs:
            w.write(NOPROTO.sub("", c[4]) + "\n\n")


def split_compilable(funcs, flags, tmp, bad):
    if not funcs:
        return []
    if compile_ok(funcs, flags, tmp):
        return funcs
    if len(funcs) == 1:
        bad.append(funcs[0])
        return []
    h = len(funcs) // 2
    return split_compilable(funcs[:h], flags, tmp, bad) + split_compilable(funcs[h:], flags, tmp, bad)


def main():
    args = sys.argv[1:]
    opt = lambda k, d: type(d)(args[args.index(k) + 1]) if k in args else d
    max_size, limit = opt("--max-size", 0x100000), opt("--limit", 0)
    out = opt("--out", os.path.join(ROOT, "build", "automatch", "results.tsv"))
    os.makedirs(os.path.dirname(out), exist_ok=True)
    kind = kind_of(delay_slots.groups())
    done_before = set()
    retry = None
    if "--retry" in args:
        retry = set()
        for l in open(args[args.index("--retry") + 1]).read().split("\n")[1:]:
            f = l.split("\t")
            if len(f) == 6 and f[5] in ("DIFF", "no compile"):
                retry.add(int(f[0], 16))
    if "--skip-done" in args:
        for l in open(args[args.index("--skip-done") + 1]).read().split("\n")[1:]:
            f = l.split("\t")
            if len(f) == 6 and not f[5].startswith("skipped"):
                done_before.add(int(f[0], 16))
    img = open(program.IMAGE, "rb").read()
    symtab, slots = match.load_symbols(), match.game_slots()
    rows, todo = [], {"empty": [], "filled": []}
    for c in sorted(candidates(), key=lambda c: (c[1], c[0])):
        if c[1] > max_size or c[0] in done_before or (retry is not None and c[0] not in retry):
            continue
        k = kind(c[0])
        why = next((n for n, p in NOT_C if re.search(p, c[4])), None)
        if k in (None, "assembly"):
            why = why or "assembly or no build group"
        if why:
            rows.append((c, "-", "skipped: " + why))
        else:
            todo[k].append(c)
    if limit:
        for k in todo:
            todo[k] = todo[k][:limit]
    for k, flags in (("empty", ["-fno-delayed-branch"]), ("filled", [])):
        items = todo[k]
        for i in range(0, len(items), 40):
            batch = items[i:i + 40]
            with tempfile.TemporaryDirectory(dir=os.path.join(ROOT, "build")) as tmp:
                bad = []
                ok = split_compilable(batch, flags, tmp, bad)
                for c in bad:
                    rows.append((c, " ".join(flags) or "-", "no compile"))
                if ok:
                    f = os.path.join(tmp, "batch.c")
                    write_batch(f, ok, flags)
                    _, _, res = match.run_checks([f], img, symtab, slots, say=lambda *a, **kw: None)
                    got = {r[0]: r[3] for r in res}
                    for c in ok:
                        rows.append((c, " ".join(flags) or "-", got.get(c[3], "?")))
            done = sum(1 for r in rows if not r[2].startswith("skipped"))
            matched = sum(1 for r in rows if r[2].startswith("match"))
            print(f"{k}: {min(i + 40, len(items))}/{len(items)} compiled-or-tried; {matched} match so far", flush=True)
    with open(out, "w") as w:
        w.write("address\tname\tsize\tfile\tflags\tresult\n")
        for c, flags, res in sorted(rows, key=lambda r: r[0][0]):
            w.write(f"0x{c[0]:08x}\t{c[3]}\t{c[1]}\t{c[2]}\t{flags}\t{res}\n")
    from collections import Counter
    tally = Counter(r[2] for r in rows)
    for k, v in tally.most_common():
        print(f"{v:6d}  {k}")
    mb = sum(r[0][1] for r in rows if r[2].startswith("match"))
    print(f"matching: {sum(v for k, v in tally.items() if k.startswith('match'))} functions, {mb} bytes -> {out}")


if __name__ == "__main__":
    main()
