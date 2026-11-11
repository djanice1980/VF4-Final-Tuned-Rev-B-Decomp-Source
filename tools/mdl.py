#!/usr/bin/env python3
"""Export the obj_/mdl_ model sets to Wavefront OBJ (+ MTL and a node tree).

Format (measured on all 215 sets; loader libpoly load_poly.c 0x0C0E8540,
pol_uti.c 0x0C0E7120; see docs/asset-formats.md):
  obj_<set>.bin  header u32 offsets, made pointers by the loader:
                   [0] zero-ended list of nodes, [1] parallel list of
                   name strings, [2] texture descriptors, [3] optional list
                 node: +0x04 position, +0x10 rotation (radians), +0x1C scale
                   (3 floats each), +0x2C first child, +0x30 next sibling,
                   +0x34 parent, +0x3C type (0x13/0x15 skinned bodies, with
                   0x7B/0xA2 bone pointers at +0x88; 0x12 envelopes)
  mdl_<set>.bin  u32 offset per node (index = node index; 0 = none), to
                 either { -1, n } (no geometry) or a model block:
                   +0x00 0x100, +0x08 bounding sphere x, y, z, r,
                   +0x18 block size, +0x40 / +0x48 / +0x50 / +0x58 four
                   (offset from that word, size) slots of Naomi 2 Elan
                   command lists, laid out one after another from +0x60
                 The list holds only GMP material records (PCW command 5,
                 64 bytes, tex0 at +0x24 = index into the obj_ texture
                 descriptors) and ICH vertex lists (command 7, 32 bytes +
                 vertices; vertex flags 0x02 V, 0x0A VU, 0x4A VUR). These
                 are the structures Flycast's elan_struct.h describes, and
                 all 14,315 blocks walk to their exact end with them.
  Vertices: { u32 header (s8 nx, ny, nz; bit 29 strip, 30 fan, 31 end of
  strip), float x, y, z } then u, v (VU, VUR) and two ARGB words (VUR).
  A strip ends at the end-of-strip bit; a fan vertex (strip 0, fan 1) makes
  a triangle with the strip's first vertex and the previous one, else with
  the previous two (as Flycast's sendVertices). strip 1 / fan 1 marks a
  strip's third vertex, not a fan; every character and stage strip has it
  and none has a real fan vertex. Texture v counts from the first stored row,
  which is the bottom of the decoded PNG (pvr_tex.py flips), so it is
  already OBJ's v.

Geometry is written as stored, without node transforms: stage pieces carry
identity transforms and world-space vertices, and a character's pieces share
the body's bounding sphere (bind pose in body space).

usage: mdl.py <unpacked asset dir> <texture dir> <out dir> [set ...]
"""
import json
import math
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import pvr_tex  # noqa: E402

VTX = {0x02: 16, 0x0A: 24, 0x4A: 32, 0x0E: 40, 0x42: 24, 0x10A: 40}  # V, VU, VUR, VNU, VR, VUB (elan_struct.h)
LIST = {0: "opaque", 1: "opaque_mod", 2: "translucent", 3: "translucent_mod", 4: "punch_through"}


def u32(d, o):
    return struct.unpack_from("<I", d, o)[0]


def zlist(d, o):
    out = []
    while u32(d, o):
        out.append(u32(d, o))
        o += 4
    return out


def nodes_of(obj):
    nodes = zlist(obj, u32(obj, 0))
    names = zlist(obj, u32(obj, 4))
    idx = {n: i for i, n in enumerate(nodes)}
    out = []
    for i, n in enumerate(nodes):
        f = struct.unpack_from("<9f", obj, n + 4)
        name = obj[names[i]:obj.index(b"\0", names[i])].decode("latin1") if i < len(names) else f"node{i}"
        out.append({"index": i, "name": name, "type": u32(obj, n + 0x3C),
                    "position": [round(v, 6) for v in f[0:3]], "rotation": [round(v, 6) for v in f[3:6]],
                    "scale": [round(v, 6) for v in f[6:9]],
                    "parent": idx.get(u32(obj, n + 0x34)), "child": idx.get(u32(obj, n + 0x2C)),
                    "sibling": idx.get(u32(obj, n + 0x30))})
    return out


def model(m, b, with_tsp=False):
    """Model block at b -> list of (material, list_type, vertices, triangles),
    plus, when with_tsp, a dict of the list's ISP word (cull mode, bits
    27-28) and TSP word (texture clamp / flip bits) and the active GMP's
    paramSelect, diffuse0 and specular0 words, and the list slot (0-3).

    The header holds four (offset, size) list slots at +0x40, +0x48, +0x50,
    +0x58, each offset counted from its own slot; the lists follow one
    another from +0x60. A GMP carries over from one slot to the next."""
    tex, out, gmp = None, [], (0, 0xFFFFFFFF, 0)
    for slot in range(4):
        rel, size = u32(m, b + 0x40 + 8 * slot), u32(m, b + 0x44 + 8 * slot)
        if size:
            tex, gmp = _walk(m, b, b + 0x40 + 8 * slot + rel, size, with_tsp, slot, tex, gmp, out)
    return out


def _walk(m, b, p, size, with_tsp, slot, tex, gmp, out):
    end = p + size
    while p < end:
        pcw = u32(m, p)
        cmd = pcw >> 8 & 0xF
        if cmd == 5:  # GMP
            t = u32(m, p + 0x24)
            tex = None if t == 0xFFFFFFFF else t
            gmp = (u32(m, p + 8), u32(m, p + 0xC), u32(m, p + 0x10))
            p += 64
        elif cmd == 7:  # ICH
            flags, n = struct.unpack_from("<II", m, p + 24)
            size = VTX[flags]
            verts = []
            for k in range(n):
                v = p + 32 + size * k
                hdr = u32(m, v)
                x, y, z = struct.unpack_from("<3f", m, v + 4)
                nrm = [b - 256 if b > 127 else b for b in m[v:v + 3]]
                uv = (struct.unpack_from("<2f", m, v + 32) if flags == 0x0E else
                      struct.unpack_from("<2f", m, v + 16) if flags in (0x0A, 0x4A, 0x10A) else None)
                rgb = u32(m, v + 24) if flags == 0x4A else u32(m, v + 16) if flags == 0x42 else None
                # header bits 29 strip, 30 fan, 31 end of strip: strip=0/fan=0 the
                # first two of a strip, strip=1/fan=1 the third, strip=0/fan=1 a fan
                # vertex (elan_struct.h HeaderAndNormal::isFan)
                verts.append({"p": (x, y, z), "n": [c / 127 for c in nrm], "uv": uv, "rgb": rgb,
                              "fan": int(hdr >> 30 & 1 and not hdr >> 29 & 1), "end": hdr >> 31 & 1,
                              "offset": v - b})
            item = (tex if flags not in (0x02, 0x42) else None, LIST.get(pcw >> 24 & 7, "other"), verts, triangles(verts))
            info = {"isp": u32(m, p + 4), "tsp": u32(m, p + 8), "select": gmp[0], "diffuse": gmp[1],
                    "specular": gmp[2], "slot": slot}
            out.append(item + (info,) if with_tsp else item)
            p += 32 + size * n
        else:
            raise ValueError(f"Elan command {cmd} at {p:#x}")
    if p != end:
        raise ValueError(f"list slot {slot} ends at {p - b:#x}, not {end - b:#x}")
    return tex, gmp


def triangles(verts):
    """Strips and fans as Flycast's sendVertices emits them, as one TA strip."""
    seq, start, centre = [], True, 0
    for i, v in enumerate(verts):
        if start:
            centre = i
            if seq:  # link strips with degenerate triangles
                seq += [seq[-1], i]
                if len(seq) & 1:
                    seq.append(i)
            start = False
        elif v["fan"]:
            last = seq[-1]
            seq += [last, centre]
            if len(seq) & 1:
                seq.append(centre)
            seq += [centre, last]
        seq.append(i)
        if v["end"]:
            start = True
    tris = []
    for k in range(len(seq) - 2):
        a, b, c = seq[k], seq[k + 1], seq[k + 2]
        if a != b and b != c and a != c:
            tris.append((a, b, c) if k % 2 == 0 else (b, a, c))
    return tris


def export(src, texdir, dest, s):
    obj = open(os.path.join(src, f"obj_{s}.bin"), "rb").read()
    m = open(os.path.join(src, f"mdl_{s}.bin"), "rb").read()
    nodes = nodes_of(obj)
    descs = pvr_tex.descriptors(os.path.join(src, f"obj_{s}.bin"))
    texnames = [f"{i:03d}_{w}x{h}_{pvr_tex.PIXEL[f & 0xFF]}_{pvr_tex.TYPE[f & 0xF00]}.png"
                for i, (w, h, f, off, x) in enumerate(descs)]
    os.makedirs(dest, exist_ok=True)
    rel = os.path.relpath(os.path.join(texdir, s), dest)
    lines, mats, base, stats = [f"# {s}: exported by tools/mdl.py", f"mtllib {s}.mtl"], {}, 1, [0, 0, 0]
    for nd in nodes:
        i = nd["index"]
        b = u32(m, 4 * i) if 4 * i < u32(m, 0) else 0
        nd["geometry"] = False
        if not b or u32(m, b) == 0xFFFFFFFF:
            continue
        if u32(m, b) != 0x100:
            raise ValueError(f"{s} node {i}: block magic {u32(m, b):#x}")
        nd["geometry"] = True
        nd["bounds"] = [round(v, 6) for v in struct.unpack_from("<4f", m, b + 8)]
        lines.append(f"o {nd['name']}")
        for tex, kind, verts, tris in model(m, b):
            mat = f"tex{tex:03d}_{kind}" if tex is not None else f"untextured_{kind}"
            mats[mat] = (tex, kind)
            for v in verts:
                x, y, z = v["p"]
                if v["rgb"] is not None:
                    c = v["rgb"]
                    lines.append(f"v {x:.6f} {y:.6f} {z:.6f} {(c >> 16 & 255) / 255:.4f} "
                                 f"{(c >> 8 & 255) / 255:.4f} {(c & 255) / 255:.4f}")
                else:
                    lines.append(f"v {x:.6f} {y:.6f} {z:.6f}")
                uv = v["uv"] or (0.0, 0.0)
                if not all(math.isfinite(c) for c in uv):  # 48 vertices store v = NaN (0x7FC00000)
                    uv = tuple(c if math.isfinite(c) else 0.0 for c in uv)
                    stats[2] += 1
                lines.append(f"vt {uv[0]:.6f} {uv[1]:.6f}")
                lines.append("vn {:.4f} {:.4f} {:.4f}".format(*v["n"]))
            lines.append(f"usemtl {mat}")
            for t in tris:
                lines.append("f " + " ".join(f"{base + k}/{base + k}/{base + k}" for k in t))
            base += len(verts)
            stats[0] += len(verts)
            stats[1] += len(tris)
    with open(os.path.join(dest, f"{s}.obj"), "w") as f:
        f.write("\n".join(lines) + "\n")
    with open(os.path.join(dest, f"{s}.mtl"), "w") as f:
        for mat, (tex, kind) in sorted(mats.items()):
            f.write(f"newmtl {mat}\nKd 1 1 1\n")
            if tex is not None and tex < len(texnames):
                f.write(f"map_Kd {rel}/{texnames[tex]}\n")
                if kind in ("translucent", "punch_through"):
                    f.write(f"map_d {rel}/{texnames[tex]}\n")
            f.write("\n")
    with open(os.path.join(dest, f"{s}.json"), "w") as f:
        json.dump({"set": s, "nodes": nodes}, f, indent=1)
    return stats


def main():
    src, texdir, dest = sys.argv[1], sys.argv[2], sys.argv[3]
    sets = sys.argv[4:] or sorted(n[4:-4] for n in os.listdir(src) if n.startswith("mdl_") and n.endswith(".bin")
                                  and os.path.exists(os.path.join(src, f"obj_{n[4:-4]}.bin")))
    total = [0, 0, 0, 0]
    for s in sets:
        if os.path.getsize(os.path.join(src, f"mdl_{s}.bin")) < 8:
            continue
        v, t, nan = export(src, texdir, dest, s)
        total = [total[0] + 1, total[1] + v, total[2] + t, total[3] + nan]
    print(f"{total[0]} model sets written to {dest}: {total[1]:,} vertices, {total[2]:,} triangles; "
          f"{total[3]} NaN UVs written as 0", file=sys.stderr)


if __name__ == "__main__":
    main()
