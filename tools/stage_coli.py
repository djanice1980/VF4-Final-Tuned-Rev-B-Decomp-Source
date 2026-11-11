#!/usr/bin/env python3
"""Decode the stage collision files (st_*_col.bin).

stage_coli_load (source/vf4lib/librobdisp/src/stage.c) reads the file named
by the stage record (+0x1C) whole into the buffer of the static heap record
"field_load" (the record { addr, size 0x8000, name } is at 0x0C360000;
static_heaps_alloc_all allocates the buffer, 0x0CDA01A0 in the saved
states), makes it the current collision data (field_coli, 0x0C3742F8) and
calls field_coli_set(0, buffer). field_coli_relocate then
adds (buffer - 0) to every pointer: the files were built for address 0, so
every pointer in them is an offset from the start of the file. The layout
below is the one the matched C (source/vf4lib/libbase/src/field.c) and the
hand-written assembly (source/asm/0c04f520.s .. 0c04f900.s, coli_segment_hit,
coli_sweep_hit, coli_grid_cell, coli_grid_cells_on_segment,
coli_poly_contains_hit) read; every named field cites a reader. Words no
code reads are field_0xNN.

  header, 0x4C bytes (19 words; mode.c copies exactly 19 words of it into
  DAT_0c370bec before and after rep_load_end)
    +0x00 field_0x00: not read by the field code; equals the offset of the
          block-pointer array (+0x24) in 13 files, 0x78 / 0xA8 short of it
          in the four with breakable walls, 0x24C8 (past the end) in
          st_debug0 and st_test0
    +0x04 version, 0x3F2 (coli_segment_hit, coli_sweep_hit,
          coli_segment_hit_any: "Collision data version error!!")
    +0x08 -> vertices (field_coli_relocate)
    +0x0C -> polygons (field_coli_relocate, coli_polys_relocate)
    +0x10 -> grid (field_coli_relocate; coli_grid_cell_at,
          coli_grid_cells_along, coli_grid_cell, coli_grid_cells_on_segment)
    +0x14 -> cell table (coli_cell_table_relocate; coli_segment_hit,
          coli_sweep_hit, coli_segment_hit_nearest, coli_sphere_hit_nearest;
          coli_segment_hit_any tests only the list of cell 0)
    +0x18 -> breakable-wall root word, or 0 (coli_list_relocate)
    +0x1C -> group table, or 0 (coli_groups_relocate; 0 in every file)
    +0x20 field_0x20: not read (0)
    +0x24 -> block-pointer array, +0x40 entries (coli_ptrs_relocate)
    +0x28 run time: the first entry of that array, the address of block 0
          (field_coli_set; 0 in the files)
    +0x2C vertex count: not read (the code reaches vertices only through
          polygon pointers); equals (polygons - vertices) / 12
    +0x30 polygon count (field_coli_relocate)
    +0x34 cell count (coli_cell_table_relocate; the bound in
          coli_segment_hit_nearest / coli_sphere_hit_nearest)
    +0x38 field_0x38: not read; the breakable record count (0 without)
    +0x3C field_0x3c: not read (1 in every file)
    +0x40 entry count of the group table and of the block-pointer array
          (field_coli_relocate; 1 in every file)
    +0x44, +0x48 run time: cleared by field_coli_set, read by nothing
  grid, 0x14 bytes
    +0x00 x0, +0x04 z0 (world origin of cell 0), +0x08 1 / cell size,
    +0x0C u16 width (cells per row), +0x0E u16 field_0x0e (not read; the row
    count), +0x10 u16 cell count (bound), +0x12 u16 pad. cell =
    trunc((x - x0) * inv) + trunc((z - z0) * inv) * width, -1 unless
    0 <= cell < count (coli_grid_cell_at, coli_grid_cell)
  vertex, 12 bytes: x, y, z (floats)
  polygon, 0x3C bytes (coli_polys_relocate)
    +0x00 off: non-zero switches the polygon off; every test skips it
          unless the caller passes flag 2. coli_flag_list_set writes 1 when
          a breakable wall breaks; the files already hold 1 in the walls of
          the open rings (rob_floor_check's wall probes, flag 2, read an off
          wall as an open edge)
    +0x04 kind bits (the flag filter of coli_segment_hit / coli_sweep_hit /
          *_nearest / _any): 0x100 floor, 0x200 wall, 0x400 ring-out
          ground, 0x80000000 also out (rob_floor_check counts a sphere over
          0x400 / 0x80000000 as over a drop); bit 0 a breakable wall
          (rob_wall_coli_side); 0x1000 / 0x8000 / 0x10000 not read
    +0x08 field_0x08, +0x09 field_0x09 (bytes, not read; 0)
    +0x0A edge count (byte; coli_poly_point_inside, coli_segment_hit ->
          coli_poly_contains_hit; 4 in every polygon)
    +0x0B field_0x0b (byte, not read; 0)
    +0x0C s16 slope: acos(n.y) as an angle (0 flat, 0x4000 vertical,
          -0x8000 facing down). rob_wall_coli_side: > 0x1000 pushes; slope
          0 on a wall (its top face) takes the centroid of v[0..3]; the flag
          filter: kinds 0x100 / 0x200 with slope 0 need flags 0x48 / 0x10
    +0x0E s16 heading of the normal, atan2_s16(n.x, -n.z)
          (rob_wall_coli_side: wall side 0-3 from the rob's angle minus it)
    +0x10 -> breakable record, or 0 (coli_polys_relocate when non-zero;
          rob_wall_coli_side passes it to coli_flag_list_set)
    +0x14 -> field_0x14, or 0 (relocated when non-zero; 0 in every file)
    +0x18 normal x, y, z; +0x24 plane distance d (n . p = d on the plane;
          front is n . p > d)
    +0x28 five vertex pointers: edge i runs from v[i] to v[i + 1], and the
          one after the last edge is v[0] again (always all five relocated);
          inside is (v[i+1] - v[i]) x (p - v[i]) . n >= 0 for every edge
  cell lists: null-ended lists of polygon pointers, stored before the cell
    table; the table holds one list pointer per cell (coli_cell_table_relocate
    relocates the table, then every non-zero word from the lowest list
    pointer up to the table); cells with the same polygons share a list
  breakable walls (+0x18): root word -> null-ended array of record pointers
    (coli_list_relocate); record, 0x14 bytes: +0x00 byte field_0x00, +0x01
    byte number (rob_wall_coli_side stores it to DAT_0c292808), +0x02 u16
    field_0x02, +0x04 -> null-ended list of the polygons the wall is made of
    (coli_flag_list_set sets their +0x00 to 1), +0x08 .. +0x10 three floats
    field_0x08 .. field_0x10 (not read; near the panel)
  group table (+0x1C): +0x40 pointers to null-ended lists of group records
    (+0x10 .. +0x24 relocated, +0x18 / +0x1C null-ended polygon lists whose
    polygons are relocated one by one); no file has one
  block-pointer array (+0x24): +0x40 pointers; the block itself is not read
    (field_coli_set keeps its address in +0x28); it is the rest of the
    file, 0x100 zero bytes in every file

usage:
  python3 -I tools/stage_coli.py FILE [--json OUT] [--obj OUT.obj]
        readable dump (and JSON / an OBJ of the polygons, one group per kind)
  python3 -I tools/stage_coli.py all DIR OUT     every st_*_col.bin in DIR to
                                                 OUT/<name>.json + .obj, summary
  python3 -I tools/stage_coli.py check DIR [MAIN_BIN]
        offline proof over every file in DIR: (a) a port of
        field_coli_relocate changes exactly the parse's pointer words, each
        landing on a structure start of the right type; (b) every byte is in
        a parsed structure; (c) ports of field_floor_height /
        coli_segment_hit at the stage center and start positions (stage
        table in MAIN_BIN, default work/main.bin), the ring size and the wall
        probes of rob_floor_check; also the polygon geometry (planes,
        winding, slope / heading words), the cell lists against the
        polygons' x / z boxes, the stage model's vertex heights
        (work/models/stg_*.obj) and the values of the unread fields
  python3 -I tools/stage_coli.py floor FILE X Y Z [--ground]
        field_floor_height at one point (ray from Y + 0.5 down)
  python3 -I tools/stage_coli.py live RAM [ASSETDIR] [--ram-base HEX]
        compares the game's copy with the file, from a dump of main RAM
        (32 MB at 0x0C000000 unless --ram-base; tools/flycast_state.py
        --dump writes one from a save state): stage_cur names the file,
        field_coli gives the buffer; checks the relocated words, the
        run-time words +0x28 / +0x44 / +0x48, the polygons switched off,
        coli_floor_poly and the robs' wall polygon (coli record +0x98) on
        polygon starts, and each rob's floor height +0x7C against the port
        of field_floor_height and its four wall probes (+0x12C .. +0x15C)
        against the port of rob_floor_check's probes, at its position
        +0x90 less the frame's shift (coli record +0x370)
"""
import glob, json, math, os, struct, sys

HEADER, GRID, VERTEX, POLY, BRK = 0x4C, 0x14, 12, 0x3C, 0x14
VERSION = 0x3F2
FIELD_LOAD = 0x0C360000          # static_heap_rec field_load { addr, size, name }
BUFFER = 0x0CDA01A0              # field_load.addr in the saved states (any base works offline)
FIELD_COLI, COLI_FLOOR_POLY = 0x0C3742F8, 0x0C3743EC
STAGE_CUR = 0x0C306CE8
ROBS, ROB_SIZE = 0x0C37BA30, 0x86C   # rob_get(i); +0x3C -> coli record
RAM_BASE = 0x0C000000
PROGRAM_BASE = 0x0C020000
STAGE_TABLE, STAGE_COUNT = 0x0C306C88, 24
SEG_FLOOR = -1000.0              # coli_segment_hit / coli_sweep_hit: 0x0C04FB0C
SEG_STEP = 1.0                   # coli_grid_cells_on_segment: 0x0C04FB10
KIND_NAMES = [(0x80000000, "out2"), (0x400, "out"), (0x200, "wall"), (0x100, "floor")]


def u8(d, o):
    return d[o]


def u16(d, o):
    return struct.unpack_from("<H", d, o)[0]


def s16(d, o):
    return struct.unpack_from("<h", d, o)[0]


def u32(d, o):
    return struct.unpack_from("<I", d, o)[0]


def s32(d, o):
    return struct.unpack_from("<i", d, o)[0]


def f32(d, o):
    return struct.unpack_from("<f", d, o)[0]


def fl(x):
    """round to float32."""
    return struct.unpack("<f", struct.pack("<f", x))[0]


def kind_name(k):
    for bit, n in KIND_NAMES:
        if k & bit:
            return n + ("_breakable" if k & 1 else "")
    return "other"


class Coli:
    """One collision file parsed the way field_coli_relocate walks it."""

    def __init__(self, d, name):
        self.d, self.name = d, name
        self.notes = []
        self.structs = []        # (offset, size, type, label)
        self.ptrs = {}           # word offset -> (target, expected type)
        h = self.h = [u32(d, 4 * i) for i in range(19)]
        self.add(0, HEADER, "header", "header")
        if h[1] != VERSION:
            self.notes.append("version 0x%x is not 0x3f2: the hit tests would stop with "
                              "'Collision data version error!!'" % h[1])
        self.verts_off, self.polys_off, self.grid_off, self.cells_off = h[2], h[3], h[4], h[5]
        self.brk_root, self.groups_off, self.blocks_off = h[6], h[7], h[9]
        self.nverts, self.npolys, self.ncells = h[11], h[12], h[13]
        self.nbrk_field, self.f3c, self.nblocks = h[14], h[15], h[16]
        for o, t in ((8, "vertex"), (0xC, "poly"), (0x10, "grid"), (0x14, "cell_table")):
            self.ptr(o, t)
        # grid
        g = self.grid_off
        self.grid = {"offset": g, "x0": f32(d, g), "z0": f32(d, g + 4), "inv_cell": f32(d, g + 8),
                     "width": u16(d, g + 12), "field_0x0e": u16(d, g + 14), "count": u16(d, g + 16),
                     "pad": u16(d, g + 18)}
        self.add(g, GRID, "grid", "grid")
        # vertices: the run up to the polygons (+0x2C gives the same count)
        self.verts = []
        for i in range(self.nverts):
            o = self.verts_off + VERTEX * i
            self.verts.append((f32(d, o), f32(d, o + 4), f32(d, o + 8)))
            self.add(o, VERTEX, "vertex", "vertex %d" % i)
        if self.verts_off + VERTEX * self.nverts != self.polys_off:
            self.notes.append("vertex count +0x2c (%d) does not end at the polygons" % self.nverts)
        self.vindex = {self.verts_off + VERTEX * i: i for i in range(self.nverts)}
        # polygons
        self.polys = [self.parse_poly(self.polys_off + POLY * i, "polygon %d" % i) for i in range(self.npolys)]
        self.pindex = {p["offset"]: i for i, p in enumerate(self.polys)}
        # cell table and lists
        self.add(self.cells_off, 4 * self.ncells, "cell_table", "cell table")
        self.cells = []
        for i in range(self.ncells):
            self.ptr(self.cells_off + 4 * i, "cell_list")
            self.cells.append(u32(d, self.cells_off + 4 * i))
        lo = min(self.cells + [self.cells_off])
        self.lists_lo = lo
        self.lists = {}
        o = lo
        while o < self.cells_off:            # the lists tile [lo, table)
            start, items = o, []
            while u32(d, o):
                self.ptr(o, "poly")
                items.append(u32(d, o))
                o += 4
            o += 4
            self.lists[start] = items
            self.add(start, o - start, "cell_list", "cell list (%d)" % len(items))
        for c in set(self.cells) - set(self.lists):
            self.notes.append("cell list +0x%x is not on a list start" % c)
        self.unused_lists = sorted(set(self.lists) - set(self.cells))
        # breakable walls
        self.brk = []
        self.brk_array = None
        if self.brk_root:
            self.ptr(0x18, "brk_root")
            self.add(self.brk_root, 4, "brk_root", "breakable root word")
            a = self.brk_array = u32(d, self.brk_root)
            self.ptr(self.brk_root, "brk_array")
            k = a
            while u32(d, k):
                self.ptr(k, "brk")
                e = u32(d, k)
                lst = u32(d, e + 4)
                self.ptr(e + 4, "brk_list")
                items, j = [], lst
                while u32(d, j):
                    self.ptr(j, "poly")
                    items.append(u32(d, j))
                    j += 4
                self.add(lst, j + 4 - lst, "brk_list", "breakable list %d" % len(self.brk))
                self.add(e, BRK, "brk", "breakable record %d" % len(self.brk))
                self.brk.append({"offset": e, "field_0x00": u8(d, e), "number": u8(d, e + 1),
                                 "field_0x02": u16(d, e + 2), "list_offset": lst, "polys": items,
                                 "field_0x08": [f32(d, e + 8 + 4 * q) for q in range(3)]})
                k += 4
            self.add(a, k + 4 - a, "brk_array", "breakable array")
        # groups (none in the 18 files; parsed as coli_groups_relocate walks them)
        self.groups = []
        if self.groups_off:
            self.ptr(0x1C, "group_table")
            self.add(self.groups_off, 4 * self.nblocks, "group_table", "group table")
            for gi in range(self.nblocks):
                self.ptr(self.groups_off + 4 * gi, "group_list")
                gl = u32(d, self.groups_off + 4 * gi)
                k = gl
                while u32(d, k):
                    self.ptr(k, "group")
                    r = u32(d, k)
                    for q in range(6):
                        self.ptr(r + 0x10 + 4 * q, "any")
                    a, b = u32(d, r + 0x18), u32(d, r + 0x1C)
                    while u32(d, a):
                        self.ptr(a, "poly")
                        self.ptr(b, "poly")
                        self.parse_poly(u32(d, a), "group polygon")
                        self.parse_poly(u32(d, b), "group polygon")
                        a += 4
                        b += 4
                    self.groups.append(r)
                    k += 4
                self.add(gl, k + 4 - gl, "group_list", "group list")
        # block-pointer array
        self.blocks = []
        if self.blocks_off:
            self.ptr(0x24, "block_array")
            self.add(self.blocks_off, 4 * self.nblocks, "block_array", "block-pointer array")
            for i in range(self.nblocks):
                self.ptr(self.blocks_off + 4 * i, "block")
                self.blocks.append(u32(d, self.blocks_off + 4 * i))
            for i, b in enumerate(sorted(set(self.blocks))):
                nxt = min([s for s, _, _, _ in self.structs if s > b] + [len(d)])
                self.add(b, nxt - b, "block", "block %d" % i)
        self.block_zero = all(x == 0 for b in self.blocks for x in d[b:])

    # -- helpers ------------------------------------------------------------
    def add(self, o, size, t, label):
        self.structs.append((o, size, t, label))

    def ptr(self, o, t):
        self.ptrs[o] = (u32(self.d, o), t)

    def parse_poly(self, o, label):
        d = self.d
        p = {"offset": o, "off": s32(d, o), "kind": u32(d, o + 4), "field_0x08": u8(d, o + 8),
             "field_0x09": u8(d, o + 9), "edges": u8(d, o + 10), "field_0x0b": u8(d, o + 11),
             "slope": s16(d, o + 12), "heading": s16(d, o + 14), "brk": u32(d, o + 0x10),
             "field_0x14": u32(d, o + 0x14), "normal": [f32(d, o + 0x18 + 4 * i) for i in range(3)],
             "d": f32(d, o + 0x24), "v": [u32(d, o + 0x28 + 4 * i) for i in range(5)]}
        if p["brk"]:
            self.ptr(o + 0x10, "brk")
        if p["field_0x14"]:
            self.ptr(o + 0x14, "any")
        for i in range(5):
            self.ptr(o + 0x28 + 4 * i, "vertex")
        self.add(o, POLY, "poly", label)
        return p

    def vert(self, off):
        return self.verts[self.vindex[off]]

    def counts(self):
        return {"vertices": self.nverts, "polygons": self.npolys,
                "floor": sum(1 for p in self.polys if p["kind"] & 0x100),
                "wall": sum(1 for p in self.polys if p["kind"] & 0x200 and not p["kind"] & 1),
                "breakable": sum(1 for p in self.polys if p["kind"] & 0x200 and p["kind"] & 1),
                "out": sum(1 for p in self.polys if p["kind"] & 0x80000400),
                "cells": self.ncells, "lists": len(self.lists),
                "list_entries": sum(len(v) for v in self.lists.values()),
                "breakable_walls": len(self.brk), "pointers": len(self.ptrs)}

    def gaps(self):
        """(offset, size) of every byte range in no structure, and overlaps."""
        cover = bytearray(len(self.d))
        overl = []
        for o, s, t, lab in self.structs:
            for i in range(o, min(o + s, len(self.d))):
                if cover[i]:
                    overl.append((i, lab))
                cover[i] = 1
        gaps, i = [], 0
        while i < len(cover):
            if not cover[i]:
                j = i
                while j < len(cover) and not cover[j]:
                    j += 1
                gaps.append((i, j - i))
                i = j
            else:
                i += 1
        return gaps, overl

    def to_json(self):
        hj = {"field_0x00": self.h[0], "version": self.h[1], "vertices": self.h[2], "polygons": self.h[3],
              "grid": self.h[4], "cell_table": self.h[5], "breakable_root": self.h[6], "group_table": self.h[7],
              "field_0x20": self.h[8], "block_array": self.h[9], "first_block_word": self.h[10],
              "vertex_count": self.h[11], "polygon_count": self.h[12], "cell_count": self.h[13],
              "field_0x38": self.h[14], "field_0x3c": self.h[15], "entry_count": self.h[16],
              "run_0x44": self.h[17], "run_0x48": self.h[18]}
        polys = []
        for i, p in enumerate(self.polys):
            q = dict(p)
            q["index"] = i
            q["kind_name"] = kind_name(p["kind"])
            q["vertices"] = [self.vindex.get(v) for v in p["v"]]
            q["breakable"] = next((k for k, b in enumerate(self.brk) if b["offset"] == p["brk"]), None)
            polys.append(q)
        return {"name": self.name, "size": len(self.d), "header": hj, "grid": self.grid,
                "vertices": [list(v) for v in self.verts], "polygons": polys,
                "cells": [{"cell": i, "list": c, "polygons": [self.pindex.get(x) for x in self.lists.get(c, [])]}
                          for i, c in enumerate(self.cells)],
                "breakable_walls": [dict(b, polys=[self.pindex.get(x) for x in b["polys"]]) for b in self.brk],
                "blocks": self.blocks, "counts": self.counts(), "notes": self.notes}

    def obj(self, out):
        w = lambda s: out.write(s + "\n")
        w("# %s: collision polygons, exported by tools/stage_coli.py" % self.name)
        for x, y, z in self.verts:
            w("v %.6f %.6f %.6f" % (x, y, z))
        groups = {}
        for i, p in enumerate(self.polys):
            groups.setdefault(kind_name(p["kind"]) + ("_off" if p["off"] else ""), []).append((i, p))
        for g in sorted(groups):
            w("g %s" % g)
            for i, p in groups[g]:
                idx = [self.vindex[v] + 1 for v in p["v"][:p["edges"]]]
                w("f " + " ".join("%d" % k for k in idx))


# ---------------------------------------------------------------------------
# relocation (field_coli_set / field_coli_relocate and its helpers, ported
# word for word onto a copy of the file placed at `new`; the file's own
# pointers are relative to `old`)

class Mem:
    def __init__(self, d, base):
        self.b, self.base = bytearray(d), base

    def rd(self, a):
        return struct.unpack_from("<i", self.b, a - self.base)[0]

    def wr(self, a, v):
        struct.pack_into("<i", self.b, a - self.base, ((v + 0x80000000) & 0xFFFFFFFF) - 0x80000000)


def coli_polys_relocate(m, p, n, delta):
    for _ in range(n):
        if m.rd(p + 0x10):
            m.wr(p + 0x10, m.rd(p + 0x10) + delta)
        if m.rd(p + 0x14):
            m.wr(p + 0x14, m.rd(p + 0x14) + delta)
        for i in range(5):
            m.wr(p + 0x28 + 4 * i, m.rd(p + 0x28 + 4 * i) + delta)
        p += POLY


def coli_cell_table_relocate(m, t, n, delta):
    base = lo = t
    for i in range(n):
        v = m.rd(t + 4 * i) + delta
        m.wr(t + 4 * i, v)
        if v < lo:
            lo = v
    p = lo
    while p < base:
        if m.rd(p):
            m.wr(p, m.rd(p) + delta)
        p += 4


def coli_list_relocate(m, r, delta):
    p = m.rd(r) + delta
    m.wr(r, p)
    while m.rd(p):
        e = m.rd(p) + delta
        q = delta + m.rd(e + 4)
        m.wr(e + 4, q)
        m.wr(p, e)
        while m.rd(q):
            m.wr(q, m.rd(q) + delta)
            q += 4
        p += 4


def coli_groups_relocate(m, t, n, delta):
    for i in range(n):
        pp = m.rd(t + 4 * i) + delta
        m.wr(t + 4 * i, pp)
        o = m.rd(pp)
        while o:
            o += delta
            m.wr(pp, o)
            for k in range(0x10, 0x28, 4):
                m.wr(o + k, m.rd(o + k) + delta)
            a, b = m.rd(o + 0x18), m.rd(o + 0x1C)
            while m.rd(a):
                m.wr(a, delta + m.rd(a))
                coli_polys_relocate(m, m.rd(a), 1, delta)
                a += 4
                m.wr(b, delta + m.rd(b))
                coli_polys_relocate(m, m.rd(b), 1, delta)
                b += 4
            pp += 4
            o = m.rd(pp)


def field_coli_set(d, old, new):
    """returns the relocated copy (field_coli_set(old, new) with the file at
    new): +0x44 / +0x48 cleared, pointers moved, +0x28 set."""
    m = Mem(d, new)
    m.wr(new + 0x44, 0)
    m.wr(new + 0x48, 0)
    if old != new:
        delta = new - old
        for k in (8, 0xC, 0x10, 0x14):
            m.wr(new + k, m.rd(new + k) + delta)
        coli_polys_relocate(m, m.rd(new + 0xC), m.rd(new + 0x30), delta)
        coli_cell_table_relocate(m, m.rd(new + 0x14), m.rd(new + 0x34), delta)
        if m.rd(new + 0x18):
            m.wr(new + 0x18, delta + m.rd(new + 0x18))
            coli_list_relocate(m, m.rd(new + 0x18), delta)
        if m.rd(new + 0x1C):
            m.wr(new + 0x1C, delta + m.rd(new + 0x1C))
            coli_groups_relocate(m, m.rd(new + 0x1C), m.rd(new + 0x40), delta)
        if m.rd(new + 0x24):
            m.wr(new + 0x24, delta + m.rd(new + 0x24))
            for i in range(m.rd(new + 0x40)):
                a = m.rd(new + 0x24) + 4 * i
                m.wr(a, m.rd(a) + delta)
    m.wr(new + 0x28, m.rd(m.rd(new + 0x24)))
    return bytes(m.b)


# ---------------------------------------------------------------------------
# hit tests over the parse (file offsets; Python doubles on float32 inputs)

def grid_cell(g, x, z):
    """coli_grid_cell / coli_grid_cell_at: trunc toward zero, 16-bit width."""
    cx = int(fl((x - g["x0"]) * g["inv_cell"]))
    cz = int(fl((z - g["z0"]) * g["inv_cell"]))
    r = cx + cz * g["width"]
    return r if 0 <= r < g["count"] else -1


def grid_cells_on_segment(g, x0, z0, x1, z1):
    """coli_grid_cells_on_segment: walks from (x1, z1) toward (x0, z0) in
    steps of at most SEG_STEP; stops at the first cell outside the grid."""
    dx, dz = fl(x0 - x1), fl(z0 - z1)
    n = 1
    if not (SEG_STEP * SEG_STEP > dx * dx + dz * dz):
        m = max(abs(dx), abs(dz))
        n = int(fl(m / SEG_STEP + 1.0))
        inv = fl(1.0 / n)
        dx, dz = fl(dx * inv), fl(dz * inv)
    out, prev, x, z = [], -1, x1, z1
    for _ in range(n):
        c = grid_cell(g, x, z)
        if c < 0:
            break
        if c != prev:
            out.append(c)
            prev = c
        x, z = fl(x + dx), fl(z + dz)
    return out


def flag_pass(p, flags):
    """the polygon filter shared by coli_segment_hit and coli_sweep_hit."""
    if p["off"] != 0 and not flags & 2:
        return False
    k = p["kind"]
    if k & 0x80000000 or k & 0x400:
        return bool(flags & 0x40)
    if k & 0x200:
        if flags & 0x20 or not flags & 4:
            return False
        return bool(flags & 0x10) or p["slope"] != 0
    if k & 0x100:
        if flags & 8:
            return True
        return bool(flags & 0x40) and p["slope"] != 0
    return True


def dot(a, b):
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]


def poly_contains(c, p, pt):
    """coli_poly_contains_hit: (v[i+1] - v[i]) x (pt - v[i]) . n >= 0 for
    every edge."""
    n = p["normal"]
    for i in range(p["edges"]):
        a, b = c.vert(p["v"][i]), c.vert(p["v"][i + 1])
        e = (b[0] - a[0], b[1] - a[1], b[2] - a[2])
        q = (pt[0] - a[0], pt[1] - a[1], pt[2] - a[2])
        x = (e[1] * q[2] - e[2] * q[1], e[2] * q[0] - e[0] * q[2], e[0] * q[1] - e[1] * q[0])
        if 0.0 > dot(x, n):
            return False
    return True


def segment_hit(c, frm, to, flags):
    """coli_segment_hit(out, from, to, flags): the polygon crossed from its
    back (from) to its front (to) nearest `to`, over the cells walked from
    `to` toward `from`, stopping after the first cell with a hit. Returns
    (polygon index, point) or None."""
    if c.h[1] != VERSION:
        return None
    if SEG_FLOOR > frm[1] and SEG_FLOOR > to[1]:
        return None
    cells = grid_cells_on_segment(c.grid, frm[0], frm[2], to[0], to[2])
    dirv = (frm[0] - to[0], frm[1] - to[1], frm[2] - to[2])
    best, bestd = None, SEG_FLOOR * SEG_FLOOR
    for cell in cells:
        if best is not None:
            break
        for po in c.lists.get(c.cells[cell], []):
            p = c.polys[c.pindex[po]]
            if not flag_pass(p, flags):
                continue
            n, dd = p["normal"], p["d"]
            dt, df = dot(n, to) - dd, dot(n, frm) - dd
            if 0.0 > dt or not (0.0 > df):
                continue
            t = -dt / dot(n, dirv)
            pt = (to[0] + t * dirv[0], to[1] + t * dirv[1], to[2] + t * dirv[2])
            if not poly_contains(c, p, pt):
                continue
            dist = (to[0] - pt[0]) ** 2 + (to[1] - pt[1]) ** 2 + (to[2] - pt[2]) ** 2
            if not (dist > bestd):
                best, bestd, bpt = c.pindex[po], dist, pt
    return (best, bpt) if best is not None else None


def floor_height(c, pos, ground=False, above=0.5):
    """field_floor_height: a ray from y + above down to y = -1000.01 with
    flags 9 (0x49 with the ground flag). Returns (y, polygon index or None);
    y = -1000 and kind 0x400 when nothing is hit."""
    frm = (pos[0], fl(-1000.01), pos[2])
    to = (pos[0], fl(above + pos[1]), pos[2])
    r = segment_hit(c, frm, to, 0x49 if ground else 9)
    if r is None:
        return -1000.0, None
    return r[1][1], r[0]


def heading_move(ang, pos, dist):
    """vec3_move_on_heading as rob_floor_check uses it: angle 0 along -z,
    0x4000 along -x (the rob's facing convention, atan2_s16(x, -z))."""
    a = ang / 32768.0 * math.pi
    return (pos[0] + dist * math.sin(a), pos[1], pos[2] - dist * math.cos(a))


# ---------------------------------------------------------------------------
# stage table (main.bin)

def stage_table(main_path):
    d = open(main_path, "rb").read()
    rd = lambda a: u32(d, a - PROGRAM_BASE)
    rf = lambda a: f32(d, a - PROGRAM_BASE)
    rs = lambda a: s16(d, a - PROGRAM_BASE)

    def cs(a):
        o = a - PROGRAM_BASE
        return d[o:d.index(b"\0", o)].decode("latin1")
    out = []
    for i in range(STAGE_COUNT):
        r = rd(STAGE_TABLE + 4 * i)
        sp = rd(r + 0x34)
        starts = [{"angle": rs(sp + 16 * k + 2), "pos": [rf(sp + 16 * k + 4 + 4 * j) for j in range(3)]}
                  for k in range(2)]
        out.append({"index": i, "record": r, "name": cs(rd(r)), "coli_file": cs(rd(r + 0x1C)),
                    "center": [rf(r + 0x50 + 4 * j) for j in range(3)], "starts": starts})
    return out


# ---------------------------------------------------------------------------

def dump(c, out=sys.stdout):
    w = lambda s="": out.write(s + "\n")
    k = c.counts()
    h = c.h
    w("%s: %d bytes; %d vertices, %d polygons (%d floor, %d out, %d wall, %d breakable), %d cells, "
      "%d lists, %d breakable walls" % (c.name, len(c.d), k["vertices"], k["polygons"], k["floor"], k["out"],
                                        k["wall"], k["breakable"], k["cells"], k["lists"], k["breakable_walls"]))
    w("header: +00 %08x  version %x  vertices +%x  polygons +%x  grid +%x  cells +%x  breakable +%x  groups +%x  "
      "+20 %x  blocks +%x  +28 %x" % tuple(h[:11]))
    w("        counts: vertices %d  polygons %d  cells %d  +38 %d  +3c %d  entries +40 %d  run time +44 %x +48 %x"
      % tuple(h[11:19]))
    g = c.grid
    w("grid +%x: origin x %g z %g, cell %g (inv %g), %d x %d (+0e), %d cells, pad %d" % (
        g["offset"], g["x0"], g["z0"], 1.0 / g["inv_cell"] if g["inv_cell"] else 0, g["inv_cell"], g["width"],
        g["field_0x0e"], g["count"], g["pad"]))
    w()
    w("vertices +%x:" % c.verts_off)
    for i, v in enumerate(c.verts):
        w("  %3d  %9.4f %9.4f %9.4f" % ((i,) + v))
    w()
    w("polygons +%x:" % c.polys_off)
    for i, p in enumerate(c.polys):
        vi = [c.vindex.get(v, "+%x" % v) for v in p["v"]]
        extra = ""
        if p["brk"]:
            extra += " breakable %d" % next(k for k, b in enumerate(c.brk) if b["offset"] == p["brk"])
        if p["field_0x14"]:
            extra += " +14 %x" % p["field_0x14"]
        if p["off"]:
            extra += " OFF %d" % p["off"]
        w("  %3d +%04x %-8s kind %08x edges %d slope %5d heading %6d n (%7.4f %7.4f %7.4f) d %8.4f v %s%s"
          "  [+08 %d +09 %d +0b %d]" % (
              i, p["offset"], kind_name(p["kind"]), p["kind"], p["edges"], p["slope"], p["heading"],
              p["normal"][0], p["normal"][1], p["normal"][2], p["d"], vi, extra, p["field_0x08"], p["field_0x09"],
              p["field_0x0b"]))
    w()
    w("cell lists +%x .. +%x, cell table +%x:" % (c.lists_lo, c.cells_off, c.cells_off))
    share = {}
    for i, l in enumerate(c.cells):
        share.setdefault(l, []).append(i)
    for l in sorted(c.lists):
        w("  list +%04x cells %-24s polygons %s" % (l, ",".join("%d" % x for x in share.get(l, [])) or "(none)",
                                                    " ".join("%d" % c.pindex[x] for x in c.lists[l])))
    if c.brk:
        w()
        w("breakable walls: root +%x -> array +%x" % (c.brk_root, c.brk_array))
        for i, b in enumerate(c.brk):
            w("  %2d +%04x number %2d [+00 %d +02 %d] polygons %s  (+08 %.3f %.3f %.3f)" % (
                i, b["offset"], b["number"], b["field_0x00"], b["field_0x02"],
                " ".join("%d" % c.pindex[x] for x in b["polys"]), *b["field_0x08"]))
    w()
    w("blocks: array +%x -> %s (%s)" % (c.blocks_off, " ".join("+%x" % b for b in c.blocks),
                                         "all zero" if c.block_zero else "not zero"))
    for n in c.notes:
        w("note: " + n)


# ---------------------------------------------------------------------------
# the offline proof

TARGET_TYPES = {"vertex": {"vertex"}, "poly": {"poly"}, "grid": {"grid"}, "cell_table": {"cell_table"},
                "cell_list": {"cell_list"}, "brk_root": {"brk_root"}, "brk_array": {"brk_array"}, "brk": {"brk"},
                "brk_list": {"brk_list"}, "group_table": {"group_table"}, "group_list": {"group_list"},
                "block_array": {"block_array"}, "block": {"block"}}


def check_relocation(c, out):
    """(a): relocate a copy to FIELD_LOAD and compare."""
    w = lambda s: out.write(s + "\n")
    d = c.d
    r = field_coli_set(d, 0, BUFFER)
    changed = {}
    for o in range(0, len(d) & ~3, 4):
        a, b = u32(d, o), u32(r, o)
        if a != b:
            changed[o] = (a, b)
    runtime = {0x28, 0x44, 0x48}
    moved = {o for o, (a, b) in changed.items() if o not in runtime}
    bad_delta = [o for o in moved if (changed[o][1] - changed[o][0]) & 0xFFFFFFFF != BUFFER]
    parse = set(c.ptrs)
    missing, extra = sorted(parse - moved), sorted(moved - parse)
    starts = {}
    for o, s, t, lab in c.structs:
        starts.setdefault(o, set()).add(t)
    wrong = []
    for o, (tgt, t) in sorted(c.ptrs.items()):
        if t == "any":
            ok = tgt in starts
        else:
            ok = bool(starts.get(tgt, set()) & TARGET_TYPES[t])
        if not ok:
            wrong.append((o, tgt, t))
    # words that are not pointers but hold a value equal to a structure start
    look = []
    for o in range(0, len(d) & ~3, 4):
        if o in parse or o in runtime:
            continue
        v = u32(d, o)
        if v and v in starts:
            look.append((o, v))
    rt = "run-time words +28 %x +44 %x +48 %x" % (u32(r, 0x28), u32(r, 0x44), u32(r, 0x48))
    ok = not missing and not extra and not wrong and not bad_delta
    w("  (a) relocation: %d words moved by 0x%08x, parse lists %d pointers: %d missing, %d extra, "
      "%d off a structure start; %s; %s" % (len(moved), BUFFER, len(parse), len(missing), len(extra),
                                             len(wrong), rt, "OK" if ok else "FAIL"))
    for o in missing[:8]:
        w("      missing +%x" % o)
    for o in extra[:8]:
        w("      extra +%x (%x -> %x)" % ((o,) + changed[o]))
    for o, tgt, t in wrong[:8]:
        w("      +%x -> +%x not a %s start" % (o, tgt, t))
    if look:
        w("      %d other words equal a structure start: %s" % (len(look), " ".join(
            "+%x=%x(%s)" % (o, v, describe_word(c, o)) for o, v in look[:12])))
    return ok, {"moved": len(moved), "pointers": len(parse), "lookalikes": look}


def describe_word(c, o):
    for s, size, t, lab in c.structs:
        if s <= o < s + size:
            return "%s +%x" % (lab, o - s)
    return "?"


def check_coverage(c, out):
    w = lambda s: out.write(s + "\n")
    gaps, overl = c.gaps()
    tot = sum(s for _, s in gaps)
    w("  (b) coverage: %d structures, %d gap bytes, %d overlapping bytes; %s" % (
        len(c.structs), tot, len(overl), "OK" if not gaps and not overl else "see below"))
    for o, s in gaps:
        w("      gap +%x, %d bytes: %s" % (o, s, c.d[o:o + min(s, 32)].hex(" ")))
    for i, lab in overl[:4]:
        w("      overlap +%x (%s)" % (i, lab))
    return not overl, {"gap_bytes": tot, "gaps": gaps}


def check_geometry(c, out):
    """properties every polygon has: closing pointer, plane, winding, angles."""
    w = lambda s: out.write(s + "\n")
    worst_plane, worst_unit, slope_err, head_err, bad_wind, bad_close = 0.0, 0.0, 0, 0, 0, 0
    for p in c.polys:
        n = p["normal"]
        worst_unit = max(worst_unit, abs(math.sqrt(dot(n, n)) - 1.0))
        vs = [c.vert(v) for v in p["v"][:p["edges"]]]
        for v in vs:
            worst_plane = max(worst_plane, abs(dot(n, v) - p["d"]))
        if p["v"][p["edges"]] != p["v"][0]:
            bad_close += 1
        # winding: the polygon's own centroid must be inside by the hit test
        cen = tuple(sum(v[i] for v in vs) / len(vs) for i in range(3))
        if not poly_contains(c, p, cen):
            bad_wind += 1
        if dot(n, n) < 0.5:
            continue                      # st_test0: two walls with a zero normal
        # slope: acos(n.y) as an s16 angle (0x8000 wraps to -0x8000);
        # heading: atan2_s16(n.x, -n.z), the convention rob_wall_coli_side
        # and rob_ring_out_start use for angles (only defined off vertical)
        sl = round(math.acos(max(-1.0, min(1.0, n[1]))) * 32768 / math.pi)
        e = (sl - p["slope"] + 32768) % 65536 - 32768
        slope_err = max(slope_err, abs(e))
        if abs(n[1]) < 0.999:
            hd = round(math.atan2(n[0], -n[2]) * 32768 / math.pi)
            e = (hd - p["heading"] + 32768) % 65536 - 32768
            head_err = max(head_err, abs(e))
    ok = worst_plane < 1e-3 and bad_wind == 0 and bad_close == 0 and slope_err <= 2 and head_err <= 2
    w("  geometry: %d polygons; |n|-1 <= %.2g, vertex off its plane <= %.2g, closing pointer wrong %d, "
      "centroid outside %d; slope = acos(n.y) within %d, heading = atan2(n.x, -n.z) within %d (s16 units)" % (
          c.npolys, worst_unit, worst_plane, bad_close, bad_wind, slope_err, head_err))
    return ok, {"plane": worst_plane, "slope_err": slope_err, "head_err": head_err}


def check_floor(c, stages, out):
    """(c): field_floor_height at the stage center and start positions."""
    w = lambda s: out.write(s + "\n")
    users = [s for s in stages if s["coli_file"] == c.name]
    ok = True
    res = []
    if not users:
        w("  (c) no stage record names this file")
    seen = set()
    for s in users:
        pts = [("center", tuple(s["center"]))] + [("start %d" % k, tuple(st["pos"])) for k, st in enumerate(s["starts"])]
        key = tuple(pts)
        if key in seen:
            continue
        seen.add(key)
        line = []
        for lab, pos in pts:
            y, pi = floor_height(c, pos)
            yg, pg = floor_height(c, pos, ground=True)
            kn = kind_name(c.polys[pi]["kind"]) if pi is not None else "none"
            good = pi is not None and abs(y - pos[1]) < 0.02 and c.polys[pi]["kind"] & 0x100
            ok &= bool(good)
            line.append("%s (%g, %g, %g): floor %.4f poly %s %s%s" % (lab, pos[0], pos[1], pos[2], y, pi, kn,
                                                                      "" if good else " MISMATCH"))
            res.append((s["name"], lab, pos, y, pi))
        w("  (c) %s: %s" % ("/".join(x["name"] for x in users if tuple(
            [("center", tuple(x["center"]))] + [("start %d" % k, tuple(st["pos"])) for k, st in enumerate(x["starts"])])
            == key), "; ".join(line)))
    # ring size: walk out from the center along +x / -x / +z / -z until the
    # floor below stops being a 0x100 floor
    cen = tuple(users[0]["center"]) if users else (0.0, 4.0, 0.0)
    ring = []
    for ang, lab in ((0x4000, "-x"), (-0x4000, "+x"), (0, "-z"), (-0x8000, "+z")):
        r = 0.0
        while r < 40.0:
            p = heading_move(ang, cen, r + 0.05)
            y, pi = floor_height(c, p)
            if pi is None or not c.polys[pi]["kind"] & 0x100:
                break
            r += 0.05
        p = heading_move(ang, cen, r + 0.05)
        y, pi = floor_height(c, p, ground=True)
        beyond = "%s at %.2f" % (kind_name(c.polys[pi]["kind"]), y) if pi is not None else "nothing (-1000)"
        ring.append("%s %.2f (then %s)" % (lab, r, beyond))
    w("      ring floor from the center: " + ", ".join(ring))
    # rob_floor_check's four wall probes, 20 units out, flags 7, from the center
    probes = []
    for ang in (0, 0x4000, -0x8000, -0x4000):
        far = heading_move(ang, cen, 20.0)
        h = segment_hit(c, far, cen, 7)
        if h:
            p = c.polys[h[0]]
            # rob_floor_check keeps the distance as a wall (+0x12C) only
            # when the polygon is on; a switched-off one is an open edge
            probes.append("%s%s %.2f" % (kind_name(p["kind"]), " (off: open edge)" if p["off"] else "",
                                         math.dist(cen, h[1])))
        else:
            probes.append("none")
    w("      wall probes (coli_segment_hit flags 7, 20 units, angles 0 / 0x4000 / 0x8000 / 0xC000): "
      + ", ".join(probes))
    return ok, res


def check_all(dirp, main_path, out=sys.stdout):
    w = lambda s="": out.write(s + "\n")
    stages = stage_table(main_path)
    files = sorted(glob.glob(os.path.join(dirp, "st_*_col.bin")))
    allok = True
    cols = []
    tot = {"moved": 0, "pointers": 0, "gap": 0, "bytes": 0}
    for p in files:
        c = Coli(open(p, "rb").read(), os.path.basename(p))
        w("%s (%d bytes)" % (c.name, len(c.d)))
        a, ra = check_relocation(c, out)
        b, rb = check_coverage(c, out)
        g, rg = check_geometry(c, out)
        f, rf = check_floor(c, stages, out)
        if c.notes:
            for n in c.notes:
                w("  note: " + n)
        cl = check_cells(c, out)
        model_compare(c, dirp, out)
        tot["moved"] += ra["moved"]
        tot["pointers"] += ra["pointers"]
        tot["gap"] += rb["gap_bytes"]
        tot["bytes"] += len(c.d)
        used = any(s["coli_file"] == c.name for s in stages)
        allok &= a and b and (f or not used) and (g or not used) and (cl or not used)
        cols.append(c)
    w()
    w("total: %d files, %d bytes, %d pointer words (relocation moves %d), %d gap bytes" % (
        len(files), tot["bytes"], tot["pointers"], tot["moved"], tot["gap"]))
    field_stats(cols, stages, out)
    return allok


def cell_rect(c, i):
    g = c.grid
    size = 1.0 / g["inv_cell"]
    cx, cz = i % g["width"], i // g["width"]
    return g["x0"] + cx * size, g["z0"] + cz * size, size


def check_cells(c, out):
    """each cell's list against the polygons whose x / z extent meets the
    cell's square (borders included): the converter's rule, as far as the
    data shows it."""
    w = lambda s: out.write(s + "\n")
    if c.grid["inv_cell"] <= 0:
        w("  cells: grid scale %g, no cell can be found (cell lists not compared)" % c.grid["inv_cell"])
        return False
    eps = 1e-4
    missing = extra = 0
    listed = {c.pindex[q] for l in c.lists.values() for q in l}
    unlisted = [k for k in range(c.npolys) if k not in listed]
    for i, lst in enumerate(c.cells):
        x, z, s = cell_rect(c, i)
        want = set()
        for k, p in enumerate(c.polys):
            vs = [c.vert(v) for v in p["v"][:p["edges"]]]
            if (min(v[0] for v in vs) <= x + s + eps and max(v[0] for v in vs) >= x - eps and
                    min(v[2] for v in vs) <= z + s + eps and max(v[2] for v in vs) >= z - eps):
                want.add(k)
        want -= set(unlisted)
        have = {c.pindex[q] for q in c.lists[lst]}
        missing += len(want - have)
        extra += len(have - want)
    shared = len(c.cells) - len(set(c.cells))
    w("  cells: %d cells, %d distinct lists (%d cells share a list, %d lists no cell uses); against the polygons "
      "whose x / z box meets the cell square: %d listed polygons outside, %d meeting polygons not listed "
      "(polygons in no list: %s); cell 0 (the list coli_segment_hit_any tests) holds polygons %s" % (
          len(c.cells), len(set(c.cells)), shared, len(c.unused_lists), extra, missing,
          " ".join("%d(%s)" % (k, kind_name(c.polys[k]["kind"])) for k in unlisted) or "none",
          " ".join("%d(%s)" % (c.pindex[q], kind_name(c.polys[c.pindex[q]]["kind"])) for q in c.lists[c.cells[0]])))
    return missing == 0


def model_compare(c, dirp, out):
    """how many vertices of the stage model (work/models/stg_<name>.obj,
    tools/mdl.py) inside the ring floor lie at the collision floor height."""
    w = lambda s: out.write(s + "\n")
    stem = c.name[3:-8]
    path = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(dirp))), "work", "models",
                        "stg_%s.obj" % stem)
    if not os.path.exists(path):
        path = os.path.join(os.path.dirname(os.path.abspath(dirp)), "models", "stg_%s.obj" % stem)
    if not os.path.exists(path):
        w("  model: no stg_%s.obj" % stem)
        return
    floors = [p for p in c.polys if p["kind"] & 0x100 and p["slope"] == 0]
    if not floors:
        return
    main = max(floors, key=lambda p: area_xz(c, p))
    vs = [c.vert(v) for v in main["v"][:main["edges"]]]
    x0, x1 = min(v[0] for v in vs), max(v[0] for v in vs)
    z0, z1 = min(v[2] for v in vs), max(v[2] for v in vs)
    y = vs[0][1]
    n = at = 0
    hist = {}
    for line in open(path):
        if line.startswith("v "):
            _, a, b, cc = line.split()[:4]
            a, b, cc = float(a), float(b), float(cc)
            if x0 < a < x1 and z0 < cc < z1:
                n += 1
                at += abs(b - y) < 0.01
                hist[round(b, 2)] = hist.get(round(b, 2), 0) + 1
    top = sorted(hist.items(), key=lambda t: -t[1])[:3]
    w("  model stg_%s.obj: %d vertices over the largest flat floor (y %.2f, x %.1f..%.1f, z %.1f..%.1f), %d at "
      "that height; commonest heights %s" % (stem, n, y, x0, x1, z0, z1, at,
                                             ", ".join("%.2f x%d" % t for t in top)))


def area_xz(c, p):
    vs = [c.vert(v) for v in p["v"][:p["edges"]]]
    return abs(sum(vs[i][0] * vs[(i + 1) % len(vs)][2] - vs[(i + 1) % len(vs)][0] * vs[i][2]
                   for i in range(len(vs)))) / 2


def field_stats(cols, stages, out):
    """the values the unread fields take over all files."""
    w = lambda s: out.write(s + "\n")
    vals = {}

    def put(k, v):
        vals.setdefault(k, {}).setdefault(v, 0)
        vals[k][v] += 1
    for c in cols:
        h = c.h
        blk = c.blocks_off
        put("header +0x00 - block array", ("%+d" % (h[0] - blk)) if h[0] < len(c.d) else "0x%x (past the end)" % h[0])
        put("header +0x20", h[8])
        put("header +0x38 - breakable count", h[14] - len(c.brk))
        put("header +0x3c", h[15])
        put("header +0x40", h[16])
        put("header +0x2c - (polygons - vertices) / 12", h[11] - (h[3] - h[2]) // 12)
        put("grid +0x0e - count / width", c.grid["field_0x0e"] - c.grid["count"] // max(1, c.grid["width"]))
        put("grid pad", c.grid["pad"])
        put("block size", len(c.d) - c.blocks[0] if c.blocks else None)
        put("block all zero", c.block_zero)
        for p in c.polys:
            put("polygon +0x00", p["off"])
            put("polygon kind", "0x%x" % p["kind"])
            put("polygon edges", p["edges"])
            put("polygon +0x08/+0x09/+0x0b", (p["field_0x08"], p["field_0x09"], p["field_0x0b"]))
            put("polygon +0x14", p["field_0x14"])
            if p["edges"] == 3:
                put("triangle v[4]", "= v[0]" if p["v"][4] == p["v"][0] else "= v[%s]" % p["v"].index(p["v"][4]))
            put("kind 0x200 and slope", (p["kind"] & 0x200 != 0, p["slope"]))
        for b in c.brk:
            put("breakable +0x00/+0x02", (b["field_0x00"], b["field_0x02"]))
            put("breakable polygons", len(b["polys"]))
            cen = [sum(c.vert(c.polys[c.pindex[q]]["v"][i])[j] for q in b["polys"] for i in range(4)) /
                   (4 * len(b["polys"])) for j in range(3)]
            put("breakable +0x08 - polygon center (max abs)", round(max(abs(cen[j] - b["field_0x08"][j])
                                                                    for j in range(3)), 2))
            put("breakable number - index", b["number"] - c.brk.index(b))
    w("field values over all files (value: count):")
    for k in vals:
        w("  %-44s %s" % (k, ", ".join("%s: %d" % (v, n) for v, n in sorted(vals[k].items(), key=lambda t: str(t[0])))))
    used = {}
    for s in stages:
        used.setdefault(s["coli_file"], []).append(s["name"])
    w("stage records per file: " + "; ".join("%s %s" % (f, "/".join(n)) for f, n in sorted(used.items())))
    w("files no stage record names: " + " ".join(c.name for c in cols if c.name not in used))


def live(ram_path, asset_dir, ram_base=RAM_BASE, out=sys.stdout):
    """compares the game's relocated copy with the file, from a RAM dump."""
    w = lambda s: out.write(s + "\n")
    r = open(ram_path, "rb").read()
    rw = lambda a: u32(r, a - ram_base)
    rf = lambda a: f32(r, a - ram_base)
    sc = rw(STAGE_CUR)
    name = r[rw(sc + 0x1C) - ram_base:r.index(b"\0", rw(sc + 0x1C) - ram_base)].decode("latin1")
    sname = r[rw(sc) - ram_base:r.index(b"\0", rw(sc) - ram_base)].decode("latin1")
    base = rw(FIELD_COLI)
    rec = (rw(FIELD_LOAD), rw(FIELD_LOAD + 4))
    d = open(os.path.join(asset_dir, name), "rb").read()
    m = r[base - ram_base:base - ram_base + len(d)]
    c = Coli(d, name)
    w("stage %r loads %s; field_load record 0x%08x = { 0x%08x, 0x%x }, field_coli 0x%08x" % (
        sname, name, FIELD_LOAD, rec[0], rec[1], base))
    exp = field_coli_set(d, 0, base)
    diff = [o for o in range(0, len(d) & ~3, 4) if u32(exp, o) != u32(m, o)]
    reloc = sum(1 for o in c.ptrs if u32(m, o) == u32(d, o) + base)
    offs = [i for i, p in enumerate(c.polys) if u32(m, p["offset"]) != 0]
    poly_off = {c.polys[i]["offset"] for i in offs}
    other = [o for o in diff if o not in poly_off and o not in (0x28, 0x44, 0x48)]
    w("  %d of %d pointer words equal file + 0x%08x; %d words differ from the port of field_coli_set; "
      "%d polygons switched off (+0x00 != 0): %s" % (reloc, len(c.ptrs), base, len(diff), len(offs),
                                                      " ".join("%d" % i for i in offs) or "none"))
    w("  run time: +0x28 %08x (port %08x)  +0x44 %08x  +0x48 %08x" % (u32(m, 0x28), u32(exp, 0x28), u32(m, 0x44),
                                                                      u32(m, 0x48)))
    for o in other[:40]:
        w("    +%04x %-28s file %08x expected %08x memory %08x" % (o, describe_word(c, o), u32(d, o), u32(exp, o),
                                                                   u32(m, o)))

    def poly_at(a):
        if a == 0:
            return "null"
        i = c.pindex.get(a - base)
        return "polygon %d (%s)" % (i, kind_name(c.polys[i]["kind"])) if i is not None else "NOT a polygon start"
    fp = rw(COLI_FLOOR_POLY)
    w("  coli_floor_poly 0x%08x: %s" % (fp, poly_at(fp)))
    ok = not other and reloc == len(c.ptrs) and (fp == 0 or fp - base in c.pindex)
    # the run-time polygon test reads the game's copy: use the switched-off
    # words as they are in memory
    for i in offs:
        c.polys[i]["off"] = s32(m, c.polys[i]["offset"])
    for k in range(2):
        b = ROBS + ROB_SIZE * k
        cr = rw(b + 0x3C)
        # rob_floor_check runs before robs_coli_shift_apply moves the rob by
        # the frame's total shift (coli record +0x370, rob_coli_offset): the
        # position it tested is +0x90 minus that shift
        sh = (rf(cr + 0x370), rf(cr + 0x374), rf(cr + 0x378))
        pos = (fl(rf(b + 0x90) - sh[0]), fl(rf(b + 0x94) - sh[1]), fl(rf(b + 0x98) - sh[2]))
        near = rw(cr + 0x48) != 0
        y, pi = floor_height(c, pos, ground=near)
        got = rf(b + 0x7C)
        wall = rw(cr + 0x98)
        good = abs(got - y) < 1e-4
        ok &= good and (wall == 0 or wall - base in c.pindex)
        w("  rob %d at (%.4f, %.4f, %.4f): +0x7C floor %.6f, port %.6f (polygon %s, ground flag %d) %s; "
          "coli record +0x98 wall %s" % (k, pos[0], pos[1], pos[2], got, y, pi, near, "OK" if good else "MISMATCH",
                                         poly_at(wall)))
        # rob_floor_check's four probes: 20 units out at the facing (+0x0E)
        # + i * 0x4000, coli_segment_hit flags 7; +0x13C any hit distance,
        # +0x12C the distance only when the polygon is on (else 20), bits
        # +0x15C (from the unshifted position above)
        ang = s16(r, b + 0x0E - ram_base)
        bits, worst, same = 0, 0.0, True
        line = []
        for i in range(4):
            a = (ang + 0x4000 * i + 0x8000) % 0x10000 - 0x8000
            h = segment_hit(c, heading_move(a, pos, 20.0), pos, 7)
            d1 = math.dist(pos, h[1]) if h else 20.0
            on = h is not None and c.polys[h[0]]["off"] == 0
            d0 = d1 if on else 20.0
            bits |= (1 << i) if on else 0
            g0, g1 = rf(b + 0x12C + 4 * i), rf(b + 0x13C + 4 * i)
            same &= (g0 == 20.0) == (d0 == 20.0) and (g1 == 20.0) == (d1 == 20.0)
            worst = max(worst, abs(g0 - d0), abs(g1 - d1))
            line.append("%.4f/%.4f" % (g1, d1))
        gbits = r[b + 0x15C - ram_base]
        same &= gbits == bits
        ok &= same
        w("    probes +0x13C game/port %s; bits +0x15C %x port %x; largest difference %.3g; %s" % (
            " ".join(line), gbits, bits, worst, "same hits" if same else "DIFFERENT HITS"))
    return ok


def main(argv):
    if not argv or argv[0] in ("-h", "--help"):
        print(__doc__)
        return 0
    opt = lambda k, dflt: argv[argv.index(k) + 1] if k in argv else dflt
    pos = [x for i, x in enumerate(argv) if not x.startswith("--") and (i == 0 or argv[i - 1] not in ("--json", "--obj", "--ram-base"))]
    here = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    main_bin = os.path.join(here, "work", "main.bin")
    if pos[0] == "all":
        dirp, outd = pos[1], pos[2]
        os.makedirs(outd, exist_ok=True)
        files = sorted(glob.glob(os.path.join(dirp, "st_*_col.bin")))
        cols = ["vertices", "polygons", "floor", "out", "wall", "breakable", "cells", "lists", "list_entries",
                "breakable_walls", "pointers"]
        print("%-20s %6s " % ("file", "bytes") + " ".join("%6s" % k[:6] for k in cols) + "    gap")
        tot = {}
        for p in files:
            c = Coli(open(p, "rb").read(), os.path.basename(p))
            k = c.counts()
            gap = sum(s for _, s in c.gaps()[0])
            base = os.path.join(outd, c.name[:-4])
            with open(base + ".json", "w") as f:
                json.dump(c.to_json(), f, indent=1)
            with open(base + ".obj", "w") as f:
                c.obj(f)
            for x in cols:
                tot[x] = tot.get(x, 0) + k[x]
            tot["bytes"] = tot.get("bytes", 0) + len(c.d)
            tot["gap"] = tot.get("gap", 0) + gap
            print("%-20s %6d " % (c.name, len(c.d)) + " ".join("%6d" % k[x] for x in cols) + " %6d" % gap)
            for n in c.notes:
                print("    note: " + n)
        print("%-20s %6d " % ("total (%d)" % len(files), tot["bytes"]) + " ".join("%6d" % tot[x] for x in cols)
              + " %6d" % tot["gap"])
        return 0
    if pos[0] == "check":
        ok = check_all(pos[1], pos[2] if len(pos) > 2 else main_bin)
        print("PASS" if ok else "FAIL")
        return 0 if ok else 1
    if pos[0] == "floor":
        c = Coli(open(pos[1], "rb").read(), os.path.basename(pos[1]))
        p = tuple(float(x) for x in pos[2:5])
        y, pi = floor_height(c, p, ground="--ground" in argv)
        print("floor %.6f polygon %s %s" % (y, pi, kind_name(c.polys[pi]["kind"]) if pi is not None else "(none: kind 0x400)"))
        return 0
    if pos[0] == "live":
        ok = live(pos[1], pos[2] if len(pos) > 2 else os.path.join(here, "work", "assets"),
                  int(opt("--ram-base", "%x" % RAM_BASE), 16))
        print("PASS" if ok else "FAIL")
        return 0 if ok else 1
    c = Coli(open(pos[0], "rb").read(), os.path.basename(pos[0]))
    dump(c)
    if "--json" in argv:
        with open(opt("--json", None), "w") as f:
            json.dump(c.to_json(), f, indent=1)
    if "--obj" in argv:
        with open(opt("--obj", None), "w") as f:
            c.obj(f)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
