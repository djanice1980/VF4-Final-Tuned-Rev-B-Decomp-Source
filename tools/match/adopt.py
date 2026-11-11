"""Move functions auto_match.py found matching into source/.

    python3 -I tools/match/adopt.py [results.tsv]     (default build/automatch/results.tsv)
    python3 -I tools/match/adopt.py results.tsv --texts variants.json
        (variants.py's matching rewrites, in place of the decompiler's text;
         results.tsv gives each function's flags)

Only "match" rows are taken ("match*", with a relocation that could not be
checked, is counted and left). Each function's text comes from src/ as it
is now, with its signature joined onto one line (source/'s rule, which the
exporter relies on). A function goes into source/<its src/ file> when that
file is known; one in an _unmapped/ chunk goes into
source/_unmapped/<start of its build group>.c (the address ranges
delay_slots.py --groups prints), a name that does not change as files get
placed; where a group holds two original files, the second is a file named
by its first address with a "split-from: <group start>" line near its top,
and the group's functions from that address on go there. A new file gets its match-flags from the group (-fno-delayed-branch
where the game's C leaves delay slots empty); an existing one keeps its
own, and a function whose group wants other flags than its file has is
reported, not moved. Every file declares the unnamed globals it uses itself
(tools/match/pin_globals.py: stable DAT_<address> names, the types they had
when the code matched), rather than including vf4ft_data.h, whose
placeholder names change as Ghidra's types do.

A function that only fails because its file pins a global with an older
type (one since typed char / short, say) is tried again with the globals
it uses declared as they are now, and kept if the whole file still matches.

Each file written is then compiled and checked (match.py's run_checks): new
functions that do not compile together are split out as auto_match does,
and any that does not match in its new file is left out again, so every
file is left with only matching functions. Run tools/match/match.py
afterwards all the same.
"""
import os, re, sys, tempfile
from collections import Counter, defaultdict

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import program
import auto_match
import delay_slots
import match
import pin_globals

ROOT = auto_match.ROOT
# also an old-style (K&R) definition: "void f(c) char c;" then "{"
DEF = re.compile(r"^[A-Za-z_][^\n;]*?\b([A-Za-z_]\w*)\([^\n;]*?\)(?: [^\n{]*;)?\n\{", re.M)


def defname(t):
    """The function a text is for: its last definition (a static inline
    helper the game's compiler inlined has to come before it)."""
    names = DEF.findall(t)
    return names[-1] if names else None


def one_line(text):
    """The signature (everything before the '{' line) joined onto one line;
    NOPROTO_ define lines in front stay as they are. The permuter's
    "inline static" helpers are spelled "static inline", the only form
    defname and ExportSource read as a helper rather than a game function."""
    text = re.sub(r"^inline static\b", "static inline", text, flags=re.M)
    defines = "".join(auto_match.NOPROTO.findall(text))
    lines = auto_match.NOPROTO.sub("", text).split("\n")
    k = lines.index("{")
    sig = " ".join(l.strip() for l in lines[:k] if l.strip())
    sig = re.sub(r"\s*,\s*", ", ", re.sub(r"\s+", " ", sig))
    return defines + sig + "\n" + "\n".join(lines[k:])


def prototype_of(sig):
    """An old-style (K&R) signature "void f(a, b) char a; int b;" as the
    prototype "void f(char a, int b)" (what a declaration needs; ExportSource
    does the same)."""
    m = re.fullmatch(r"(.*?\b\w+)\(([^()]*)\) (.+;)", sig)
    if not m:
        return sig
    types = {}
    for d in m.group(3).split(";"):
        dm = re.fullmatch(r"\s*(.*?)\s*\b(\w+)\s*", d)
        if dm:
            types[dm.group(2)] = dm.group(1)
    ps = [f"{types.get(n.strip(), 'int')} {n.strip()}" for n in m.group(2).split(",") if n.strip()]
    return f"{m.group(1)}({', '.join(ps) or 'void'})"


def sync_declaration(t):
    """A matching rewrite can change the function's own signature (another
    return or parameter type); vf4ft_functions.h still declares the old one,
    and GCC rejects the definition as conflicting. Put the new signature in
    the header (the next export writes the same: a function in source/ is
    declared as it is defined there)."""
    # the last definition's line: a static inline helper in front has its own
    defs = list(DEF.finditer(auto_match.NOPROTO.sub("", t)))
    if not defs:
        return
    sig = prototype_of(defs[-1].group(0).split("\n", 1)[0].strip())
    m = re.search(r"\b([A-Za-z_]\w*)\s*\(", sig)
    if not m:
        return
    h = os.path.join(os.environ.get("VF4FT_INC") or os.path.join(program.SRC, "include"), "vf4ft_functions.h")
    text = open(h).read()
    name = m.group(1)
    # the prototype line (not a guard's old-style "f();")
    pat = re.compile(r"^[^\n#]*\b" + re.escape(name) + r"\s*\(\s*[^)\s][^\n]*\);( /\* 0x[0-9a-f]+ \*/)?$", re.M)
    old = pat.search(text)
    if old and not old.group(0).startswith(sig + ";"):
        new = sig + ";" + (old.group(1) or "")
        guarded = f"#elif defined(KRPROTO_{name})" in text
        if NARROW.search(sig) and not guarded:  # as the export writes it: three ways
            new = (f"#if defined(NOPROTO_{name})\n#elif defined(KRPROTO_{name})\n" + sig[:sig.index("(") + 1] + ");\n#else\n"
                   + new + "\n#endif")
        open(h, "w").write(text[:old.start()] + new + text[old.end():])
        SYNCED.append(name)
        SYNC_UNDO[t] = (h, old.group(0), new)


SYNC_UNDO = {}  # text -> (header, old declaration, new one): undone if the function is not kept


def undo_sync(t):
    """Put back the declaration sync_declaration changed for a function that
    was not kept (left as it was, the header would declare a signature no
    definition has, and the callers would not compile)."""
    if t not in SYNC_UNDO:
        return
    h, old, new = SYNC_UNDO.pop(t)
    text = open(h).read()
    if new in text:
        open(h, "w").write(text.replace(new, old, 1))
    m = re.search(r"\b([A-Za-z_]\w*)\s*\(", old)
    if m and m.group(1) in SYNCED:
        SYNCED.remove(m.group(1))


NARROW = re.compile(r"\((?:[^()]*[,(]\s*)?(?:char|short|byte|uchar|ushort|bool|undefined1|undefined2|schar)\s+\w+\s*[,)]")


def guard_declaration(name):
    """Declare name three ways in vf4ft_functions.h (NOPROTO_ / KRPROTO_ /
    prototype), as the export does for a function a source/ file defines
    one of those for; a no-op when it already is."""
    h = os.path.join(os.environ.get("VF4FT_INC") or os.path.join(program.SRC, "include"), "vf4ft_functions.h")
    text = open(h).read()
    if f"#if defined(NOPROTO_{name})" in text:
        return
    pat = re.compile(r"^[^\n#]*\b" + re.escape(name) + r"\s*\(\s*[^)\s][^\n]*\);( /\* 0x[0-9a-f]+ \*/)?$", re.M)
    old = pat.search(text)
    if not old:
        return
    line = old.group(0)
    head = line[:re.search(r"\b" + re.escape(name) + r"\s*\(", line).end()]
    new = f"#if defined(NOPROTO_{name})\n#elif defined(KRPROTO_{name})\n{head});\n#else\n{line}\n#endif"
    open(h, "w").write(text[:old.start()] + new + text[old.end():])


def repair(broken_files, ctx):
    """A file whose matched functions broke after a declaration changed: the
    original file declared that callee its own way. For each changed callee it
    calls, try no define, then NOPROTO_ (undeclared: called as returning int,
    through r0), then KRPROTO_ (old-style: arguments passed unconverted), and
    keep what makes the most of the file match. Returns the files still
    broken."""
    def score(f):
        try:
            _, _, res = match.run_checks([f], *ctx, say=lambda *a, **k: None)
        except SystemExit:
            return -1, False
        good = sum(r[3] == "match" for r in res)
        return good, good == len(res)

    still = []
    for f in broken_files:
        text = open(f).read()
        inc = text.find('#include "vf4ft.h"')
        calls = [n for n in SYNCED if re.search(r"\b" + re.escape(n) + r"\s*\(", text)
                 and f"PROTO_{n}\n" not in text]
        if not calls or inc < 0:
            still.append(f)
            continue
        chosen = {}
        best, ok = score(f)
        for n in calls:
            guard_declaration(n)
            for kind in ("NOPROTO", "KRPROTO"):
                trial = dict(chosen, **{n: kind})
                defs = "".join(f"#define {k}_{m}\n" for m, k in trial.items())
                open(f, "w").write(text[:inc] + defs + text[inc:])
                s, all_ok = score(f)
                if s > best:
                    best, ok, chosen = s, all_ok, trial
                if all_ok:
                    break
            if ok:
                break
        defs = "".join(f"#define {k}_{m}\n" for m, k in chosen.items())
        open(f, "w").write(text[:inc] + defs + text[inc:] if ok else text)
        if ok:
            print(f"  {os.path.relpath(f, ROOT)}: " + ", ".join(f"{k}_{m}" for m, k in chosen.items()))
        else:
            still.append(f)
    return still


SYNCED = []  # functions whose declaration sync_declaration changed


def header(path, file_flags):
    return (f"/*\n * {path}\n * Decompiler output that compiles to the game's bytes (found by\n"
            f" * tools/match/auto_match.py, moved here by tools/match/adopt.py); every\n"
            f" * function here matches (python3 -I tools/match/match.py checks the tree).\n"
            + (f" * match-flags: {file_flags}\n" if file_flags else "")
            + " */\n#include \"vf4ft.h\"\n")


HELPER = re.compile(r"^static inline [^\n;]*?\b(\w+)\([^\n;]*\)\n\{\n.*?^\}\n?", re.M | re.S)


def drop_known_helpers(base, add):
    """Texts may each carry the same static inline helper (fsqrt_asm, an
    interrupt-mask wrapper); one definition per file: a helper already in the
    file, or in an earlier text, with the same text is left out of the later
    ones (a different body under the same name stays, and fails to compile)."""
    seen = {m.group(0).rstrip("\n") for m in HELPER.finditer(base)}
    out = []
    for t in add:
        for m in list(HELPER.finditer(t)):
            h = m.group(0).rstrip("\n")
            if h in seen:
                t = t.replace(m.group(0), "", 1)
            seen.add(h)
        out.append(t)
    return out


def compose(base, add):
    """base with the functions in add; a "#define NOPROTO_<name>" (or KRPROTO_)
    one carries (variants.py's noproto rule) goes before the file's includes,
    and the header gets that name's guard if it has none yet."""
    defines = [d for t in add for d in auto_match.NOPROTO.findall(t) if d not in base]
    for d in defines:  # the define does nothing until the header has the guard
        guard_declaration(re.match(r"#define (?:NO|KR)PROTO_(\w+)", d).group(1))
    add = drop_known_helpers(base, [auto_match.NOPROTO.sub("", t) for t in add])
    if defines:
        inc = base.find('#include "vf4ft.h"')
        base = base[:inc] + "".join(dict.fromkeys(defines)) + base[inc:] if inc >= 0 else "".join(dict.fromkeys(defines)) + base
    return base.rstrip("\n") + ("\n\n" + "\n\n".join(t.strip() for t in add) if add else "") + "\n"


DECLS = None  # src/include/vf4ft_data.h as it is now (pin_globals)


def write(p, text, refresh=()):
    """The file, with its unnamed globals declared in it (pin_globals)."""
    open(p, "w").write(text)
    pin_globals.pin(p, DECLS, refresh)


def repin(p, kept, left, ctx):
    """A function left out because the file pins a global with an older type
    (one now typed char/short, say): try it with the globals it uses declared
    as they are now, kept only if every function in the file still matches."""
    pinned = {m.group(2): m.group(1).strip() for m in pin_globals.DECL.finditer(open(p).read())}
    out = []
    for t in left:
        newer = {n for n in pin_globals.NAME.findall(t)
                 if n in pinned and n in DECLS and DECLS[n][0] != pinned[n]}
        if not newer:
            continue
        before = open(p).read()
        write(p, compose(before, [t]), newer)
        try:
            img, symtab, slots = ctx
            _, _, res = match.run_checks([p], img, symtab, slots, say=lambda *a, **k: None)
            ok = all(r[3] == "match" for r in res) and defname(t) in {r[0] for r in res}
        except SystemExit:
            ok = False
        if ok:
            out.append(t)
        else:
            open(p, "w").write(before)
    return out


def compiles(p, base, add):
    write(p, compose(base, add))
    try:
        with tempfile.TemporaryDirectory(dir=os.path.join(ROOT, "build")) as tmp:
            match.compile_c(p, tmp)
        return True
    except SystemExit:
        return False


def settle(p, base, add, ctx, orig):
    """Write base + the largest part of add that compiles and matches; return
    what was kept. orig: the file as it was (None for a new file), put back
    if base itself does not compile; a new file that keeps nothing is removed."""
    if not compiles(p, base, []):
        if os.environ.get("VF4FT_ADOPT_DEBUG"):
            print(f"DEBUG settle {p}: the file without the new functions does not compile", file=sys.stderr)
        if orig is None:
            os.remove(p)
        else:
            open(p, "w").write(orig)
        return []
    def split(items):
        if not items or compiles(p, base, items):
            return items
        if len(items) == 1:
            return []
        h = len(items) // 2
        return split(items[:h]) + split(items[h:])
    ok = split(add)
    write(p, compose(base, ok))
    if ok:
        img, symtab, slots = ctx
        try:
            _, _, res = match.run_checks([p], img, symtab, slots, say=lambda *a, **k: None)
        except SystemExit as e:  # put the file back rather than stop half-written
            if os.environ.get("VF4FT_ADOPT_DEBUG"):
                print(f"DEBUG settle {p}: run_checks exited: {e}", file=sys.stderr)
            res = []
            ok = []
        new = {defname(t) for t in ok if DEF.search(t)}
        if any(r[3] not in ("match", "match*") and r[0] not in new for r in res) and any(auto_match.NOPROTO.search(t) for t in ok):
            # a NOPROTO_ define broke a function already there: leave out
            # the ones that brought defines
            ok = [t for t in ok if not auto_match.NOPROTO.search(t)]
            write(p, compose(base, ok))
            _, _, res = match.run_checks([p], img, symtab, slots, say=lambda *a, **k: None) if ok else (None, None, [])
        good = {r[0] for r in res if r[3] == "match"}
        if os.environ.get("VF4FT_ADOPT_DEBUG"):
            print(f"DEBUG settle {p}: {len(ok)} compiled; not match: "
                  f"{[(r[0], r[3]) for r in res if r[3] != 'match'][:8]}", file=sys.stderr)
        ok = [t for t in ok if DEF.search(t) and defname(t) in good]
        write(p, compose(base, ok))
    if not ok:
        if orig is None:
            os.remove(p)
        else:
            open(p, "w").write(orig)
    return ok


def unmapped_file(a, g):
    """source/ path (from source/) for a function at a in build group g: the
    group's file, or the split of it (a "split-from:" file) that a is in."""
    best = g[0]
    d = os.path.join(program.SOURCE, "_unmapped")
    for f in os.listdir(d) if os.path.isdir(d) else []:
        m = re.fullmatch(r"(0c[0-9a-f]{6})\.c", f)
        if m and best < int(m.group(1), 16) <= a:
            head = open(os.path.join(d, f)).read(2000)
            if re.search(r"split-from: %08x\b" % g[0], head):
                best = int(m.group(1), 16)
    return f"_unmapped/{best:08x}.c"


def main():
    args = sys.argv[1:]
    texts = None
    if "--texts" in args:
        import json
        texts = {int(k, 16): v for k, v in json.load(open(args[args.index("--texts") + 1])).items()}
        args = [a for i, a in enumerate(args) if a != "--texts" and (i == 0 or args[i - 1] != "--texts")]
    res = args[0] if args else os.path.join(ROOT, "build", "automatch", "results.tsv")
    want = {}
    tally = Counter()
    for l in open(res).read().split("\n")[1:]:
        f = l.split("\t")
        if len(f) == 6:
            tally[f[5]] += 1
            a = int(f[0], 16)
            if (texts is None and f[5] == "match") or (texts is not None and a in texts):
                want[a] = None
    global DECLS
    DECLS = pin_globals.declarations(None)
    ctx = (open(program.IMAGE, "rb").read(), match.load_symbols(), match.game_slots())
    groups = delay_slots.groups()
    group_of = lambda a: next((g for g in groups if g[0] <= a <= g[1]), None)
    # each function's flags from its build group, as auto_match picks them,
    # not from its results row: a skipped row says "-" whatever the group is
    kind = auto_match.kind_of(groups)
    want = {a: "-fno-delayed-branch" if kind(a) == "empty" else "-" for a in want}
    cands = {c[0]: c for c in auto_match.candidates()}
    if texts is not None:
        cands = {a: (c[0], c[1], c[2], c[3], texts[a]) if a in texts else c for a, c in cands.items()}
    files = defaultdict(list)
    for a, flags in sorted(want.items()):
        c = cands.get(a)
        if c is None:
            tally["gone from src/ (renamed or already in source/)"] += 1
            continue
        path = c[2]
        if path.startswith("_unmapped/"):
            path = unmapped_file(a, group_of(a))
        files[path].append((c, "" if flags == "-" else flags))
    moved = 0
    for path, items in sorted(files.items()):
        p = os.path.join(program.SOURCE, path)
        flags = {f for _, f in items}
        orig = None
        if os.path.exists(p):
            s = orig = open(p).read()
            have = set(DEF.findall(s))
            m = re.search(r"match-flags:\s*(.*?)\s*(\*/)?$", s, re.M)
            file_flags = m.group(1) if m else ""
        else:
            s, have, file_flags = None, set(), next(iter(flags))
        add, addr = [], {}
        for c, f in items:
            if f != file_flags:
                tally[f"flags differ from {path}'s"] += 1
                continue
            if c[3] in have:
                continue
            t = one_line(c[4])
            if not DEF.search(t):
                tally["left out: not a definition adopt can read"] += 1
                continue
            add.append(t)
            addr[t] = c[0]
            sync_declaration(t)
        if s is not None and add and not compiles(p, s, []):
            # a new signature the file's own callers do not fit (they pass
            # more arguments): leave out only the functions whose sync breaks
            # the file, not every function headed for it
            for t in add:
                undo_sync(t)
            bad = []
            for t in add:
                sync_declaration(t)
                if t in SYNC_UNDO and not compiles(p, s, []):
                    undo_sync(t)
                    bad.append(t)
            tally["left out: its new signature breaks callers in its file"] += len(bad)
            add = [t for t in add if t not in bad]
            open(p, "w").write(orig)
        if not add:
            continue
        if s is None:
            os.makedirs(os.path.dirname(p), exist_ok=True)
            s = header(path, file_flags)
        kept = settle(p, s, add, ctx, orig)
        left = [t for t in add if t not in kept]
        if left and os.path.exists(p):
            again = repin(p, kept, left, ctx)
            tally["matched once its globals were declared with today's types"] += len(again)
            kept += again
        # in an _unmapped/ chunk, a function that matches only without a
        # declaration its neighbours need was in another original file: a
        # file of its own, named by its address (one per set of defines)
        if path.startswith("_unmapped/"):
            groups = defaultdict(list)
            for t in add:
                if t not in kept and auto_match.NOPROTO.search(t):
                    groups["".join(auto_match.NOPROTO.findall(t))].append(t)
            for g in groups.values():
                own = f"_unmapped/{min(addr[t] for t in g):08x}.c"
                q = os.path.join(program.SOURCE, own)
                if os.path.exists(q) or own == path:
                    continue
                more = settle(q, header(own, file_flags), g, ctx, None)
                tally["matched in a file of their own (no declaration there)"] += len(more)
                kept += more
        for t in add:
            if t not in kept:
                undo_sync(t)
        moved += len(kept)
        tally["left out: did not compile or match in its file"] += len(add) - len(kept)
    if SYNCED:
        # a new declaration changes how its callers compile (a narrower
        # parameter is converted before the call): check every file
        files = [os.path.join(d, f) for d, _, fs in os.walk(program.SOURCE) for f in fs if f.endswith(".c")]
        broken_files = []
        for f in files:
            try:
                _, _, res = match.run_checks([f], *ctx, say=lambda *a, **k: None)
            except SystemExit:
                res = [(None, 0, 0, "compile failed")]
            # match* matches too (a relocation it could not check)
            if any(r[3] not in ("match", "match*") for r in res):
                broken_files.append(f)
        still = repair(broken_files, ctx) if broken_files else []
        if still:
            print(f"WARNING: after declaring {', '.join(SYNCED)} anew, these files no longer match: "
                  + ", ".join(os.path.relpath(f, ROOT) for f in still))
    for k, v in tally.most_common():
        if v:
            print(f"{v:6d}  {k}")
    print(f"moved {moved} functions into {os.path.relpath(program.SOURCE, ROOT)}/ ({len(files)} files)")


if __name__ == "__main__":
    main()
