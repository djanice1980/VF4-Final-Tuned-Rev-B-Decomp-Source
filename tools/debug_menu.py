#!/usr/bin/env python3
"""Extract the debug-menu item records from main.bin.

Each item is a 0x58-byte record in .rodata. Layout as measured over all
1,456 records (fields never seen non-zero are omitted):

  +0x00 u32   1 for integer items, 0 for float items
  +0x04       step   (int, or float bits for float items)
  +0x08       min    (same)
  +0x0C       max    (same; for enum lists: entry count - 1)
  +0x10 u32   always 4
  +0x14 char* label, e.g. "*CV_TYPE_0"
  +0x24 u32   type word: kind << 24 | flags << 8 | size code
  +0x28 u32   1 when +0x2C holds a name table
  +0x2C char** enum name table (kind 0x0F)
  +0x30       target: absolute address, or a struct offset if flag 0x01
  +0x54       pointer to the record itself (used here to find records)

Kinds (from variable spacing and step/min/max values):
  0x00 float        0x07 integer       0x08 integer (0..255 style ranges)
  0x09 byte         0x0A 16-bit        0x0B 16-bit
  0x0E bit flag (bit number in flags)  0x0F enum list (names at +0x2C)
Size code for 0x07/0x08/0x0F: 0 = 1 byte, 1 = 2 bytes, 2 = 4 bytes.

usage: debug_menu.py <main.bin> <out.tsv>
"""
import struct
import sys

BASE = 0x0C020000
REC = 0x58


def main():
    data = open(sys.argv[1], "rb").read()

    def u32(a):
        return struct.unpack_from("<I", data, a - BASE)[0]

    def cstr(a):
        off = a - BASE
        if not 0 <= off < len(data):
            return None
        end = data.find(b"\0", off, off + 80)
        if end <= off:
            return None
        raw = data[off:end]
        return raw.decode("ascii") if all(0x20 <= b < 0x7F for b in raw) else None

    rows = []
    for a in range(BASE, BASE + len(data) - REC, 4):
        if u32(a + 0x54) != a:
            continue
        label = cstr(u32(a + 0x14))
        if label is None:
            continue
        tw = u32(a + 0x24)
        kind, flags, size = tw >> 24, (tw >> 8) & 0xFF, tw & 0xFF
        target = u32(a + 0x30)
        absolute = not (flags & 1) and 0x0C000000 <= target < 0x0E000000
        table = u32(a + 0x2C) if u32(a + 0x28) == 1 else 0
        maxv = u32(a + 0x0C)
        rows.append((a, label, kind, flags, size, target, absolute, table, maxv))

    with open(sys.argv[2], "w") as out:
        out.write("record\tlabel\tkind\tflags\tsize\ttarget\tabsolute\ttable\tmax\n")
        for a, label, kind, flags, size, target, absolute, table, maxv in rows:
            out.write(f"{a:08x}\t{label}\t{kind:02x}\t{flags:02x}\t{size}\t{target:08x}\t"
                      f"{int(absolute)}\t{table:08x}\t{maxv:08x}\n")
    print(f"{len(rows)} records, {sum(r[6] for r in rows)} with absolute targets, "
          f"{sum(1 for r in rows if r[7])} enum lists -> {sys.argv[2]}")


if __name__ == "__main__":
    main()
