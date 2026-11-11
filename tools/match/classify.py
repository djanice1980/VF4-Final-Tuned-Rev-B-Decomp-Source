"""Sort the near misses by how their compiled code differs from the game's.

    python3 -I tools/match/classify.py [--from diff-perm.tsv] [--jobs N] [--out FILE.tsv]

For choosing what to teach variants.py next: each DIFF function's current C
is compiled as it stands and both sides are disassembled, with literal
pools dropped (the words pc-relative loads read) and alignment nop runs
dropped. Then, as instruction text, the first of these that holds names
the function's kind:

  callreg    only the register calls go through differs: GCC 3.2 calls
             through r0 when the callee returns a value or has no
             declaration, r1 when it is declared void, so a callee's
             declared return type is wrong (fix the declaration)
  registers  the same instructions with other registers (allocation: a
             variable's lifetime, a temporary, the order values are made)
  values     the same instructions with other immediates or displacements
             as well (a struct offset, an array size, a constant, a type's
             size)
  order      the same instructions in another order (statement order, an
             expression's operand order)
  extend     the difference is mostly exts/extu (a variable or a field
             narrower or wider, signed or unsigned, than the original)
  frame      the stack frame differs (saved registers, locals' space)
  calls      a call more or fewer (an inlined helper, a call the
             decompiler merged)
  branches   branch instructions differ (a condition inverted, a loop
             in another shape: for/while/do, an early exit)
  other      anything else

Writes FILE (default build/automatch/classify.tsv): address, name, game
size, compiled size, kind, distance (instructions), and the instructions
only the game has (-) and only the compile has (+), counted by mnemonic.
Prints a summary per kind, with the most common mnemonic differences.
"""
import difflib, multiprocessing, os, re, subprocess, sys, tempfile
from collections import Counter

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import program
import auto_match as am
import match

ROOT = am.ROOT
B = match.BASE
INSN = re.compile(r"^\s*([0-9a-f]+):\s+(?:[0-9a-f]{2} ){2}\s*\t(\S+)\s*(.*?)\s*(?:!.*)?$")  # "! value": objdump's note
EXTEND = {"exts.b", "exts.w", "extu.b", "extu.w"}
BRANCH = {"bt", "bf", "bt.s", "bf.s", "bt/s", "bf/s", "bra", "braf", "bsr", "bsrf"}
CALL = {"jsr", "bsr", "bsrf"}
DELAYED = {"rts", "rte", "bra", "braf", "bsr", "bsrf", "jmp", "jsr", "bt.s", "bf.s", "bt/s", "bf/s"}


def insns(blob, at):
    """[(address, mnemonic, operands)] without pool words or alignment nop runs."""
    with tempfile.NamedTemporaryFile(suffix=".bin") as f:
        f.write(blob)
        f.flush()
        r = subprocess.run(["sh4-elf-objdump", "-D", "-b", "binary", "-m", "sh4", "-EL",
                            f"--adjust-vma={at:#x}", f.name], capture_output=True, text=True)
    out = [(int(m.group(1), 16), m.group(2), m.group(3)) for m in map(INSN.match, r.stdout.split("\n")) if m]
    # pc-relative loads only reach forward, so in address order a word is
    # known to be pool before it is reached; a pool word that happens to
    # decode as a load must not mark its "target" (a real instruction
    # vanished from 0c0873c0's listing that way)
    pool = set()
    for a, mn, ops in out:
        t = re.match(r"0x([0-9a-f]+)", ops)
        if a in pool:
            continue
        if t and mn in ("mov.l", "mov.w", "mova"):
            d = int(t.group(1), 16)
            pool.update(range(d, d + (2 if mn == "mov.w" else 4), 2))
    out = [x for x in out if x[0] not in pool]
    # nop runs pad to an alignment; a nop just after a delayed branch is its
    # (empty) slot, which counts
    slot = lambda i: i and out[i - 1][1] in DELAYED
    keep = []
    for i, x in enumerate(out):
        run = x[1] == "nop" and not slot(i) and (
            (i and out[i - 1][1] == "nop") or (i + 1 < len(out) and out[i + 1][1] == "nop"))
        if not run:
            keep.append(x)
    # a nop after the last rts and its slot pads the pool to a word boundary
    while len(keep) > 2 and keep[-1][1] == "nop" and keep[-3][1] == "rts":
        keep.pop()
    return keep


def norm(x, level):
    """Instruction text: 0 = as is (addresses masked), 1 = registers masked too,
    2 = immediates and displacements masked too."""
    mn, ops = x[1], re.sub(r"0x[0-9a-f]+", "A", x[2])
    if level >= 1:
        ops = re.sub(r"\b(?:r\d+|fr\d+|dr\d+|xd\d+|xf\d+)\b", "R", ops)
    if level >= 2:
        ops = re.sub(r"#-?\d+", "#N", ops)
        ops = re.sub(r"@\(-?\d+,", "@(N,", ops)
    return f"{mn} {ops}"


def kind(game, mine):
    g = [[norm(x, l) for x in game] for l in range(3)]
    c = [[norm(x, l) for x in mine] for l in range(3)]
    if g[1] == c[1]:
        # a pool load into the register a jsr then uses, r0 on one side and r1 on the other
        def calls(xs):
            return [(xs[i][2], xs[i + 1][2]) for i in range(len(xs) - 1)
                    if xs[i + 1][1] == "jsr" and xs[i][1] == "mov.l" and xs[i][2].endswith(",r0") or
                    i + 1 < len(xs) and xs[i + 1][1] == "jsr" and xs[i][1] == "mov.l" and xs[i][2].endswith(",r1")]
        rest = lambda xs: [norm(x, 0) for x in xs if not (x[1] == "jsr" and x[2] in ("@r0", "@r1"))
                           and not (x[1] == "mov.l" and re.match(r"0x[0-9a-f]+,r[01]$", x[2]))]
        if rest(game) == rest(mine) and calls(game) != calls(mine):
            return "callreg"
        return "registers"
    if g[2] == c[2]:
        return "values"
    if Counter(g[1]) == Counter(c[1]):
        return "order"
    gm, cm = Counter(x[1] for x in game), Counter(x[1] for x in mine)
    only_g, only_c = gm - cm, cm - gm
    diff = only_g + only_c
    total = sum(diff.values())
    if total and sum(diff[m] for m in EXTEND) * 2 >= total:
        return "extend"
    frame = lambda xs: [norm(x, 0) for x in xs if "r15" in x[2] and x[1] in ("add", "mov.l", "sts.l", "lds.l", "fmov.s")]
    if frame(game) != frame(mine) and abs(len(game) - len(mine)) <= 6:
        return "frame"
    if sum(gm[m] for m in CALL) != sum(cm[m] for m in CALL):
        return "calls"
    if any(gm[m] != cm[m] for m in BRANCH):
        return "branches"
    return "other"


def one_batch(job):
    flags, funcs, img = job
    rows = []
    with tempfile.TemporaryDirectory(dir=os.path.join(ROOT, "build")) as tmp:
        named = [(a, size, None, f"V0_{a:08x}", re.sub(r"\b" + re.escape(n) + r"\b(?=\s*\()", f"V0_{a:08x}", t, count=1))
                 for a, size, n, t in funcs]
        ok = am.split_compilable(named, [] if flags == "-" else [flags], tmp, [])
        if not ok:
            return rows
        f = os.path.join(tmp, "batch.c")
        am.write_batch(f, ok, [] if flags == "-" else [flags])
        secs, syms, _ = match.read_elf(match.compile_c(f, tmp))
        sizes = {a: size for a, size, _, _ in funcs}
        names = {a: n for a, _, n, _ in funcs}
        for si, s in enumerate(secs):
            if s["typ"] != 1 or not (s["flags"] & 4):
                continue
            for fs in (x for x in syms if x["shn"] == si and x["type"] == 2):
                m = re.match(r"_?V0_(0c[0-9a-f]{6})$", fs["name"])
                if not m:
                    continue
                a = int(m.group(1), 16)
                code = s["data"][fs["value"]:fs["value"] + fs["size"]]
                game = insns(img[a - B:a - B + sizes[a]], a)
                mine = insns(code, a)
                gt, ct = [norm(x, 1) for x in game], [norm(x, 1) for x in mine]
                sm = difflib.SequenceMatcher(None, gt, ct, autojunk=False)
                dist = max(len(gt), len(ct)) - sum(b.size for b in sm.get_matching_blocks())
                gm, cm = Counter(x[1] for x in game), Counter(x[1] for x in mine)
                fmt = lambda c: ",".join(f"{k}x{v}" for k, v in c.most_common())
                rows.append((a, names[a], sizes[a], len(code), kind(game, mine), dist, fmt(gm - cm), fmt(cm - gm)))
    return rows


def main():
    args = sys.argv[1:]
    opt = lambda k, d: type(d)(args[args.index(k) + 1]) if k in args else d
    src = opt("--from", os.path.join(ROOT, "build", "automatch", "diff-perm.tsv"))
    out = opt("--out", os.path.join(ROOT, "build", "automatch", "classify.tsv"))
    jobs = opt("--jobs", max(1, (os.cpu_count() or 2) - 4))
    rows = [l.split("\t") for l in open(src).read().split("\n")[1:] if l.count("\t") == 5]
    want = {int(r[0], 16): r[4] for r in rows if r[5] == "DIFF"}
    done = set()  # defined in source/ already (adopted since the last export)
    for d, _, fs in os.walk(program.SOURCE):
        for f in fs:
            if f.endswith(".c"):
                done |= set(re.findall(r"(?m)^[A-Za-z_][^\n;]*?\b(\w+)\([^\n]*\)\n\{", open(os.path.join(d, f)).read()))
    cands = [c for c in am.candidates() if c[0] in want and c[3] not in done]
    img = open(program.IMAGE, "rb").read()
    work = []
    for flags in ("-fno-delayed-branch", "-"):
        group = [(c[0], c[1], c[3], c[4]) for c in cands if want[c[0]] == flags]
        work += [(flags, group[i:i + 25], img) for i in range(0, len(group), 25)]
    with multiprocessing.Pool(jobs) as p:
        res = sorted(r for batch in p.imap_unordered(one_batch, work) for r in batch)
    with open(out, "w") as w:
        w.write("address\tname\tgame size\tcompiled size\tkind\tdistance\tgame only\tcompile only\n")
        for a, n, gs, cs, k, d, g, c in res:
            w.write(f"0x{a:08x}\t{n}\t{gs}\t{cs}\t{k}\t{d}\t{g}\t{c}\n")
    print(f"{len(res)} of {len(cands)} near misses classified -> {out}")
    print(f"{'kind':<10} {'count':>6} {'distance<=4':>12} {'median dist':>12}")
    by = {}
    for r in res:
        by.setdefault(r[4], []).append(r)
    for k, rs in sorted(by.items(), key=lambda kv: -len(kv[1])):
        ds = sorted(r[5] for r in rs)
        print(f"{k:<10} {len(rs):>6} {sum(1 for d in ds if d <= 4):>12} {ds[len(ds) // 2]:>12}")
    tally = lambda col: Counter(m for r in res if r[5] <= 8 for part in r[col].split(",") if part
                                for m, n in [part.rsplit("x", 1)] for _ in range(int(n)))
    print("\nin near misses at distance <= 8, the instructions")
    print("  only the game has:   " + ", ".join(f"{m} {n}" for m, n in tally(6).most_common(12)))
    print("  only the compile has: " + ", ".join(f"{m} {n}" for m, n in tally(7).most_common(12)))


if __name__ == "__main__":
    main()
