"""How the stack frames of the "frame" near misses differ from the game's.

    python3 -I tools/match/frames.py [--kind frame] [--from results.tsv] [--jobs N]

For the near misses classify.py put in one kind (default "frame"), each
function's current C is compiled and the prologue of both sides is read up
to the frame pointer (mov r15,r14): the registers pushed (mov.l rN,@-r15,
sts.l pr,@-r15, fmov.s frN,@-r15) and the space reserved (add #-N,r15, or
a constant subtracted). GCC 3.2 saves a callee-saved register only when a
value lives in it across a call, so more saved registers mean more values
kept alive (locals the decompiler merged or dropped); more space means
bigger locals (an array, a struct on the stack). Prints how often each
differs and by how much, and writes build/automatch/frames.tsv (address,
game's saved registers and bytes, ours) for variants.py's framefit rule.
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


def prologue(dump):
    """(saved registers, bytes reserved) from an objdump listing."""
    lines = [re.sub(r"\s+!.*$", "", l) for l in dump.split("\n") if re.match(r"\s+[0-9a-f]+:", l)]
    saved, space, consts = [], 0, {}
    for l in lines[:40]:
        ins = l.split("\t")[2:] if l.count("\t") >= 2 else []
        if not ins:
            continue
        mn, ops = ins[0].strip(), (ins[1].strip() if len(ins) > 1 else "")
        m = re.fullmatch(r"(\w+),@-r15", ops)
        if mn in ("mov.l", "sts.l", "fmov.s", "fmov") and m:
            saved.append(m.group(1))
            continue
        m = re.fullmatch(r"#(-?\d+),r15", ops)
        if mn == "add" and m:
            space -= int(m.group(1))
            continue
        m = re.fullmatch(r"#(-?\d+),(r\d+)", ops)
        if mn == "mov" and m:
            consts[m.group(2)] = int(m.group(1))
        m = re.fullmatch(r"(r\d+),r15", ops)
        if mn == "sub" and m and m.group(1) in consts:
            space += consts[m.group(1)]
        if mn == "mov" and ops == "r15,r14":
            break
    return saved, space


def one(job):
    a, size, name, flags, text = job
    with tempfile.NamedTemporaryFile(suffix=".bin") as f:
        f.write(CTX["img"][a - B:a - B + size])
        f.flush()
        g = subprocess.run(["sh4-elf-objdump", "-D", "-b", "binary", "-m", "sh4", "-EL", f"--adjust-vma={a:#x}", f.name],
                           capture_output=True, text=True).stdout
    with tempfile.TemporaryDirectory(dir=os.path.join(ROOT, "build")) as tmp:
        t = re.sub(r"\b" + re.escape(name) + r"\b(?=\s*\()", f"V0_{a:08x}", text, count=1)
        src = os.path.join(tmp, "b.c")
        am.write_batch(src, [(None, None, None, None, t)], [] if flags == "-" else [flags])
        try:
            o = match.compile_c(src, tmp)
        except SystemExit:
            return None
        c = subprocess.run(["sh4-elf-objdump", "-d", o], capture_output=True, text=True).stdout
    return (a, name, prologue(g), prologue(c))


def main():
    args = sys.argv[1:]
    opt = lambda k, d: type(d)(args[args.index(k) + 1]) if k in args else d
    kind = opt("--kind", "frame")
    src = opt("--from", os.path.join(ROOT, "build", "automatch", "results.tsv"))
    jobs = opt("--jobs", max(1, (os.cpu_count() or 2) - 4))
    flags = {int(l.split("\t")[0], 16): l.split("\t")[4] for l in open(src).read().split("\n")[1:] if l.count("\t") == 5}
    rows = [r for r in csv.DictReader(open(os.path.join(ROOT, "build", "automatch", "classify.tsv")), delimiter="\t")
            if r["kind"] == kind]
    cands = {c[0]: c for c in am.candidates()}
    CTX["img"] = open(program.IMAGE, "rb").read()
    work = [(c[0], c[1], c[3], flags.get(c[0], "-"), c[4]) for c in (cands.get(int(r["address"], 16)) for r in rows) if c]
    with multiprocessing.get_context("fork").Pool(jobs) as p:
        res = [x for x in p.map(one, work) if x]
    with open(os.path.join(ROOT, "build", "automatch", "frames.tsv"), "w") as w:
        w.write("address\tname\tgame saved\tgame bytes\tour saved\tour bytes\n")
        for a, n, (gs, gb), (cs, cb) in res:
            w.write(f"0x{a:08x}\t{n}\t{','.join(gs)}\t{gb}\t{','.join(cs)}\t{cb}\n")
    what, regs, space = Counter(), Counter(), Counter()
    for a, n, (gs, gb), (cs, cb) in res:
        gc = [r for r in gs if re.fullmatch(r"r(8|9|1[0-3])|fr1[2-5]", r)]
        cc = [r for r in cs if re.fullmatch(r"r(8|9|1[0-3])|fr1[2-5]", r)]
        k = []
        if len(gc) != len(cc):
            k.append("game saves more registers" if len(gc) > len(cc) else "game saves fewer registers")
            regs[len(gc) - len(cc)] += 1
        elif gc != cc:
            k.append("other registers saved")
        if gb != cb:
            k.append("game reserves more space" if gb > cb else "game reserves less space")
            space[gb - cb] += 1
        what[" + ".join(k) or "same prologue (the epilogue or body differs)"] += 1
    print(f"{len(res)} '{kind}' near misses")
    for k, n in what.most_common():
        print(f"{n:5}  {k}")
    print("saved registers, game minus ours:", sorted(regs.items()))
    print("bytes reserved, game minus ours (most common):", space.most_common(12))


if __name__ == "__main__":
    main()
