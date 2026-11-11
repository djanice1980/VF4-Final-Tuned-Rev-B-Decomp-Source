"""Which register each call loads its callee's address into, per callee.

    python3 -I tools/match/call_registers.py [--callee NAME|0xADDR ...] [--tsv]

GCC 3.2 (SH) loads a call's address into r0 when the callee is declared to
return an integer or pointer, or not declared at all (implicitly int), and
into r1 when nothing comes back in r0 (void, float, double, a struct);
checked by compiling each. So in GCC-built code a call records what its
file's declaration said. ghidra/AssignReturns.java acts on the same signal. Only a load right before the jsr
(mov.l @(disp,pc),rN within the 7 instructions before jsr @rN) counts; an
address kept in r8-r13 for several calls says nothing. Assembly (no GCC frame
setup) is left out. Callers' frames and functions come from work/main.bin and
the exported src/ (function headers, src/include/vf4ft_functions.h).
"""
import collections, os, re, struct, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import program

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
B = 0x0C020000


def functions():
    """{address: (size, src file)} and {address: declared return type}."""
    head = re.compile(r"^/\* 0x(0c[0-9a-f]{6})  size 0x([0-9a-f]+) ", re.M)
    fns = {}
    for d, _, fs in os.walk(program.SRC):
        for f in fs:
            if f.endswith(".c"):
                p = os.path.relpath(os.path.join(d, f), program.SRC)
                for m in head.finditer(open(os.path.join(d, f), errors="replace").read()):
                    fns[int(m.group(1), 16)] = (int(m.group(2), 16), p)
    ret, names = {}, {}
    for l in open(os.path.join(program.SRC, "include", "vf4ft_functions.h")):
        m = re.match(r"(.*?)\b(\w+)\s*\(.*\);\s*/\* 0x(0c[0-9a-f]{6}) \*/", l)
        if m:
            a = int(m.group(3), 16)
            ret[a] = m.group(1).strip()
            names[a] = m.group(2)
    return fns, ret, names


def calls(img, fns):
    """[(caller, callee, register)] for GCC-built callers."""
    out = []
    for a, (size, _) in sorted(fns.items()):
        off = a - B
        if not any(struct.unpack_from("<H", img, off + 2 * i)[0] == 0x6EF3 for i in range(24)):
            continue  # no GCC frame setup: assembly
        for pc in range(off, off + size, 2):
            w = struct.unpack_from("<H", img, pc)[0]
            if w & 0xF0FF != 0x400B:
                continue
            n = (w >> 8) & 15
            for back in range(1, 8):
                p = pc - 2 * back
                v = struct.unpack_from("<H", img, p)[0]
                if v >> 12 == 0xD and (v >> 8) & 15 == n:
                    t = struct.unpack_from("<I", img, ((p + 4) & ~3) + (v & 0xFF) * 4)[0]
                    if t in fns:
                        out.append((a, t, n))
                    break
                if (v >> 8) & 15 == n and v >> 12 not in (0x2, 0x3, 0x8):
                    break  # rN written by something else first
    return out


def main():
    img = open(program.IMAGE, "rb").read()
    fns, ret, names = functions()
    votes = collections.defaultdict(collections.Counter)
    for caller, callee, n in calls(img, fns):
        votes[callee]["r0" if n == 0 else "r1" if n == 1 else "other"] += 1
    want = None
    if "--callee" in sys.argv:
        want = set()
        for x in sys.argv[sys.argv.index("--callee") + 1:]:
            if x.startswith("--"):
                break
            want |= {a for a, nm in names.items() if nm == x} | ({int(x, 16)} if x.startswith("0x") else set())
    rows = sorted(votes.items())
    if "--tsv" in sys.argv:
        print("callee\tname\tdeclared return\tr0\tr1\tother")
    for a, c in rows:
        if want is not None and a not in want:
            continue
        if "--tsv" in sys.argv:
            print(f"{a:08x}\t{names.get(a, '?')}\t{ret.get(a, '?')}\t{c['r0']}\t{c['r1']}\t{c['other']}")
        else:
            print(f"0x{a:08x} {names.get(a, '?'):28s} {ret.get(a, '?'):14s} r0 {c['r0']:4d}  r1 {c['r1']:4d}  other {c['other']:3d}")


if __name__ == "__main__":
    main()
