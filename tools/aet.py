#!/usr/bin/env python3
"""Decode the *_aet.bin 2D scene files of the sprite library (libsprite).

The layout is the one the matched C reads (source/vf4lib/libsprite/src/
aetset.c, aet_anim_if.c, aet_play.c); every named field below cites the
function that reads or writes it. Words no code reads are field_0xNN.

  file header, 4 words (aet_file_relocate relocates all four)
    +0x00  -> scene count (aet_file_relocate, aet_set_scene_count)
    +0x04  -> scene table: count words, one scene each (aet_scene_get)
    +0x08  -> second table: count words (aet_scene_table2_get; nothing
              calls it; in every file each word points at a string)
    +0x0C  -> pair table: count x { tex table, sprite set list }
              (aet_scene_tex_table_get, aet_scene_sprite_sets_get)
  scene, 0x18 bytes (aet_scene_relocate)
    +0x00 composition count, +0x04 -> compositions (0x2C each; the last
    one is the scene's main composition, aet_main_comp_get)
    +0x08 image count, +0x0C -> images (0x18 each; aet_scene_textures_load)
    +0x10 third-table count, +0x14 -> third table (0x18 each; its entries
    are not relocated, aet_table3_item_relocate does nothing)
  composition, 0x2C bytes (aet_comp_relocate)
    +0x08 start frame, +0x0C end frame (floats; aet_comp_visible_update,
    aet_obj_start), +0x20 visible byte (written at run time by
    aet_comp_visible_update), +0x24 layer count, +0x28 -> layers
  layer, 0x8C bytes (aet_layer_relocate)
    +0x00 -> name (aet_comp_layer_find), +0x04 start frame, +0x08 end frame
    (aet_layer_tracks_eval, aet_obj_start), +0x0C frame offset, +0x10 time
    scale (aet_layer_draw: child frame = (frame - start) * scale + offset),
    +0x14 .. +0x30 eight floats written at run time from the eight tracks
    (aet_layer_tracks_eval; aet_layer_transform reads them as anchor x/y,
    position x/y, scale x/y percent, rotation degrees; aet_layer_draw reads
    +0x30 as opacity percent), +0x34 visible byte (run time,
    aet_layer_tracks_eval), +0x35 blend byte (aet_image_layer_draw,
    aet_blend_select: 4 additive), +0x37 no-send byte (aet_image_layer_draw),
    +0x38 16-bit flags (bit 0: visible only from start to end,
    aet_layer_tracks_eval), +0x3A type byte (0 image, 1 composition,
    2 box; aet_layer_draw), +0x3C -> item (image, composition or box),
    +0x40 -> parent layer (aet_layer_transform), +0x44 .. +0x80 eight tracks
    { key count, -> keys } (aet_layer_tracks_eval), +0x84 marker count,
    +0x88 -> markers (aet_layer_marker_find)
  key, 16 bytes: { frame, value, in slope, out slope } (aet_key_curve_eval)
  marker, 8 bytes: { frame (float), -> name } (aet_obj_marker_frame,
    aet_layer_marker_find, aet_marker_relocate)
  image, 0x18 bytes (aet_image_relocate)
    +0x00 width, +0x04 height (ints; aet_image_layer_draw, aet_quad_build),
    +0x08 picture rate (int; aet_image_picture_set: picture =
    (int)(rate * frame)), +0x0C picture count, +0x10 -> pictures,
    +0x14 current picture (run time, aet_image_picture_set)
  picture, 0x14 bytes: +0x00 index into the scene's tex table
    (aet_image_layer_draw, aet_scene_textures_load), +0x04 u0, +0x08 v0,
    +0x0C u1, +0x10 v1 (aet_quad_build)
  tex table: ints, texture numbers indexed by picture (aet_scene_textures_load)
  sprite set list: ints ended by -1 (aet_scene_textures_load)

usage:
  python3 -I tools/aet.py FILE [--json OUT] [--header H0,H1,H2,H3]
        readable dump (and JSON); --header (hex offsets) for a file whose
        first words are not its header (adv_rank_aet.bin: 448,44c,464,45c)
  python3 -I tools/aet.py all DIR OUT             every *_aet.bin in DIR to
                                                  OUT/<name>.json + summary
  python3 -I tools/aet.py check FILE MEM BASE [STATE_BIN] [--round M] [--fmac M]
        compares the file with the game's relocated copy MEM loaded at BASE
        (hex): (a) relocated words, (b) run-time words, and with STATE_BIN
        (RAM 0x0C4ECC04 .. 0x0C50E898) (c) the objects and stills in use
        and the layer floats the evaluator gives for their frames
  python3 -I tools/aet.py checkdir DUMPDIR ASSETDIR [--round M] [--fmac M]
        check for every set in DUMPDIR/index.tsv
  python3 -I tools/aet.py eval FILE SCENE COMP LAYER FRAME
        the eight track values of one layer at a frame
--round nearest|zero (default nearest) and --fmac fused|split (default fused)
choose the SH-4 float model of the evaluator; check tries all four and
names the one used.
"""
import glob, json, os, struct, sys
from fractions import Fraction

SCENE, COMP, LAYER, IMAGE, PICTURE, TABLE3 = 0x18, 0x2C, 0x8C, 0x18, 0x14, 0x18
TRACK_NAMES = ["anchor_x", "anchor_y", "pos_x", "pos_y", "scale_x", "scale_y", "rotation", "opacity"]
STATE_BASE, STATE_END = 0x0C4ECC04, 0x0C50E898
OBJS, OBJ_SIZE = 0x0C4EE188, 0x78
STILLS, STILL_SIZE = 0x0C4EFF90, 0x6C
SETS, SET_SIZE = 0x0C4ECC04, 0x90
PROGRAM_BASE = 0x0C020000


def u32(d, o):
    return struct.unpack_from("<I", d, o)[0]


def s32(d, o):
    return struct.unpack_from("<i", d, o)[0]


def f32(d, o):
    return struct.unpack_from("<f", d, o)[0]


def cstr(d, o):
    if not (0 <= o < len(d)):
        return None
    e = d.find(b"\0", o)
    return d[o:e if e >= 0 else len(d)].decode("latin1")


# ---------------------------------------------------------------------------
# SH-4 single precision (FPSCR.PR = 0); -m4-single-only makes every double
# in the C a float. Results are rounded exactly from rational arithmetic.

class F32:
    def __init__(self, mode="nearest", fmac="fused"):
        self.mode, self.fmac_mode = mode, fmac

    def r(self, x):
        """round a Fraction to the nearest float32 (mode 'nearest', ties to
        even) or toward zero (mode 'zero', FPSCR.RM = 1)."""
        if x == 0:
            return 0.0
        neg = x < 0
        x = -x if neg else x
        e = x.numerator.bit_length() - x.denominator.bit_length()
        if Fraction(2) ** e > x:
            e -= 1
        e = max(e, -126)
        scale = Fraction(2) ** (e - 23)
        q = x / scale
        m = q.numerator // q.denominator
        rem = q - m
        if self.mode == "nearest" and (rem > Fraction(1, 2) or (rem == Fraction(1, 2) and m & 1)):
            m += 1
        v = float(m * scale)
        v = struct.unpack("<f", struct.pack("<f", v))[0]
        return -v if neg else v

    def add(self, a, b): return self.r(Fraction(a) + Fraction(b))
    def sub(self, a, b): return self.r(Fraction(a) - Fraction(b))
    def mul(self, a, b): return self.r(Fraction(a) * Fraction(b))
    def div(self, a, b): return self.r(Fraction(a) / Fraction(b))

    def fmac(self, f0, fm, fn):
        """FMAC FR0,FRm,FRn: FR0 * FRm + FRn -> FRn."""
        if self.fmac_mode == "fused":
            return self.r(Fraction(f0) * Fraction(fm) + Fraction(fn))
        return self.add(self.mul(f0, fm), fn)


def ftrc(x):
    """FTRC: float to int, toward zero."""
    return int(x)


def key_curve_eval(fp, keys, frame):
    """aet_key_curve_eval, in the order of the game's instructions
    (0x0C0E6140): keys is a list of (frame, value, in slope, out slope)."""
    n = len(keys)
    if n <= 0:
        return 0.0
    if n == 1:
        return keys[0][1]
    i = 0
    while i < n and frame > keys[i][0]:      # fcmp/gt fr1,fr4: frame > k[0]
        i += 1
    if i >= n:
        return keys[n - 1][1]                 # k[-3]
    if i == 0:
        return keys[0][1]                     # k[1]
    k, p = keys[i], keys[i - 1]
    t0 = p[0]                                 # fr12 = k[-4]
    d = fp.sub(frame, t0)                     # fr13
    if abs(fp.sub(k[0], t0)) < 1.1920929e-07:
        raise ValueError("aet_key_curve_eval: two keys on frame %g (the game's fatal_at)" % t0)
    span = fp.sub(k[0], t0)
    inv = fp.div(1.0, span)                   # fr2
    inv2 = fp.mul(inv, inv)                   # fr3
    d2 = fp.mul(d, d)                         # fr1
    a = fp.mul(d2, inv)                       # fr7
    d3 = fp.mul(fp.mul(d2, d), inv2)          # fr4
    c2 = fp.mul(fp.add(d3, d3), inv)          # fr5
    b = fp.mul(fp.mul(d2, 3.0), inv2)         # fr1
    d3 = fp.sub(d3, a)                        # fr4 -= fr7
    h00 = fp.add(fp.sub(c2, b), 1.0)          # fr3
    acc = fp.mul(fp.sub(b, c2), k[1])         # fr1 = (b - c2) * k[1]
    acc = fp.fmac(h00, p[1], acc)             # + h00 * k[-3]
    h10 = fp.add(fp.sub(d3, a), d)            # fr2
    acc = fp.fmac(h10, p[3], acc)             # + h10 * k[-1]
    acc = fp.fmac(d3, k[2], acc)              # + d3 * k[2]
    return acc


def layer_tracks_eval(fp, layer, frame):
    """the eight-track part of aet_layer_tracks_eval: (visible byte,
    eight floats for +0x14 .. +0x30)."""
    if layer["flags"] & 1 == 0:
        vis = 1
    elif layer["start_frame"] > frame or not (layer["end_frame"] > frame):
        vis = 0
    else:
        vis = 1
    return vis, [key_curve_eval(fp, t["keys"], frame) for t in layer["tracks"]]


# ---------------------------------------------------------------------------
# parse

class Aet:
    """the structure of one AET file, by file offset. relocs maps each word
    the loader turns into a pointer to the function that does it; zeros
    lists pointer words that are 0 (the loader leaves them), unreached
    lists words aet_ptr_array_relocate never gets to (it stops advancing at
    a zero word)."""

    def __init__(self, data, name="", header=None):
        self.d, self.name, self.given_header = data, name, header
        self.relocs, self.zeros, self.unreached = {}, [], []
        self.double = []
        self.cover = {}                       # offset -> (size, kind)
        self.runtime = []                     # (offset, size, what, writer)
        self.notes = []
        self.parse()

    # relocation bookkeeping, following the code's walk
    def rel(self, off, fn):
        if u32(self.d, off) == 0:
            self.zeros.append((off, fn))
            return 0
        if off in self.relocs:
            self.double.append((off, fn))
        self.relocs[off] = fn
        return u32(self.d, off)

    def rel_array(self, off, count, fn):
        """aet_ptr_array_relocate: p moves on only past a non-zero word."""
        j = 0
        for _ in range(count):
            if u32(self.d, off + 4 * j) != 0:
                self.rel(off + 4 * j, fn)
                j += 1
        if j < count:
            self.zeros.append((off + 4 * j, fn))
            for k in range(j + 1, count):
                self.unreached.append((off + 4 * k, fn))
            if any(u32(self.d, off + 4 * k) for k in range(j + 1, count)):
                self.notes.append("%s array +0x%x: non-zero words after a zero are not relocated" % (fn, off))

    def mark(self, off, size, kind):
        self.cover.setdefault(off, (size, kind))

    def parse(self):
        d = self.d
        self.comps, self.layers, self.images, self.scenes = {}, {}, {}, []
        if self.given_header:
            # a file whose first words are not the header (adv_rank_aet.bin):
            # parse from header words given on the command line
            self.header = list(self.given_header)
            self.notes.append("header words given: %s (the file's own first words are %s)" % (
                " ".join("0x%x" % h for h in self.header), " ".join("%08x" % u32(d, 4 * i) for i in range(4))))
        else:
            self.header = [u32(d, 4 * i) for i in range(4)]
            self.mark(0, 16, "header")
            for i in range(4):
                self.rel(4 * i, "aet_file_relocate")
            if self.header[0] == 0:
                self.notes.append("header word 0 is 0: aet_file_relocate would read the scene count through "
                                  "a null pointer; this file has no header (try --header)")
                self.index()
                return
        h0, h1, h2, h3 = self.header
        n = u32(d, h0)
        self.mark(h0, 4, "scene count")
        self.rel_array(h1, n, "aet_ptr_array_relocate (scene table)")
        self.rel_array(h2, n, "aet_ptr_array_relocate (second table)")
        self.rel_array(h3, 2 * n, "aet_ptr_array_relocate (pair table)")
        self.mark(h1, 4 * n, "scene table")
        self.mark(h2, 4 * n, "second table")
        self.mark(h3, 8 * n, "pair table")
        for i in range(n):
            so = u32(d, h1 + 4 * i)
            t2 = u32(d, h2 + 4 * i)
            tex, sets = u32(d, h3 + 8 * i), u32(d, h3 + 8 * i + 4)
            sc = self.scene(so, i)
            sc["table2"] = t2
            sc["table2_string"] = cstr(d, t2) if t2 else None
            if t2:
                self.mark(t2, len(sc["table2_string"]) + 1, "string")
            sc["sprite_sets_offset"] = sets
            sc["sprite_sets"] = self.int_list_m1(sets) if sets else None
            if sets:
                self.mark(sets, 4 * (len(sc["sprite_sets"]) + 1), "sprite set list")
            used = [p["tex_index"] for im in sc["images"] for p in im["pictures"]]
            sc["tex_table_offset"] = tex
            ntex = max(used) + 1 if used else 0
            sc["tex_table"] = [s32(d, tex + 4 * k) for k in range(ntex)] if tex else None
            if tex and ntex:
                self.mark(tex, 4 * ntex, "tex table")
            self.scenes.append(sc)
        self.index()

    def int_list_m1(self, off):
        out = []
        while off + 4 <= len(self.d):
            v = s32(self.d, off)
            if v == -1:
                break
            out.append(v)
            off += 4
        return out

    def scene(self, so, idx):
        d = self.d
        w = [u32(d, so + 4 * k) for k in range(6)]
        self.mark(so, SCENE, "scene")
        for k in (1, 3, 5):
            self.rel(so + 4 * k, "aet_scene_relocate")
        sc = {"index": idx, "offset": so, "comp_count": w[0], "comps_offset": w[1],
              "image_count": w[2], "images_offset": w[3], "table3_count": w[4], "table3_offset": w[5]}
        sc["comps"] = [self.comp(w[1] + COMP * k, idx, k) for k in range(w[0])]
        sc["images"] = [self.image(w[3] + IMAGE * k) for k in range(w[2])]
        sc["table3"] = []
        for k in range(w[4]):
            o = w[5] + TABLE3 * k
            self.mark(o, TABLE3, "third-table entry")
            sc["table3"].append({"offset": o, **{"field_0x%02x" % (4 * j): u32(d, o + 4 * j) for j in range(6)}})
        return sc

    def comp(self, co, si, ci):
        d = self.d
        self.mark(co, COMP, "composition")
        c = {"offset": co, "scene": si, "index": ci}
        for j in (0, 4, 0x10, 0x14, 0x18, 0x1C):
            c["field_0x%02x" % j] = u32(d, co + j)
        c["start_frame"], c["end_frame"] = f32(d, co + 8), f32(d, co + 0xC)
        c["visible"] = d[co + 0x20]
        c["field_0x21"] = d[co + 0x21:co + 0x24].hex()
        c["layer_count"], c["layers_offset"] = s32(d, co + 0x24), u32(d, co + 0x28)
        self.runtime.append((co + 0x20, 1, "comp visible", "aet_comp_visible_update"))
        self.rel(co + 0x28, "aet_comp_relocate")
        c["layers"] = [self.layer(c["layers_offset"] + LAYER * k, co) for k in range(max(c["layer_count"], 0))]
        self.comps[co] = c
        return c

    def layer(self, lo, co):
        d = self.d
        self.mark(lo, LAYER, "layer")
        L = {"offset": lo, "comp": co}
        L["name_offset"] = u32(d, lo)
        L["name"] = cstr(d, L["name_offset"]) if L["name_offset"] else None
        if L["name_offset"]:
            self.mark(L["name_offset"], len(L["name"]) + 1, "string")
        L["start_frame"], L["end_frame"] = f32(d, lo + 4), f32(d, lo + 8)
        L["frame_offset"], L["time_scale"] = f32(d, lo + 0xC), f32(d, lo + 0x10)
        L["runtime_floats"] = [f32(d, lo + 0x14 + 4 * k) for k in range(8)]
        L["visible"], L["blend"], L["field_0x36"], L["no_send"] = d[lo + 0x34], d[lo + 0x35], d[lo + 0x36], d[lo + 0x37]
        L["flags"] = struct.unpack_from("<H", d, lo + 0x38)[0]
        L["type"], L["field_0x3b"] = d[lo + 0x3A], d[lo + 0x3B]
        L["item_offset"], L["parent_offset"] = u32(d, lo + 0x3C), u32(d, lo + 0x40)
        self.runtime.append((lo + 0x14, 0x20, "layer track values", "aet_layer_tracks_eval"))
        self.runtime.append((lo + 0x34, 1, "layer visible", "aet_layer_tracks_eval"))
        for o in (0, 0x3C, 0x40, 0x88):
            self.rel(lo + o, "aet_layer_relocate")
        L["tracks"] = []
        for k in range(8):
            to = lo + 0x44 + 8 * k
            cnt, ko = s32(d, to), u32(d, to + 4)
            self.rel(to + 4, "aet_track_relocate")
            keys = [struct.unpack_from("<4f", d, ko + 16 * j) for j in range(max(cnt, 0))] if ko else []
            if ko and cnt > 0:
                self.mark(ko, 16 * cnt, "keys")
            L["tracks"].append({"name": TRACK_NAMES[k], "count": cnt, "keys_offset": ko, "keys": keys})
        L["marker_count"], L["markers_offset"] = s32(d, lo + 0x84), u32(d, lo + 0x88)
        L["markers"] = []
        for k in range(max(L["marker_count"], 0)):
            mo = L["markers_offset"] + 8 * k
            no = u32(d, mo + 4)
            self.rel(mo + 4, "aet_marker_relocate")
            nm = cstr(d, no) if no else None
            if no:
                self.mark(no, len(nm) + 1, "string")
            L["markers"].append({"offset": mo, "frame": f32(d, mo), "name_offset": no, "name": nm})
        if L["marker_count"] > 0:
            self.mark(L["markers_offset"], 8 * L["marker_count"], "markers")
        self.layers[lo] = L
        return L

    def image(self, io):
        d = self.d
        self.mark(io, IMAGE, "image")
        im = {"offset": io, "width": s32(d, io), "height": s32(d, io + 4), "picture_rate": s32(d, io + 8),
              "picture_count": s32(d, io + 0xC), "pictures_offset": u32(d, io + 0x10),
              "current_picture": s32(d, io + 0x14)}
        self.runtime.append((io + 0x14, 4, "image current picture", "aet_image_picture_set"))
        self.rel(io + 0x10, "aet_image_relocate")
        im["pictures"] = []
        for k in range(max(im["picture_count"], 0)):
            po = im["pictures_offset"] + PICTURE * k
            im["pictures"].append({"offset": po, "tex_index": s32(d, po),
                                   "uv": list(struct.unpack_from("<4f", d, po + 4))})
        if im["picture_count"] > 0 and im["pictures_offset"]:
            self.mark(im["pictures_offset"], PICTURE * im["picture_count"], "pictures")
        self.images[io] = im
        return im

    def index(self):
        """what each layer's item and parent are, by the parse."""
        for L in self.layers.values():
            it = L["item_offset"]
            L["item_kind"] = ("composition" if it in self.comps else "image" if it in self.images
                              else "none" if it == 0 else "other")
            p = L["parent_offset"]
            L["parent_kind"] = "layer" if p in self.layers else "none" if p == 0 else "other"

    def coverage(self):
        """byte runs of the file no parsed structure covers."""
        got = bytearray(len(self.d))
        for o, (s, _) in self.cover.items():
            got[o:o + s] = b"\1" * len(got[o:o + s])
        gaps, i = [], 0
        while i < len(got):
            if not got[i]:
                j = i
                while j < len(got) and not got[j]:
                    j += 1
                gaps.append((i, j - i, self.d[i:j]))
                i = j
            else:
                i += 1
        return gaps

    def real_gaps(self):
        """uncovered runs that are not 1-3 zero bytes padding a string to a
        word boundary."""
        return [(o, s, b) for o, s, b in self.coverage()
                if not (s < 4 and (o + s) % 4 == 0 and not b.strip(b"\0"))]

    def counts(self):
        tracks = [t for L in self.layers.values() for t in L["tracks"]]
        return {"scenes": len(self.scenes), "comps": len(self.comps), "layers": len(self.layers),
                "images": len(self.images),
                "pictures": sum(len(i["pictures"]) for i in self.images.values()),
                "tracks_with_keys": sum(1 for t in tracks if t["count"] > 0),
                "keys": sum(max(t["count"], 0) for t in tracks),
                "markers": sum(max(L["marker_count"], 0) for L in self.layers.values()),
                "table3": sum(len(s["table3"]) for s in self.scenes),
                "relocated_words": len(self.relocs), "zero_pointer_words": len(self.zeros)}

    def to_json(self):
        def strip(sc):
            return sc
        return {"file": self.name, "size": len(self.d), "header": self.header, "counts": self.counts(),
                "scenes": [strip(s) for s in self.scenes],
                "uncovered": [{"offset": o, "size": s, "hex": b[:64].hex()} for o, s, b in self.real_gaps()],
                "notes": self.notes}


# ---------------------------------------------------------------------------
# dump

def fmt_keys(keys):
    return " ".join("(%g %g %g %g)" % k for k in keys)


def dump(a, out=sys.stdout):
    w = lambda s="": out.write(s + "\n")
    c = a.counts()
    w("%s: %d bytes; %d scenes, %d compositions, %d layers, %d images, %d pictures, %d keys, %d markers"
      % (a.name, len(a.d), c["scenes"], c["comps"], c["layers"], c["images"], c["pictures"], c["keys"], c["markers"]))
    w("header: count -> +0x%x, scenes +0x%x, second table +0x%x, pairs +0x%x" % tuple(a.header))
    for sc in a.scenes:
        w()
        w("scene %d at +0x%x  second-table string %r" % (sc["index"], sc["offset"], sc["table2_string"]))
        w("  sprite sets %s (list +0x%x), tex table +0x%x: %s" % (
            sc["sprite_sets"], sc["sprite_sets_offset"], sc["tex_table_offset"],
            " ".join("%d" % t for t in (sc["tex_table"] or []))))
        for im in sc["images"]:
            w("  image +0x%x: %dx%d rate %d, %d pictures: %s" % (
                im["offset"], im["width"], im["height"], im["picture_rate"], im["picture_count"],
                ", ".join("tex[%d] uv %s" % (p["tex_index"], " ".join("%.4f" % u for u in p["uv"]))
                          for p in im["pictures"][:4]) + (" ..." if im["picture_count"] > 4 else "")))
        for t in sc["table3"]:
            w("  third-table +0x%x: %s" % (t["offset"], " ".join("%08x" % t["field_0x%02x" % (4 * j)] for j in range(6))))
        for cp in sc["comps"]:
            main = " (main)" if cp is sc["comps"][-1] else ""
            w("  composition %d at +0x%x%s: frames %g..%g, %d layers  [+0 %08x +4 %08x +10 %08x +14 %08x +18 %08x +1c %08x]" % (
                cp["index"], cp["offset"], main, cp["start_frame"], cp["end_frame"], cp["layer_count"],
                cp["field_0x00"], cp["field_0x04"], cp["field_0x10"], cp["field_0x14"], cp["field_0x18"], cp["field_0x1c"]))
            for L in cp["layers"]:
                kind = {"composition": "comp +0x%x" % L["item_offset"], "image": "image +0x%x" % L["item_offset"],
                        "none": "no item"}.get(L["item_kind"], "item +0x%x (%s)" % (L["item_offset"], L["item_kind"]))
                par = " parent +0x%x" % L["parent_offset"] if L["parent_offset"] else ""
                w("    layer +0x%x %r type %d %s%s frames %g..%g offset %g scale %g flags %x blend %d no_send %d [+36 %d +3b %d]" % (
                    L["offset"], L["name"], L["type"], kind, par, L["start_frame"], L["end_frame"],
                    L["frame_offset"], L["time_scale"], L["flags"], L["blend"], L["no_send"], L["field_0x36"], L["field_0x3b"]))
                for t in L["tracks"]:
                    if t["count"] > 0:
                        w("      %-8s %d: %s" % (t["name"], t["count"], fmt_keys(t["keys"][:6]) + (" ..." if t["count"] > 6 else "")))
                for m in L["markers"]:
                    w("      marker %g %r" % (m["frame"], m["name"]))
    allg, gaps = a.coverage(), a.real_gaps()
    w()
    w("bytes no structure covers: %d in %d runs, of them %d bytes in %d runs that are not zero padding to a word"
      % (sum(s for _, s, _ in allg), len(allg), sum(s for _, s, _ in gaps), len(gaps)))
    for o, s, b in gaps[:20]:
        w("  +0x%x %d: %s" % (o, s, b[:32].hex()))
    for n in a.notes:
        w("note: " + n)


# ---------------------------------------------------------------------------
# live checks

class Sim:
    """replays the writes the AET draw pass makes into one file buffer:
    aet_objs_draw / aet_obj_draw / aet_layer_draw / aet_comp_draw and the
    still build (aet_still_build / aet_still_layer_draw / aet_still_comp_draw).
    writes maps file offset -> (value, who)."""

    def __init__(self, a, base, fp):
        self.a, self.base, self.fp = a, base, fp
        self.writes = {}
        self.log = []
        self.who = None

    def off(self, p):
        o = p - self.base
        return o if 0 <= o < len(self.a.d) else None

    def put(self, o, v):
        self.writes[o] = (v, self.who)
        self.log.append((o, v, self.who))

    def layer_transform(self, lo, frame):
        if lo is None:
            return
        L = self.a.layers[lo]
        self.layer_transform(L["parent_offset"] or None, frame)
        vis, vals = layer_tracks_eval(self.fp, L, frame)
        self.put(lo + 0x34, ("b", vis))
        for k, v in enumerate(vals):
            self.put(lo + 0x14 + 4 * k, ("f", v))
        self.vis[lo] = vis

    def picture_set(self, io, frame):
        im = self.a.images[io]
        if im["picture_count"] <= 1:
            r = 0
        else:
            v = ftrc(self.fp.mul(float(im["picture_rate"]), frame))
            r = v if v < im["picture_count"] else im["picture_count"] - 1
        self.put(io + 0x14, ("i", r))

    def layer_draw(self, lo, frame):
        L = self.a.layers[lo]
        if not L["item_offset"]:
            return
        self.layer_transform(lo, frame)
        if self.vis[lo]:
            cf = self.fp.fmac(self.fp.sub(frame, L["start_frame"]), L["time_scale"], L["frame_offset"])
            if L["type"] == 1:
                self.comp_draw(L["item_offset"], cf)
            elif L["type"] == 0:
                self.picture_set(L["item_offset"], cf)

    def comp_visible(self, co, frame):
        c = self.a.comps[co]
        v = 0 if (frame < c["start_frame"] or c["end_frame"] <= frame) else 1
        self.put(co + 0x20, ("b", v))
        return v

    def comp_draw(self, co, frame):
        if self.comp_visible(co, frame):
            c = self.a.comps[co]
            for i in range(c["layer_count"] - 1, -1, -1):
                self.layer_draw(c["layers_offset"] + LAYER * i, frame)

    def obj_draw(self, comp, layer, frame):
        self.vis = {}
        if layer:
            self.layer_draw(layer, float(frame))
        else:
            self.comp_draw(comp, float(frame))

    # stills (frame 0)
    def still_build(self, comp, layer, counter):
        self.vis = {}
        self.counter, self.compflag = counter, False
        if layer and self.a.layers[layer]["type"] == 1:
            self.counter = self.a.comps[self.a.layers[layer]["item_offset"]]["layer_count"] - 1
            self.compflag = True
        if layer:
            self.still_layer_draw(layer)
        else:
            self.still_comp_draw(comp)

    def still_layer_draw(self, lo):
        L = self.a.layers[lo]
        if not L["item_offset"]:
            return
        if self.compflag:
            self.layer_transform(lo, 0.0)
        if L["type"] == 1:
            self.still_comp_draw(L["item_offset"])
        elif L["type"] == 0:
            self.picture_set(L["item_offset"], 0.0)

    def still_comp_draw(self, co):
        self.comp_visible(co, 0.0)
        c = self.a.comps[co]
        for i in range(self.counter, -1, -1):
            self.still_layer_draw(c["layers_offset"] + LAYER * i)


def words(b):
    return struct.unpack("<%dI" % (len(b) // 4), b[:len(b) // 4 * 4])


def read_objs(st):
    out = []
    for i in range(64):
        o = OBJS - STATE_BASE + OBJ_SIZE * i
        if st[o]:
            r = {"index": i, "scene_id": u32(st, o + 4), "flags": struct.unpack_from("<H", st, o + 8)[0],
                 "layer_name": u32(st, o + 0x30), "handle": s32(st, o + 0x48), "scene": u32(st, o + 0x4C),
                 "comp": u32(st, o + 0x50), "layer": u32(st, o + 0x54), "frame": s32(st, o + 0x58),
                 "start": s32(st, o + 0x5C), "end": s32(st, o + 0x60), "end0": s32(st, o + 0x64),
                 "start0": s32(st, o + 0x68), "loop": s32(st, o + 0x6C), "end_reached": s32(st, o + 0x70),
                 "hide": u32(st, o + 0x74)}
            out.append(r)
    return out


def read_stills(st):
    out = []
    for i in range(64):
        o = STILLS - STATE_BASE + STILL_SIZE * i
        if st[o]:
            out.append({"index": i, "scene_id": u32(st, o + 4), "layer_name": u32(st, o + 0x20),
                        "handle": s32(st, o + 0x50), "scene": u32(st, o + 0x54), "comp": u32(st, o + 0x58),
                        "layer": u32(st, o + 0x5C), "flags": u32(st, o + 0x60), "counter": s32(st, o + 0x64),
                        "counter2": s32(st, o + 0x68)})
    return out


def comp_layer_find(a, co, name):
    """aet_comp_layer_find: the first layer with the name, depth first
    through composition layers; None when not found."""
    c = a.comps.get(co)
    if c is None or name is None:
        return None
    for L in c["layers"]:
        if L["name"] is not None:
            if L["name"] == name:
                return L["offset"]
            if L["type"] == 1:
                r = comp_layer_find(a, L["item_offset"], name)
                if r is not None:
                    return r
    return None


def program_string(addr):
    p = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "work", "main.bin")
    if not os.path.exists(p) or addr < PROGRAM_BASE:
        return None
    with open(p, "rb") as f:
        img = f.read()
    return cstr(img, addr - PROGRAM_BASE)


def frame_of_last_draw(o):
    """the frame aet_objs_draw last drew the object at. ev_aet_task_frame runs
    aet_objs_step and then aet_objs_draw, so between task frames the frame
    word is the one drawn; one step back is what a snapshot taken between
    the step and the draw (or after other code moved the frame) would show."""
    return o["frame"]


def frame_one_step_back(o):
    """undo one aet_objs_step: the frame drawn the task frame before."""
    f = o["frame"]
    rev = o["flags"] & 0x10
    if o["flags"] & 8:
        return f                        # paused: aet_objs_step skips it
    if o["flags"] & 4 and o["end_reached"]:
        return f                        # held: the step put it back where it was
    if not rev:
        if f == o["start"] and o["flags"] & 2:
            return o["end"] - 1         # wrapped from the end
        return f - 1
    if f == o["end"] - 1 and o["flags"] & 2:
        return o["start"]
    return f + 1


def layer_search(fp, L, live, lo_hi):
    """the frames (step 1, then 0.5) at which the layer's tracks give the
    live floats."""
    hits = []
    lo, hi = lo_hi
    f = lo
    while f <= hi:
        _, vals = layer_tracks_eval(fp, L, f)
        if [struct.pack("<f", v) for v in vals] == live:
            hits.append(f)
        f += 0.5
    return hits


def check(file_path, mem_path, base, state_path=None, fp_modes=None, out=sys.stdout, verbose=True):
    w = lambda s="": out.write(s + "\n")
    fd = open(file_path, "rb").read()
    md = open(mem_path, "rb").read()
    a = Aet(fd, os.path.basename(file_path))
    ok = True
    w("== %s at %08x (%d bytes, mem %d bytes)" % (a.name, base, len(fd), len(md)))
    n = min(len(fd), len(md)) // 4
    fw, mw = words(fd[:4 * n]), words(md[:4 * n])
    live_rel = {4 * i for i in range(n) if fw[i] != mw[i] and (fw[i] + base) & 0xFFFFFFFF == mw[i]}
    other = {4 * i for i in range(n) if fw[i] != mw[i] and (fw[i] + base) & 0xFFFFFFFF != mw[i]}
    ours = set(a.relocs)
    missing, extra = live_rel - ours, ours - live_rel
    w("(a) relocated words: live %d, parse %d, missing %d, extra %d; zero pointer words the loader skips %d, "
      "array words never reached %d, words relocated twice %d"
      % (len(live_rel), len(ours), len(missing), len(extra), len(a.zeros), len(a.unreached), len(a.double)))
    for o in sorted(missing)[:10]:
        w("    missing +0x%x file %08x mem %08x" % (o, fw[o // 4], mw[o // 4]))
    for o in sorted(extra)[:10]:
        w("    extra +0x%x file %08x mem %08x (%s)" % (o, fw[o // 4], mw[o // 4], a.relocs[o]))
    ok &= not missing and not extra and not a.double
    # (b) run-time bytes
    rt = bytearray(len(fd))
    rt_who = {}
    for o, s, what, writer in a.runtime:
        for k in range(s):
            rt[o + k] = 1
            rt_who[o + k] = (what, writer)
    bad, explained = [], {}
    for o in sorted(other):
        for k in range(4):
            if fd[o + k] != md[o + k]:
                if rt[o + k]:
                    explained.setdefault(rt_who[o + k], set()).add(o)
                else:
                    bad.append(o + k)
    w("(b) other differing words %d: inside run-time fields %d, outside %d" % (
        len(other), sum(len(v) for v in explained.values()), len(set(b & ~3 for b in bad))))
    for (what, writer), s in sorted(explained.items()):
        w("    %-24s %4d words  (written by %s)" % (what, len(s), writer))
    for b in bad[:10]:
        w("    outside +0x%x: file %02x mem %02x" % (b, fd[b], md[b]))
    ok &= not bad
    res = {"name": a.name, "a_ok": not missing and not extra, "b_ok": not bad,
           "live_reloc": len(live_rel), "other": len(other)}
    if not state_path:
        return ok, res
    st = open(state_path, "rb").read()
    end = base + len(fd)
    inside = lambda p: base <= p < end
    objs = [o for o in read_objs(st) if inside(o["comp"])]
    stills = [s for s in read_stills(st) if inside(s["comp"])]
    w("(c) %d objects, %d stills use this buffer" % (len(objs), len(stills)))
    scene_offs = {s["offset"]: s for s in a.scenes}
    struct_ok = True
    for o in objs + stills:
        kind = "obj" if o in objs else "still"
        sc = scene_offs.get(o["scene"] - base)
        sid = o["scene_id"] & 0xFFFF
        msgs = []
        if sc is None:
            msgs.append("scene pointer %08x is not a scene start" % o["scene"])
        elif sc["index"] != sid:
            msgs.append("scene id %x but pointer is scene %d" % (o["scene_id"], sc["index"]))
        co = o["comp"] - base
        if co not in a.comps:
            msgs.append("composition %08x is not a composition start" % o["comp"])
        elif sc and co != sc["comps"][-1]["offset"]:
            msgs.append("composition is not the scene's main one (aet_main_comp_get)")
        lo = o["layer"] - base if o["layer"] else None
        if lo is not None and lo not in a.layers:
            msgs.append("layer %08x is not a layer start" % o["layer"])
        nm = program_string(o["layer_name"]) if o["layer_name"] else None
        if lo is not None and lo in a.layers:
            L = a.layers[lo]
            if nm is not None and comp_layer_find(a, co, nm) != lo:
                msgs.append("the start block names %r, aet_comp_layer_find gives another layer" % nm)
            if kind == "obj" and (o["start0"] != ftrc(L["start_frame"]) or o["end0"] != ftrc(L["end_frame"])):
                msgs.append("start/end words %d/%d, layer frames %g/%g" % (o["start0"], o["end0"], L["start_frame"], L["end_frame"]))
        struct_ok &= not msgs
        if kind == "obj":
            w("    obj %2d handle %3d scene %d layer %s %r frame %d (start %d end %d flags %x)%s" % (
                o["index"], o["handle"], sid, "+0x%x" % lo if lo is not None else "-",
                a.layers[lo]["name"] if lo in a.layers else None, o["frame"], o["start"], o["end"], o["flags"],
                "" if not msgs else "  BAD: " + "; ".join(msgs)))
        else:
            w("    still %2d handle %3d scene %d layer %s %r flags %x counters %d/%d%s" % (
                o["index"], o["handle"], sid, "+0x%x" % lo if lo is not None else "-",
                a.layers[lo]["name"] if lo in a.layers else None, o["flags"], o["counter"], o["counter2"],
                "" if not msgs else "  BAD: " + "; ".join(msgs)))
    ok &= struct_ok
    # replay the draw pass with each float model; keep the best
    modes = fp_modes or [("nearest", "fused"), ("nearest", "split"), ("zero", "fused"), ("zero", "split")]
    # aet_cur_obj is the object aet_objs_draw drew last. When it is not the
    # last object in use the snapshot was taken inside the draw pass: the
    # objects after it still hold the previous task frame's draw (one step
    # back), the ones up to it this frame's.
    cur = u32(st, 0x0C4EFF88 - STATE_BASE)
    cut = (cur - OBJS) // OBJ_SIZE if OBJS <= cur < OBJS + 64 * OBJ_SIZE else None
    in_use = [o["index"] for o in read_objs(st)]
    mid = cut is not None and in_use and cut < max(in_use)
    w("    aet_cur_obj = %08x (object %s)%s" % (cur, cut, "; the snapshot is inside aet_objs_draw" if mid else
                                                    "; the last object in use: the draw pass had finished"))
    whiches = ["drawn"] + (["inside"] if mid else []) + ["back"]
    labels = {"drawn": "as the frame word",
              "inside": "the previous task frame's pass, then this frame's up to part way into object %s" % cut,
              "back": "one step back"}

    def enc(tv):
        t, v = tv
        return struct.pack("<f", v) if t == "f" else struct.pack("<i", v) if t == "i" else bytes([v])

    def draw_objs(sim, objs_, back, upto=None):
        for o in objs_:
            if upto is not None and o["index"] > upto:
                break
            if o["flags"] & 0x20 or o["hide"] & 0x40:
                continue
            f = frame_one_step_back(o) if back else frame_of_last_draw(o)
            sim.who = "obj %d frame %d" % (o["index"], f)
            sim.obj_draw(o["comp"] - base, (o["layer"] - base) if o["layer"] else None, f)

    def draw_stills(sim, building):
        for s in stills:
            if bool(s["flags"] & 1) == building and not s["flags"] & 0x40:
                sim.who = "still %d" % s["index"]
                sim.still_build(s["comp"] - base, (s["layer"] - base) if s["layer"] else None, s["counter"])

    best, tally, inside_range = None, [], None
    for mode in modes:
        for which in whiches:
            fp = F32(*mode)
            sim = Sim(a, base, fp)
            # stills built before (build flag clear) first, then the objects
            # in record order, then stills still building (they draw after)
            draw_stills(sim, False)
            if which == "inside":
                draw_objs(sim, objs, True)
                draw_stills(sim, True)
                start = len(sim.log)
                draw_objs(sim, objs, False, cut)
                # the cut object's draw stopped after some prefix of its writes
                first = next((i for i in range(start, len(sim.log)) if sim.log[i][2].startswith("obj %d " % cut)), len(sim.log))
                final = {}
                for o_, v_, who_ in sim.log[:first]:
                    final[o_] = (v_, who_)
                nbad = sum(1 for o_, (v_, _) in final.items() if md[o_:o_ + len(enc(v_))] != enc(v_))
                fits, bestn = [], (nbad, 0)
                if nbad == 0:
                    fits.append(0)
                for i in range(first, len(sim.log)):
                    o_, v_, who_ = sim.log[i]
                    if o_ in final:
                        nbad -= md[o_:o_ + len(enc(final[o_][0]))] != enc(final[o_][0])
                    final[o_] = (v_, who_)
                    nbad += md[o_:o_ + len(enc(v_))] != enc(v_)
                    if nbad == 0:
                        fits.append(i + 1 - first)
                    bestn = min(bestn, (nbad, i + 1 - first))
                n = fits[-1] if fits else bestn[1]
                sim.writes = {}
                for o_, v_, who_ in sim.log[:first + n]:
                    sim.writes[o_] = (v_, who_)
                if mode == modes[0] or fits:
                    inside_range = (fits[0], fits[-1], len(sim.log) - first) if fits else None
            else:
                draw_objs(sim, objs, which == "back")
                draw_stills(sim, True)
            good, wrong = 0, []
            for off, (tv, who) in sim.writes.items():
                exp = enc(tv)
                got = md[off:off + len(exp)]
                if exp == got:
                    good += 1
                else:
                    wrong.append((off, tv[0], tv[1], got, who))
            tally.append((mode, which, good, len(wrong)))
            key = (len(wrong), modes.index(mode), whiches.index(which))
            if best is None or key < best[0]:
                best = (key, mode, which, sim, good, wrong)
    _, mode, which, sim, good, wrong = best
    w("    replay (RM %s, fmac %s, frames %s): %d run-time words written, %d equal the live image bit for bit, %d differ"
      % (mode[0], mode[1], labels[which], len(sim.writes), good, len(wrong)))
    if which == "inside" and inside_range:
        w("      object %d's draw makes %d writes; the live image fits a stop after write %d .. %d of them"
          % (cut, inside_range[2], inside_range[0], inside_range[1]))
    if len(tally) > 1:
        w("      all models: " + "; ".join("%s/%s/%s %d" % (m[0], m[1], wh, nw) for m, wh, g, nw in tally)
          + "  (rounding / fmac / frames: words that differ)")
    for off, t, v, got, who in wrong[:12]:
        gv = struct.unpack("<f", got)[0] if t == "f" else got.hex()
        w("      +0x%x %s: ours %r live %r (%s)" % (off, t, v, gv, who))
    # per object: does the other frame choice also fit? (report the evidence)
    alt = {}
    for o in objs:
        for which2, f in (("drawn", frame_of_last_draw(o)), ("back", frame_one_step_back(o))):
            fp = F32(*mode)
            s2 = Sim(a, base, fp)
            s2.who = "x"
            s2.obj_draw(o["comp"] - base, (o["layer"] - base) if o["layer"] else None, f)
            eq = sum(1 for off, ((t, v), _) in s2.writes.items() if t == "f" and struct.pack("<f", v) == md[off:off + 4])
            nf = sum(1 for (t, _), _ in s2.writes.values() if t == "f")
            alt.setdefault(o["index"], []).append((which2, f, eq, nf))
    for i, v in alt.items():
        w("      obj %2d: %s" % (i, "; ".join("frame %g: %d/%d floats" % (f, eq, nf) for _, f, eq, nf in v)))
    # layers whose live floats differ from the file but no current draw wrote
    unexpl = []
    for lo, L in a.layers.items():
        live = [md[lo + 0x14 + 4 * k:lo + 0x18 + 4 * k] for k in range(8)]
        if live != [fd[lo + 0x14 + 4 * k:lo + 0x18 + 4 * k] for k in range(8)] and (lo + 0x14) not in sim.writes:
            unexpl.append((lo, L, live))
    w("    layers holding floats no current object or still wrote (left by earlier draws): %d" % len(unexpl))
    fp = F32(*mode)
    found = 0
    for lo, L, live in unexpl:
        c = a.comps[L["comp"]]
        hits = layer_search(fp, L, live, (min(c["start_frame"], L["start_frame"]) - 2, max(c["end_frame"], L["end_frame"]) + 2))
        found += bool(hits)
        if verbose and not hits:
            w("      +0x%x %r: frames that reproduce them: %s" % (lo, L["name"], ", ".join("%g" % h for h in hits[:8]) + (" ..." if len(hits) > 8 else "") if hits else "none in range"))
    if unexpl:
        w("      %d of %d reproduce at some whole or half frame" % (found, len(unexpl)))
    res.update({"c_struct_ok": struct_ok, "replay_written": len(sim.writes), "replay_equal": good,
                "replay_wrong": len(wrong), "mode": mode, "frames": which, "objects": len(objs),
                "stills": len(stills), "left_over_layers": len(unexpl), "left_over_found": found})
    ok &= not wrong
    return ok, res


def checkdir(dump_dir, asset_dir, fp_modes, out=sys.stdout):
    rows = [l.rstrip("\n").split("\t") for l in open(os.path.join(dump_dir, "index.tsv"))][1:]
    state = os.path.join(dump_dir, "aet_state_%08x.bin" % STATE_BASE)
    state = state if os.path.exists(state) else None
    allok, results = True, []
    for r in rows:
        s, name, buf = int(r[0]), r[1], int(r[4], 16)
        if not buf:
            continue
        mem = os.path.join(dump_dir, "%02d_%s.mem" % (s, name))
        ok, res = check(os.path.join(asset_dir, name), mem, buf, state, fp_modes, out)
        allok &= ok
        results.append(res)
        out.write("   -> %s\n" % ("PASS" if ok else "FAIL"))
    return allok, results


def main(argv):
    if not argv or argv[0] in ("-h", "--help"):
        print(__doc__)
        return 0
    opt = lambda k, d: argv[argv.index(k) + 1] if k in argv else d
    rnd, fm = opt("--round", None), opt("--fmac", None)
    pos = [x for i, x in enumerate(argv) if not x.startswith("--") and (i == 0 or argv[i - 1] not in ("--round", "--fmac", "--json", "--header"))]
    modes = [(rnd or "nearest", fm or "fused")] if (rnd or fm) else None
    if pos[0] == "all":
        d, outd = pos[1], pos[2]
        os.makedirs(outd, exist_ok=True)
        files = sorted(glob.glob(os.path.join(d, "*_aet.bin")))
        tot = {}
        print("%-26s %7s %6s %5s %6s %6s %6s %6s %6s %7s %5s" % ("file", "bytes", "scenes", "comps", "layers", "images",
                                                               "pics", "tracks", "keys", "markers", "gap"))
        for p in files:
            a = Aet(open(p, "rb").read(), os.path.basename(p))
            c = a.counts()
            gap = sum(s for _, s, _ in a.real_gaps())
            with open(os.path.join(outd, os.path.basename(p)[:-4] + ".json"), "w") as f:
                json.dump(a.to_json(), f, indent=1)
            for k, v in c.items():
                tot[k] = tot.get(k, 0) + v
            tot["bytes"] = tot.get("bytes", 0) + len(a.d)
            tot["gap"] = tot.get("gap", 0) + gap
            print("%-26s %7d %6d %5d %6d %6d %6d %6d %6d %7d %5d" % (a.name, len(a.d), c["scenes"], c["comps"], c["layers"],
                  c["images"], c["pictures"], c["tracks_with_keys"], c["keys"], c["markers"], gap))
            for n in a.notes:
                print("    note: " + n)
        print("%-26s %7d %6d %5d %6d %6d %6d %6d %6d %7d %5d" % ("total (%d files)" % len(files), tot["bytes"], tot["scenes"],
              tot["comps"], tot["layers"], tot["images"], tot["pictures"], tot["tracks_with_keys"], tot["keys"],
              tot["markers"], tot["gap"]))
        return 0
    if pos[0] == "check":
        ok, _ = check(pos[1], pos[2], int(pos[3], 16), pos[4] if len(pos) > 4 else None, modes)
        print("PASS" if ok else "FAIL")
        return 0 if ok else 1
    if pos[0] == "checkdir":
        ok, _ = checkdir(pos[1], pos[2], modes)
        print("PASS" if ok else "FAIL")
        return 0 if ok else 1
    if pos[0] == "eval":
        a = Aet(open(pos[1], "rb").read(), os.path.basename(pos[1]))
        L = a.scenes[int(pos[2])]["comps"][int(pos[3])]["layers"][int(pos[4])]
        vis, vals = layer_tracks_eval(F32(rnd or "nearest", fm or "fused"), L, float(pos[5]))
        print("%r visible %d" % (L["name"], vis))
        for n, v in zip(TRACK_NAMES, vals):
            print("  %-8s %r (%08x)" % (n, v, u32(struct.pack("<f", v), 0)))
        return 0
    hdr = opt("--header", None)
    hdr = [int(x, 16) for x in hdr.split(",")] if hdr else None
    a = Aet(open(pos[0], "rb").read(), os.path.basename(pos[0]), hdr)
    dump(a)
    if "--json" in argv:
        with open(opt("--json", None), "w") as f:
            json.dump(a.to_json(), f, indent=1)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
