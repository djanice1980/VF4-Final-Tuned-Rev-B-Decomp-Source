"""Count filled branch delay slots per source file, to tell how each file was built.

    python3 -I tools/match/delay_slots.py [--functions] [--slots PATTERN] [--groups]

GCC 3.2 for SH fills a branch's delay slot with a useful instruction unless
-fno-delayed-branch is given; the game's own code (vf4lib, naomi2lib) leaves
nearly all of them empty, the linked libraries fill most. The branches come
from work/delay_slots.tsv (ghidra/ReportDelaySlots.java, run by the export
step: Ghidra's instructions, so constant pools and jump tables are not
counted), the file of each function from the exported src/.

    --functions       also list every function with a filled slot in a file
                      whose slots are mostly empty
    --slots PATTERN   list the filled slots whose "branch ; slot" text matches
                      the regex PATTERN, with their function
    --groups          instead, the address ranges built alike, in order: C
                      with every slot empty (-fno-delayed-branch), C with
                      them mostly filled, or assembly (no GCC frame setup);
                      a range's edges are object boundaries
"""
import collections, os, re, struct, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import program

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))


def function_files():
    head = re.compile(r"^/\* 0x(0c[0-9a-f]{6})  size ", re.M)
    src = program.SRC
    files = {}
    for dirpath, _, names in os.walk(src):
        for f in names:
            if f.endswith(".c"):
                path = os.path.relpath(os.path.join(dirpath, f), src)
                for m in head.finditer(open(os.path.join(dirpath, f), encoding="utf-8", errors="replace").read()):
                    files[m.group(1)] = path
    return files


def gcc_frame(img, a, end=None):
    """Does the code at a set up GCC's frame pointer (mov r15,r14) early on?
    Not past end, the next function: a short routine (zanzou_vertices_write,
    0x2C bytes) would otherwise find the prologue of the one after it."""
    off = a - 0x0C020000
    n = 24 if end is None else min(24, (end - a) // 2)
    return any(struct.unpack_from("<H", img, off + 2 * i)[0] == 0x6EF3 for i in range(n))


def load():
    """{function: [branches, filled]} from work/delay_slots.tsv."""
    fn = collections.defaultdict(lambda: [0, 0])
    for line in open(program.SLOTS).read().split("\n")[1:]:
        if line:
            _, f, _, slot = line.split("\t")
            fn[f][0] += 1
            fn[f][1] += slot != "nop"
    return fn


def groups():
    """[(first, last, kind, functions)] in address order; functions with one
    branch say little and join whatever surrounds them."""
    img = open(program.IMAGE, "rb").read()
    runs = []
    fns = sorted(load().items())
    starts = [int(f, 16) for f, _ in fns] + [None]
    for i, (f, (n, k)) in enumerate(fns):
        a = int(f, 16)
        if not gcc_frame(img, a, starts[i + 1]):
            kind = "assembly"
        elif n < 2 and not (runs and runs[-1][2] == "assembly"):
            kind = None
        elif n < 2:
            # compiled C after assembly does not join it: its one slot decides
            kind = "C, slots empty" if k == 0 else "C, slots filled"
        else:
            kind = "C, slots empty" if k == 0 else "C, slots filled" if 2 * k >= n else "C, some filled"
        if runs and kind in (None, runs[-1][2]):
            runs[-1][1] = a
            runs[-1][3] += 1
        else:
            runs.append([a, a, kind or "C, slots empty", 1])
    return runs


def shape(text):
    """A slot's instruction with its constants and displacements generalized."""
    return re.sub(r"0x[0-9a-f]+|#-?\d+|\b\d+\b", "N", text)


def main():
    if "--groups" in sys.argv:
        for a, b, kind, n in groups():
            print(f"0x{a:08x}-0x{b:08x}  {kind:16s} {n:5d} functions")
        return
    want_funcs = "--functions" in sys.argv
    pat = re.compile(sys.argv[sys.argv.index("--slots") + 1]) if "--slots" in sys.argv else None
    files = function_files()
    per_file = collections.defaultdict(lambda: [0, 0])
    per_func = collections.defaultdict(lambda: [0, []])
    game = collections.Counter()
    rows = open(program.SLOTS).read().split("\n")[1:]
    for line in rows:
        if not line:
            continue
        branch, fn, mnem, slot = line.split("\t")
        path = files.get(fn, "?")
        per_file[path][0] += 1
        per_func[fn][0] += 1
        if slot != "nop":
            per_file[path][1] += 1
            text = f"{mnem} ; {slot}"
            per_func[fn][1].append((branch, text))
    print(f"{'file':60s} {'branches':>8s} {'filled':>7s}   rate")
    for path, (n, filled) in sorted(per_file.items()):
        print(f"{path:60s} {n:8d} {filled:7d} {100 * filled / n:6.1f}%")
    mostly_empty = lambda path: per_file[path][1] < 0.2 * per_file[path][0]
    for fn, (n, where) in sorted(per_func.items()):
        if where and mostly_empty(files.get(fn, "?")):
            for _, t in where:
                game[shape(t)] += 1
    if want_funcs:
        print("\nfunctions with filled slots in files that are mostly empty (< 20 %):")
        for fn, (n, where) in sorted(per_func.items()):
            if where and mostly_empty(files.get(fn, "?")):
                print(f"  0x{fn} {files.get(fn, '?')}: {len(where)}/{n}: " + "; ".join(t for _, t in where[:4]))
    if pat:
        for fn, (n, where) in sorted(per_func.items()):
            for b, t in where:
                if pat.search(t):
                    print(f"  0x{b} in 0x{fn} {files.get(fn, '?')}: {t}")
    print("\nfilled slots in mostly-empty files, by shape:")
    for k, v in game.most_common(25):
        print(f"  {v:6d}  {k}")


if __name__ == "__main__":
    main()
