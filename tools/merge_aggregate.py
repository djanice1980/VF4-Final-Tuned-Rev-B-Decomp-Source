"""Rewrite matched code in source/ after AssignAggregates merged smaller
aggregates into a larger object (ghidra/aggregate_extents.tsv).

    python3 -I tools/merge_aggregate.py BASE [--dry]

For every agg_P inside the object at BASE (from ghidra/aggregates.tsv):
  agg_P_P.field_0xN                 -> agg_BASE_BASE.field_0x(P - BASE + N)
  ((volatile) agg_P *)&agg_P_P)->field_0xN   the same through agg_BASE
  extern agg_P agg_P_P; /* ... */   dropped (agg_BASE's extern added once)
Any other use of agg_P is reported and left as it is. match.py decides.

Run it after the types step made the bigger object (and check that
ghidra/signs_words.tsv names no old piece: AssignSigns has to type the
merged members, or signed fields come out undefined4 and compares change).
Only pieces at a GCC split base for words ((P - BASE + 4) % 64 == 0) are
folded: folding 0c41e524 and 0c41e53c inside 0c41e010 breaks five
functions. A function that uses BASE itself and a piece in one body can
change register allocation once both are one symbol (0c13d420 and
0c147300 keep their pieces); put those bodies back as they were.
"""
import csv, glob, os, re, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
base = int(sys.argv[1], 16)
dry = "--dry" in sys.argv
rows = list(csv.DictReader(open(os.path.join(ROOT, "ghidra", "aggregates.tsv")), delimiter="\t"))
size = next(int(r["size"]) for r in rows if int(r["address"], 16) == base)
B = f"agg_{base:08x}"
BV = f"{B}_{base:08x}"
# pieces still named in the headers, or only in source/ (once the merged
# object replaced them, a file's own extern is all that is left)
data = "".join(open(p).read() for p in
               [os.path.join(ROOT, "src", "include", h) for h in ("vf4ft_data.h", "vf4ft_types.h")]
               + glob.glob(os.path.join(ROOT, "source", "**", "*.c"), recursive=True))
inside = sorted({int(a, 16) for a in re.findall(r"\bagg_(0c[0-9a-f]{6})\b", data)
                 if base < int(a, 16) < base + size})
pieces = [p for p in inside if (p - base + 4) % 64 == 0]
print(f"{B}: {size:#x} bytes; pieces {' '.join(f'{p:08x}' for p in pieces) or 'none'}"
      + "".join(f"; {p:08x} kept (not a split base)" for p in inside if p not in pieces))
changed = 0
for path in sorted(glob.glob(os.path.join(ROOT, "source", "**", "*.c"), recursive=True)):
    text = orig = open(path).read()
    for p in pieces:
        P, PV, k = f"agg_{p:08x}", f"agg_{p:08x}_{p:08x}", p - base
        if PV not in text and not re.search(r"\b" + P + r"\b", text):
            continue
        fld = lambda m: f"field_0x{k + int(m.group('n'), 16):x}"
        text = re.sub(r"\(\s*\((?P<v>volatile\s+)?" + P + r"\s*\*\s*\)\s*&" + PV + r"\s*\)\s*->\s*field_0x(?P<n>[0-9a-f]+)",
                      lambda m: f"(({m.group('v') or ''}{B} *)&{BV})->{fld(m)}", text)
        text = re.sub(r"\b" + PV + r"\.field_0x(?P<n>[0-9a-f]+)\b", lambda m: f"{BV}.{fld(m)}", text)
        text = re.sub(r"^extern " + P + " " + PV + r";[^\n]*\n", "", text, flags=re.M)
        for m in re.finditer(r"[^\n]*\b" + P + r"(_" + f"{p:08x}" + r")?\b[^\n]*", text):
            print(f"  {os.path.relpath(path, ROOT)}: left as is: {m.group(0).strip()[:120]}")
    if text != orig:
        if not re.search(r"^extern " + B + " " + BV + ";", text, re.M):
            # with the file's other externs, before the first one
            m = re.search(r"^extern ", text, re.M)
            line = f"extern {B} {BV}; /* 0x{base:08x} */\n"
            text = text[:m.start()] + line + text[m.start():] if m else line + text
        changed += 1
        print(f"  {os.path.relpath(path, ROOT)}: rewritten")
        if not dry:
            open(path, "w").write(text)
print(changed, "files", "(dry run)" if dry else "")
