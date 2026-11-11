"""Take functions out of source/ (so auto_match can try their new decompile).

    python3 -I tools/match/unadopt.py NAME [NAME ...]
    python3 -I tools/match/unadopt.py --changed-returns     (ghidra/returns.tsv: "set ..." / "passes on")

For when a function's signature changes in Ghidra after it matched (a
return found by ghidra/AssignReturns.java): its definition in source/ still
has the old one, and the export declares it as defined, so its callers would
keep the old view. Removing it lets auto_match.py compile the new decompile;
adopt.py puts it back if that matches. A file left with no functions is
removed. Hand-written files are not touched (a name found only in one is
reported).
"""
import os, re, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import program

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
DEF = re.compile(r"^[A-Za-z_][^\n;]*?\b([A-Za-z_]\w*)\([^\n;]*?\)(?: [^\n{]*;)?\n\{.*?\n\}\n", re.M | re.S)
AUTO = "Decompiler output that compiles to the game's bytes"


def main():
    args = sys.argv[1:]
    names = set(a for a in args if not a.startswith("--"))
    if "--changed-returns" in args:
        for l in open(os.path.join(ROOT, "ghidra", "returns.tsv")).read().split("\n")[1:]:
            f = l.split("\t")
            if len(f) > 5 and (f[5].startswith("set ") or "passes on" in f[5]):
                names.add(f[1])
        log = os.path.join(ROOT, "build", "all.log")
        if os.path.exists(log):  # "returns: X returns what Y passes on" lines from the last build
            names |= set(re.findall(r"returns: (\w+) returns what \w+ passes on", open(log, errors="replace").read()))
    removed, hand = 0, []
    for d, _, fs in os.walk(program.SOURCE):
        for fn in fs:
            if not fn.endswith(".c"):
                continue
            p = os.path.join(d, fn)
            s = open(p).read()
            hits = [m for m in DEF.finditer(s) if m.group(1) in names]
            if not hits:
                continue
            if AUTO not in s:
                hand += [m.group(1) for m in hits]
                continue
            for m in reversed(hits):
                s = s[:m.start()] + s[m.end():]
                removed += 1
            if DEF.search(s):
                open(p, "w").write(re.sub(r"\n{3,}", "\n\n", s))
            else:
                os.remove(p)
    print(f"removed {removed} definitions from source/")
    if hand:
        print("in hand-written files, left alone: " + ", ".join(sorted(hand)))


if __name__ == "__main__":
    main()
