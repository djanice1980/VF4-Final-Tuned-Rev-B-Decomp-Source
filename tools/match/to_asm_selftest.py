"""Run tools/match/to_asm.py's region and listing steps over every hand-written
routine already in source/asm/, writing nothing to the tree:

    python3 -I tools/match/to_asm_selftest.py [-v]

Every routine in source/ matches, so to_asm.py itself has nothing left to
do; this checks it still could. For each source/asm/*.s file, the routines
the file defines are taken as the unmatched candidates (and every other
function as matched), and the result is counted as:

  same    the span to_asm finds is the file's span, and its listing
          assembles to the game's bytes (build() checks that);
  short   the span ends before the file's: on 2026-11-12 (188 same, 13
          short, 4 errors of 205) every short file ends in data words no
          instruction loads pc-relative (a jump table, words read through
          a register or stored to, named data objects added to the file),
          which control flow cannot find;
  long    the span runs past the file's;
  error   region() or build() stops; the four on 2026-11-12 are files that
          start with data (no function there to start from).
"""
import os, re, sys, tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import to_asm
import program
import match

HEAD = re.compile(r"(?ms)^/\* (0x[0-9a-f]{8})  size (0x[0-9a-f]+).*?\*/\n(.*?)\n")


def all_functions():
    """address -> (size or extent, name, matched-by .s file or None), from src/."""
    out = {}
    for d, _, fs in os.walk(program.SRC):
        for f in fs:
            if not f.endswith(".c"):
                continue
            text = open(os.path.join(d, f), errors="replace").read()
            for m in re.finditer(r"(?ms)^/\* 0x([0-9a-f]{8})  size 0x([0-9a-f]+)(.*?)\*/\n(.*?)(?=\n)", text):
                head, sig = m.group(3), m.group(4)
                ext = re.search(r"(?m)^ \* extent: 0x([0-9a-f]+)$", head)
                size = int(ext.group(1), 16) if ext else int(m.group(2), 16)
                nm = re.search(r"\b([A-Za-z_]\w*)\s*\(", sig)
                src = re.search(r"(?m)^ \* matched: (source/asm/[0-9a-f]+\.s)", head)
                out[int(m.group(1), 16)] = (size, nm.group(1) if nm else "?", src.group(1) if src else None)
    return out


def file_span(path):
    first = open(path).readline()
    m = re.match(r"! 0x([0-9a-f]+)\s+size 0x([0-9a-f]+)", first)
    return int(m.group(1), 16), int(m.group(1), 16) + int(m.group(2), 16)


def main():
    verbose = "-v" in sys.argv
    img = open(program.IMAGE, "rb").read()
    funcs = all_functions()
    names = to_asm.name_table()
    files = sorted(f for f in os.listdir(os.path.join(program.SOURCE, "asm")) if f.endswith(".s"))
    counts = {"same": 0, "short": 0, "long": 0, "error": 0}
    for f in files:
        rel = "source/asm/" + f
        lo, hi = file_span(os.path.join(program.SOURCE, "asm", f))
        cands = {a: (s, n) for a, (s, n, src) in funcs.items() if src == rel}
        if lo not in cands:
            counts["error"] += 1
            print(f"error  {f}: no function at its start in src/")
            continue
        matched = set(a for a, (s, n, src) in funcs.items() if src != rel)
        try:
            end, entries = to_asm.region(img, lo, cands, matched)
            note = f"0x{lo:08x}  size 0x{end - lo:x}  " + ", ".join(e[1] for e in entries)
            to_asm.build(img, lo, end, entries, names, note)
        except SystemExit as e:
            counts["error"] += 1
            print(f"error  {f}: {e}")
            continue
        kind = "same" if end == hi else ("short" if end < hi else "long")
        counts[kind] += 1
        if kind != "same" or verbose:
            print(f"{kind:6} {f}: to_asm spans {lo:08x}-{end:08x}, the file {lo:08x}-{hi:08x}")
    print(" ".join(f"{k} {v}" for k, v in counts.items()), f"of {len(files)} files")


if __name__ == "__main__":
    main()
