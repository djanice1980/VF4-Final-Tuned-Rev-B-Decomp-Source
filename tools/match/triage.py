"""Group the functions whose decompiled C does not compile by why.

    python3 -I tools/match/triage.py [--from results.tsv] [--jobs N] [--out FILE.tsv]

Each "no compile" function of an auto_match.py results file is compiled on
its own (with the flags and headers auto_match uses) and its first error is
kept, with names, numbers and quoted identifiers replaced by placeholders so
that errors of one kind group together. Prints the kinds, most common first,
with an example of each; writes FILE (default build/automatch/triage.tsv):
address, name, kind, the error as GCC printed it, and the source line.
"""
import multiprocessing, os, re, sys, tempfile
from collections import Counter

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import program
import auto_match as am
import match

ROOT = am.ROOT


def kind(msg):
    k = re.sub(r"`[^']*'", "`X'", msg)
    k = re.sub(r"\b0x[0-9a-f]+\b|\b\d+\b", "N", k)
    k = re.sub(r"\b(?:FUN|DAT|PTR|LAB)_\w+", "NAME", k)
    return k.strip()


def one(job):
    a, name, flags, text = job
    with tempfile.TemporaryDirectory(dir=os.path.join(ROOT, "build")) as tmp:
        f = os.path.join(tmp, "one.c")
        am.write_batch(f, [(None, None, None, name, text)], [] if flags == "-" else [flags])
        src = open(f).read().split("\n")
        try:
            match.compile_c(f, tmp)
            return (a, name, "compiles now", "", "")
        except SystemExit as e:
            err = str(e)
    for line in err.split("\n"):
        m = re.match(r".*?:(\d+): (?:error: )?(.*)", line)
        if m and "warning" not in line and "In function" not in line:
            n = int(m.group(1))
            return (a, name, kind(m.group(2)), m.group(2).strip(), src[n - 1].strip() if 0 < n <= len(src) else "")
    return (a, name, "other: " + kind(err.split("\n")[-1])[:80], err.strip().split("\n")[-1][:200], "")


def main():
    args = sys.argv[1:]
    opt = lambda k, d: type(d)(args[args.index(k) + 1]) if k in args else d
    src = opt("--from", os.path.join(ROOT, "build", "automatch", "results.tsv"))
    out = opt("--out", os.path.join(ROOT, "build", "automatch", "triage.tsv"))
    jobs = opt("--jobs", max(1, (os.cpu_count() or 2) - 4))
    rows = [l.split("\t") for l in open(src).read().split("\n")[1:] if l.count("\t") == 5]
    want = {int(r[0], 16): r[4] for r in rows if r[5] == "no compile"}
    work = [(c[0], c[3], want[c[0]], c[4]) for c in am.candidates() if c[0] in want]
    with multiprocessing.Pool(jobs) as p:
        res = sorted(p.imap_unordered(one, work))
    with open(out, "w") as w:
        w.write("address\tname\tkind\terror\tline\n")
        for a, n, k, e, l in res:
            w.write(f"0x{a:08x}\t{n}\t{k}\t{e}\t{l}\n")
    tally, ex = Counter(r[2] for r in res), {}
    for r in res:
        ex.setdefault(r[2], r)
    print(f"{len(res)} functions that did not compile -> {out}")
    for k, n in tally.most_common(25):
        a, name, _, e, l = ex[k]
        print(f"{n:5}  {k}\n         e.g. 0x{a:08x} {name}: {l[:110]}")


if __name__ == "__main__":
    main()
