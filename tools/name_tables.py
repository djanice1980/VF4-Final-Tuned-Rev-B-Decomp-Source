#!/usr/bin/env python3
"""Find string-pointer tables in main.bin's .rodata/.data.

A run of at least MIN_RUN consecutive 32-bit words that each point at a
printable NUL-terminated string is reported as a table. In this game such
tables are debug names indexed by an enum (MODE_*, stg_*, chr_*), so each
one is a recovered enum definition. Each table is classed as enum
(all-caps identifiers), assets (filenames by index) or text. code_ref
marks entries that a .text literal pool points to: in this binary those
are where one table ends and the next begins (MODE_* stops at entry 26,
STROKE_* starts at 49, both pool-referenced).

usage: name_tables.py <main.bin> <out.tsv>
"""
import re
import struct
import sys

BASE = 0x0C020000
RODATA = 0x0C15DA00
MIN_RUN = 4


def cstr(data, addr):
    off = addr - BASE
    if not 0 <= off < len(data):
        return None
    end = data.find(b"\0", off, off + 128)
    if end <= off:
        return None
    raw = data[off:end]
    # At least two characters, or random words pointing into data match.
    # Pointers into the middle of a string are fine: the linker tail-merges
    # strings ("HIT" is a pointer into "NO HIT").
    if len(raw.strip()) < 2 or not all(0x20 <= b < 0x7F for b in raw):
        return None
    return raw.decode("ascii")


IDENT = re.compile(r"^[A-Z][A-Z0-9_]*$")
FILE = re.compile(r"^[\w./-]+\.(bin|gz|aet|pvr|txt|dat)(\.gz)?$", re.I)


def classify(names):
    """enum: all-caps identifiers; assets: filenames; text: anything else."""
    stripped = [n.strip() for n in names]
    if sum(bool(IDENT.match(n)) for n in stripped) >= 0.9 * len(stripped):
        return "enum"
    if sum(bool(FILE.match(n)) for n in stripped) >= 0.5 * len(stripped):
        return "assets"
    return "text"


def text_refs(data):
    """Addresses that some 32-bit word in .text points to (literal pools)."""
    refs = set()
    for i in range(0, RODATA - BASE, 4):
        refs.add(struct.unpack_from("<I", data, i)[0])
    return refs


def main():
    data = open(sys.argv[1], "rb").read()
    code = text_refs(data)
    tables = []
    i = RODATA - BASE
    while i + 4 <= len(data):
        run = []
        j = i
        while j + 4 <= len(data):
            s = cstr(data, struct.unpack_from("<I", data, j)[0])
            if s is None:
                break
            run.append(s)
            j += 4
        # runs of mostly identical entries are data that happens to point
        # at one short string, not a table
        if len(run) >= MIN_RUN and len(set(run)) > len(run) // 2:
            tables.append((BASE + i, run))
            i = j
        else:
            i += 4
    with open(sys.argv[2], "w") as out:
        out.write("table\tclass\tindex\tname\tcode_ref\n")
        for addr, names in tables:
            kind = classify(names)
            for n, s in enumerate(names):
                ref = int(addr + 4 * n in code)
                out.write(f"{addr:08x}\t{kind}\t{n}\t{s}\t{ref}\n")
    print(f"{len(tables)} tables, {sum(len(t[1]) for t in tables)} names -> {sys.argv[2]}")


if __name__ == "__main__":
    main()
