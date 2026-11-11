#!/usr/bin/env python3
"""Decode the spr_ sprite files to PNG pages, plus their cell rectangles.

The spr_ files have no header; their layout is in the program. A registry
in main.bin's .data (81 entries at 0x0C322154) holds 12-byte entries
{ char *name, u32 base_id, desc * }; base_id steps by 0x78. Each desc is:
  page descriptors, 16 bytes { short w, h; u32 format; u32 offset; u32 0 },
    the same as the obj_ texture tables, ended by a record of -1
  cells, 20 bytes { u32 page_id; float u0, v0, u1, v1 }, after the -1. The
    page_id is global (a file owns base_id .. base_id + pages - 1), and the
    run after one file's pages can hold another file's cells (after
    spr_sel_stg_cav's pages come cells of spr_sel_stg_djo), so cells are
    assigned by id, not by position
Measured check: the pages tile all 57 spr_ files exactly (unpacked size for
the two .gz ones). Note 13 stage-select files share one size, so a single
match proves nothing; the earlier {desc, name, id} reading matched one of
them by chance and was wrong for 41 files.

usage: spr_tex.py <main.bin> <asset dir> <out dir>
"""
import json
import os
import struct
import sys

from PIL import Image

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import pvr_tex  # noqa: E402

BASE = 0x0C020000


def registry(prog):
    def u32(a):
        return struct.unpack_from("<I", prog, a - BASE)[0]

    def cstr(a):
        o = a - BASE
        if not 0 <= o < len(prog):
            return None
        e = prog.find(b"\0", o, o + 64)
        r = prog[o:e]
        return r.decode("ascii") if e > o and all(32 <= c < 127 for c in r) else None

    def is_spr(a):
        n = cstr(u32(a))
        return n is not None and n.startswith("spr_") and ".bin" in n

    # the registry: the longest run of { name -> "spr_*", id, desc } entries
    best = (0, 0)
    a = BASE
    while a < BASE + len(prog) - 12:
        n = 0
        while is_spr(a + 12 * n) and BASE <= u32(a + 12 * n + 8) < BASE + len(prog):
            n += 1
        if n > best[0]:
            best = (n, a)
        a += 12 * n + 4 if n else 4
    n, start = best
    out = {}
    for e in range(start, start + 12 * n, 12):
        out[cstr(u32(e))] = (u32(e + 8), u32(e + 4))
    return out, u32


def parse_pages(u32, desc):
    """Page descriptors at desc; returns (pages, address after the -1 record)."""
    pages, a = [], desc
    while u32(a) != 0xFFFFFFFF:
        wh, fmt, off = u32(a), u32(a + 4), u32(a + 8)
        pages.append((wh & 0xFFFF, wh >> 16, fmt, off))
        a += 16
    return pages, a + 16


def all_cells(u32, reg):
    """Cells from every run, assigned to the file that owns their page id."""
    owner = {}
    for name, (desc, base) in reg.items():
        for i in range(len(parse_pages(u32, desc)[0])):
            owner[base + i] = (name, i)
    cells = {name: [] for name in reg}
    for name, (desc, base) in reg.items():
        a = parse_pages(u32, desc)[1]
        while u32(a) in owner:
            n, page = owner[u32(a)]
            uv = struct.unpack("<4f", struct.pack("<4I", *(u32(a + 4 + 4 * k) for k in range(4))))
            cells[n].append({"page": page, "uv": [round(v, 6) for v in uv]})
            a += 20
    return cells


def main():
    prog = open(sys.argv[1], "rb").read()
    src, dest = sys.argv[2], sys.argv[3]
    reg, u32 = registry(prog)
    cells_of = all_cells(u32, reg)
    os.makedirs(dest, exist_ok=True)
    done = mismatch = 0
    for name in sorted(reg):
        path = os.path.join(src, name)
        if name.endswith(".gz"):
            path = os.path.join(src, "unpacked", name[:-3])
        if not os.path.exists(path):
            continue
        data = open(path, "rb").read()
        desc, base = reg[name]
        pages = parse_pages(u32, desc)[0]
        cells = cells_of[name]
        end = max((off + pvr_size(w, h, fmt) for w, h, fmt, off in pages), default=0)
        if end != len(data):
            mismatch += 1
            print(f"{name}: pages end at {end:#x}, file is {len(data):#x}", file=sys.stderr)
        stem = name.split(".bin")[0]
        for i, (w, h, fmt, off) in enumerate(pages):
            img = pvr_tex.decode(data, w, h, fmt, off)
            Image.fromarray(img, "RGBA").save(os.path.join(dest, f"{stem}_p{i}_{w}x{h}.png"))
        with open(os.path.join(dest, f"{stem}.json"), "w") as f:
            json.dump({"file": name, "base_id": base,
                       "pages": [{"w": w, "h": h, "format": f"{fmt:#06x}", "offset": off}
                                 for w, h, fmt, off in pages],
                       "cells": cells}, f, indent=1)
        done += 1
    print(f"{done} sprite files decoded to {dest}; {mismatch} size mismatches", file=sys.stderr)


def pvr_size(w, h, fmt):
    t = fmt & 0xF00
    if t in (0x100, 0x900, 0xD00):
        return w * h * 2
    if t == 0x200:
        return pvr_tex.MIP[w.bit_length() - 1] * 2 + w * h * 2
    if t in (0x300, 0x400):
        return 0x800 + (pvr_tex.VQ_MIP[w.bit_length() - 1] if t == 0x400 else 0) + w * h // 4
    raise ValueError(f"type {t:#x}")


if __name__ == "__main__":
    main()
