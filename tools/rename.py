"""Rename functions or globals everywhere the repo spells them by hand:
source/ (and source_test/) C, headers and assembly, ghidra/symbols/*.tsv
declarations and ghidra/types/*.h. src/ is not touched: the next
`ghidra/run.sh types` + `export` regenerates it from Ghidra, which takes the
new name from the symbol table row that carries it.

    python3 -I tools/rename.py RENAMES.tsv [--dry-run]

RENAMES.tsv: old<TAB>new per line (a header line "old<TAB>new" is skipped).
Spellings rewritten with the name: the bare name, the assembler's leading
underscore (_FUN_x), and the prototype guards NOPROTO_x / KRPROTO_x. A
longer name that merely contains the old one (thunk_FUN_x, FUN_x_1) is
left alone. The new name must be a C identifier, not a keyword, and not
already used anywhere it would be written.
"""
import os, re, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
KEYWORDS = set("""auto break case char const continue default do double else enum extern float for goto
if inline int long register return short signed sizeof static struct switch typedef union unsigned void
volatile while""".split())


def files():
    for top, exts in (("source", (".c", ".h", ".s")), ("source_test", (".c", ".h", ".s")),
                      (os.path.join("ghidra", "symbols"), (".tsv",)), (os.path.join("ghidra", "symbols_test"), (".tsv",)),
                      (os.path.join("ghidra", "types"), (".h",))):
        for d, _, fs in os.walk(os.path.join(ROOT, top)):
            for f in sorted(fs):
                if f.endswith(exts):
                    yield os.path.join(d, f)


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    dry = "--dry-run" in sys.argv
    pairs = []
    for line in open(args[0], encoding="utf-8"):
        c = line.rstrip("\n").split("\t")
        if len(c) < 2 or c[0] == "old" or not c[0]:
            continue
        old, new = c[0].strip(), c[1].strip()
        if not re.fullmatch(r"[A-Za-z_]\w*", new) or new in KEYWORDS:
            sys.exit(f"not a usable C name: {new!r}")
        pairs.append((old, new))
    olds = [o for o, _ in pairs]
    news = [n for _, n in pairs]
    if len(set(olds)) != len(olds) or len(set(news)) != len(news):
        sys.exit("an old or new name appears twice")
    texts = {p: open(p, encoding="utf-8", errors="surrogateescape").read() for p in files()}
    # symbol tables may already carry the new name: that is the row giving it
    # string literals and comments are not uses (a game string often names its object)
    lit = re.compile(r'"(?:[^"\\\n]|\\.)*"|/\*.*?\*/|//[^\n]*', re.S)
    code = [lit.sub(" ", t) for p, t in texts.items() if os.sep + "symbols" not in p]
    taken = [n for n in news if any(re.search(r"(?<![A-Za-z0-9_])_?" + re.escape(n) + r"(?![A-Za-z0-9_])", t) for t in code)]
    if taken:
        sys.exit("new names already in use: " + ", ".join(taken))
    table = dict(pairs)
    pat = re.compile(r"(?<![A-Za-z0-9_])(_|NOPROTO_|KRPROTO_)?(" + "|".join(map(re.escape, sorted(olds, key=len, reverse=True))) + r")(?![A-Za-z0-9_])")
    changed, count = 0, 0
    for p, t in texts.items():
        n = [0]
        test_file = os.sep + "source_test" + os.sep in p or os.sep + "symbols_test" + os.sep in p

        def sub(m):
            # test.bin's files spell its own addresses: main.bin's FUN_/DAT_ <addr> is another object there
            if test_file and re.fullmatch(r"[A-Za-z]+_[0-9a-f]{8}", m.group(2)):
                return m.group(0)
            n[0] += 1
            return (m.group(1) or "") + table[m.group(2)]
        u = pat.sub(sub, t)
        if u != t:
            changed += 1
            count += n[0]
            if not dry:
                open(p, "w", encoding="utf-8", errors="surrogateescape").write(u)
    missing = [o for o in olds if not any(re.search(r"(?<![A-Za-z0-9_])_?" + re.escape(o) + r"(?![A-Za-z0-9_])", t) for t in texts.values())]
    print(f"{len(pairs)} names, {count} spellings in {changed} files" + (" (dry run)" if dry else ""))
    if missing:
        print("not found anywhere (only in Ghidra?):", ", ".join(missing))


main()
