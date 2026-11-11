"""Which functions the game calls that our compile does not, and the reverse.

    python3 -I tools/match/callees.py [--kind calls] [--from results.tsv] [--jobs N] [--callee NAME]

For the near misses classify.py put in one kind (default "calls": a call
more or fewer), each function's current C is compiled and both sides' calls
are listed: a jsr through a register loaded from the constant pool (the
pool word, relocated on our side, gives the callee) or a bsr. Prints the
callees found on one side only, with the number of functions each turns up
in: a compiler helper GCC calls for one spelling and not another
(__fixunssfsi for a float converted to unsigned) shows at the top.
"(indirect)" is a call whose address was not loaded just before it (kept
in a register across calls, or a function pointer). --callee NAME lists
the functions where NAME is called on one side only, smallest first.
"""
import multiprocessing, os, re, subprocess, sys, tempfile, csv
from collections import Counter

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import program
import auto_match as am
import match

ROOT = am.ROOT
B = match.BASE
CTX = {}


def calls_in(dump, pool_value):
    """Callees of the jsr / bsr in an objdump listing; pool_value(text) -> callee."""
    lines = [l for l in dump.split("\n") if re.match(r"\s+[0-9a-f]+:", l)]
    out = []
    for i, l in enumerate(lines):
        m = re.search(r"\tjsr\t@(r\d+)", l)
        if m:
            for p in reversed(lines[max(0, i - 8):i]):
                v = pool_value(p, m.group(1))
                if v is not None:
                    out.append(v)
                    break
            else:
                out.append("(indirect)")
        m = re.search(r"\tbsr\t0x([0-9a-f]+)", l)
        if m:
            out.append(CTX["name"](int(m.group(1), 16)))
    return out


def one(job):
    a, size, name, flags, text = job
    img = CTX["img"]
    with tempfile.NamedTemporaryFile(suffix=".bin") as f:
        f.write(img[a - B:a - B + size])
        f.flush()
        d = subprocess.run(["sh4-elf-objdump", "-D", "-b", "binary", "-m", "sh4", "-EL", f"--adjust-vma={a:#x}", f.name],
                           capture_output=True, text=True).stdout

    def game_pool(line, reg):
        mm = re.search(r"mov\.l\t0x([0-9a-f]+),(r\d+)", line)
        if mm and mm.group(2) == reg:
            pa = int(mm.group(1), 16)
            return CTX["name"](int.from_bytes(img[pa - B:pa - B + 4], "little"))
        return None
    game = calls_in(d, game_pool)
    with tempfile.TemporaryDirectory(dir=os.path.join(ROOT, "build")) as tmp:
        t = re.sub(r"\b" + re.escape(name) + r"\b(?=\s*\()", f"V0_{a:08x}", text, count=1)
        src = os.path.join(tmp, "b.c")
        am.write_batch(src, [(None, None, None, None, t)], [] if flags == "-" else [flags])
        try:
            o = match.compile_c(src, tmp)
        except SystemExit:
            return None
        d = subprocess.run(["sh4-elf-objdump", "-dr", o], capture_output=True, text=True).stdout
    relocs = dict(re.findall(r"^\s*([0-9a-f]+): R_SH_DIR32\s+_?(\w+)", d, re.M))

    def our_pool(line, reg):
        mm = re.search(r"mov\.l\t([0-9a-f]+) <[^>]*>,(r\d+)", line)
        if mm and mm.group(2) == reg:
            return relocs.get(mm.group(1), "(indirect)")
        return None
    mine = calls_in(d, our_pool)
    g, m = Counter(game), Counter(mine)
    return (a, name, g - m, m - g)


def main():
    args = sys.argv[1:]
    opt = lambda k, d: type(d)(args[args.index(k) + 1]) if k in args else d
    kind = opt("--kind", "calls")
    src = opt("--from", os.path.join(ROOT, "build", "automatch", "results.tsv"))
    jobs = opt("--jobs", max(1, (os.cpu_count() or 2) - 4))
    flags = {int(l.split("\t")[0], 16): l.split("\t")[4] for l in open(src).read().split("\n")[1:] if l.count("\t") == 5}
    rows = [r for r in csv.DictReader(open(os.path.join(ROOT, "build", "automatch", "classify.tsv")), delimiter="\t")
            if r["kind"] == kind]
    cands = {c[0]: c for c in am.candidates()}
    symtab = match.load_symbols()
    by = {}
    for n, v in symtab.items():
        by.setdefault(v, n)
    CTX.update(img=open(program.IMAGE, "rb").read(), name=lambda v: by.get(v, f"FUN_{v:08x}"))
    work = [(c[0], c[1], c[3], flags.get(c[0], "-"), c[4]) for c in (cands.get(int(r["address"], 16)) for r in rows) if c]
    with multiprocessing.get_context("fork").Pool(jobs) as p:
        res = [x for x in p.map(one, work) if x]
    if "--callee" in args:
        want = opt("--callee", "")
        hits = sorted((x for x in res if want in x[2] or want in x[3]), key=lambda x: cands[x[0]][1])
        for a, n, g, m in hits:
            print(f"0x{a:08x} {n:<28} size {cands[a][1]:5}  game {g[want]}  ours {m[want]}")
        return
    fg, fm = Counter(), Counter()
    for _, _, g, m in res:
        fg.update(set(g))
        fm.update(set(m))
    print(f"{len(res)} '{kind}' near misses")
    print("called only by the game (functions):  " + ", ".join(f"{k} {n}" for k, n in fg.most_common(15)))
    print("called only by our compile (functions): " + ", ".join(f"{k} {n}" for k, n in fm.most_common(15)))


if __name__ == "__main__":
    main()
