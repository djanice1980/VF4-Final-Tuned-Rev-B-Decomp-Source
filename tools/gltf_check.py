#!/usr/bin/env python3
"""Check a .glb from tools/gltf.py: structure, and skinning against pose.py.

Reads only the .glb (no game data):
  structure  every bufferView inside the buffer, every accessor inside its
             view and aligned, POSITION min / max right, indices in range,
             joints in range and weights summing to 1, unit rotations,
             animation times increasing with min / max, the node tree
             acyclic, images PNG
  skinning   with --pose ANIMATION FRAME [OBJ]: evaluates the animation at
             frame / 30 s, skins every vertex (sum(weight x joint global x
             inverse bind) x position, the glTF rule; vertices whose joints
             are scaled to 0, the face the animation hides, are left out)
             and, given pose.py's
             OBJ of the same frame, reports the distance from each skinned
             vertex to the nearest OBJ vertex

usage: gltf_check.py <file.glb> [--pose ANIMATION FRAME [pose.obj]]
"""
import json
import math
import struct
import sys

import numpy as np

SIZE = {5120: 1, 5121: 1, 5122: 2, 5123: 2, 5125: 4, 5126: 4}
DT = {5121: "u1", 5123: "<u2", 5125: "<u4", 5126: "<f4"}
NCOMP = {"SCALAR": 1, "VEC2": 2, "VEC3": 3, "VEC4": 4, "MAT4": 16}


def load(path):
    d = open(path, "rb").read()
    magic, ver, length = struct.unpack_from("<III", d, 0)
    assert magic == 0x46546C67 and ver == 2 and length == len(d), "bad header"
    jl, jt = struct.unpack_from("<II", d, 12)
    assert jt == 0x4E4F534A
    j = json.loads(d[20:20 + jl])
    bl, bt = struct.unpack_from("<II", d, 20 + jl)
    assert bt == 0x004E4942
    return j, d[28 + jl:28 + jl + bl]


def accessor(j, b, i):
    a = j["accessors"][i]
    v = j["bufferViews"][a["bufferView"]]
    n = NCOMP[a["type"]]
    off = v.get("byteOffset", 0) + a.get("byteOffset", 0)
    arr = np.frombuffer(b, dtype=DT[a["componentType"]], count=a["count"] * n, offset=off)
    return arr.reshape(a["count"], n) if n > 1 else arr


def structure(j, b):
    errs = []
    for k, v in enumerate(j["bufferViews"]):
        if v.get("byteOffset", 0) + v["byteLength"] > len(b):
            errs.append(f"bufferView {k} past the buffer")
    for k, a in enumerate(j["accessors"]):
        v = j["bufferViews"][a["bufferView"]]
        size = SIZE[a["componentType"]]
        need = a.get("byteOffset", 0) + a["count"] * NCOMP[a["type"]] * size
        if need > v["byteLength"]:
            errs.append(f"accessor {k} past its view")
        if (v.get("byteOffset", 0) + a.get("byteOffset", 0)) % size:
            errs.append(f"accessor {k} misaligned")
    for mi, m in enumerate(j["meshes"]):
        for pi, p in enumerate(m["primitives"]):
            pos = accessor(j, b, p["attributes"]["POSITION"])
            a = j["accessors"][p["attributes"]["POSITION"]]
            if not (np.allclose(pos.min(0), a["min"], atol=1e-5) and np.allclose(pos.max(0), a["max"], atol=1e-5)):
                errs.append(f"mesh {mi} prim {pi}: POSITION min/max wrong")
            idx = accessor(j, b, p["indices"])
            if idx.max() >= len(pos) or len(idx) % 3:
                errs.append(f"mesh {mi} prim {pi}: indices out of range or not triangles")
            for key in p["attributes"]:
                if len(accessor(j, b, p["attributes"][key])) != len(pos):
                    errs.append(f"mesh {mi} prim {pi}: {key} count differs from POSITION")
            jn = accessor(j, b, p["attributes"]["JOINTS_0"])
            wt = accessor(j, b, p["attributes"]["WEIGHTS_0"])
            skin = next(n["skin"] for n in j["nodes"] if n.get("mesh") == mi)
            if jn.max() >= len(j["skins"][skin]["joints"]):
                errs.append(f"mesh {mi}: joint index out of range")
            if (wt < 0).any() or not np.allclose(wt.sum(1), 1, atol=1e-5):
                errs.append(f"mesh {mi} prim {pi}: weights negative or not summing to 1")
            nrm = accessor(j, b, p["attributes"]["NORMAL"])
            if not np.allclose(np.linalg.norm(nrm, axis=1), 1, atol=1e-3):
                errs.append(f"mesh {mi} prim {pi}: normals not unit")
    for n in j["nodes"]:
        if "rotation" in n and abs(np.linalg.norm(n["rotation"]) - 1) > 1e-5:
            errs.append(f"node {n.get('name')}: rotation not unit")
    for an in j["animations"]:
        for s in an["samplers"]:
            t = accessor(j, b, s["input"])
            a = j["accessors"][s["input"]]
            if (np.diff(t) <= 0).any() or "min" not in a or "max" not in a:
                errs.append(f"{an['name']}: times not increasing or no min/max")
        for c in an["channels"]:
            if c["target"]["path"] == "rotation":
                q = accessor(j, b, an["samplers"][c["sampler"]]["output"])
                if not np.allclose(np.linalg.norm(q, axis=1), 1, atol=1e-5):
                    errs.append(f"{an['name']}: rotation not unit")
                    break
    seen = set()

    def walk(k, path):
        if k in path:
            errs.append(f"node {k}: cycle")
            return
        seen.add(k)
        for c in j["nodes"][k].get("children", []):
            walk(c, path | {k})
    for r in j["scenes"][0]["nodes"]:
        walk(r, set())
    if len(seen) != len(j["nodes"]):
        errs.append(f"{len(j['nodes']) - len(seen)} nodes not in the scene")
    for im in j.get("images", []):
        v = j["bufferViews"][im["bufferView"]]
        if b[v.get("byteOffset", 0):v.get("byteOffset", 0) + 8] != b"\x89PNG\r\n\x1a\n":
            errs.append(f"image {im.get('name')}: not PNG")
    return errs


def qmat(q):
    x, y, z, w = q
    return np.array([[1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w)],
                     [2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w)],
                     [2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y)]])


hidden = [0]


def skinned(j, b, anim, frame):
    t = frame / 30
    local = []
    for n in j["nodes"]:
        local.append({"t": np.array(n.get("translation", [0, 0, 0]), float),
                      "r": np.array(n.get("rotation", [0, 0, 0, 1]), float),
                      "s": np.array(n.get("scale", [1, 1, 1]), float)})
    an = next(a for a in j["animations"] if a["name"] == anim or a["name"].endswith("_" + anim))
    for c in an["channels"]:
        s = an["samplers"][c["sampler"]]
        times = accessor(j, b, s["input"])
        vals = accessor(j, b, s["output"]).reshape(len(times), -1)
        k = int(np.searchsorted(times, t - 1e-6))
        if k >= len(times):
            v = vals[-1]
        elif k == 0 or abs(times[k] - t) < 1e-6:
            v = vals[min(k, len(vals) - 1)]
        else:
            u = (t - times[k - 1]) / (times[k] - times[k - 1])
            v = vals[k - 1] * (1 - u) + vals[k] * u
        if s.get("interpolation") == "STEP" and k > 0 and not abs(times[min(k, len(times) - 1)] - t) < 1e-6:
            v = vals[min(k, len(vals)) - 1]
        local[c["target"]["node"]][{"translation": "t", "rotation": "r", "scale": "s"}[c["target"]["path"]]] = v
    glob = [None] * len(j["nodes"])

    def walk(k, parent):
        m = np.eye(4)
        m[:3, :3] = qmat(local[k]["r"] / np.linalg.norm(local[k]["r"])) * local[k]["s"]
        m[:3, 3] = local[k]["t"]
        glob[k] = parent @ m
        for c in j["nodes"][k].get("children", []):
            walk(c, glob[k])
    for r in j["scenes"][0]["nodes"]:
        walk(r, np.eye(4))
    out = []
    for n in j["nodes"]:
        if "mesh" not in n:
            continue
        skin = j["skins"][n["skin"]]
        ibm = accessor(j, b, skin["inverseBindMatrices"]).reshape(-1, 4, 4).transpose(0, 2, 1)
        jm = np.array([glob[k] @ ibm[i] for i, k in enumerate(skin["joints"])])
        for p in j["meshes"][n["mesh"]]["primitives"]:
            pos = accessor(j, b, p["attributes"]["POSITION"]).astype(float)
            jn = accessor(j, b, p["attributes"]["JOINTS_0"]).astype(int)
            wt = accessor(j, b, p["attributes"]["WEIGHTS_0"]).astype(float)
            ph = np.c_[pos, np.ones(len(pos))]
            acc = np.zeros((len(pos), 4))
            M = np.zeros((len(pos), 3, 3))
            for c in range(4):
                acc += wt[:, c:c + 1] * np.einsum("nij,nj->ni", jm[jn[:, c]], ph)
                M += wt[:, c, None, None] * jm[jn[:, c], :3, :3]
            shown = np.abs(np.linalg.det(M)) > 1e-12  # a joint scaled to 0 hides its vertices
            out.append(acc[shown, :3])
            hidden[0] += int((~shown).sum())
    return np.concatenate(out)


def main():
    j, b = load(sys.argv[1])
    errs = structure(j, b)
    print(f"{sys.argv[1]}: {len(j['nodes'])} nodes, {len(j['meshes'])} meshes, "
          f"{sum(len(m['primitives']) for m in j['meshes'])} primitives, {len(j.get('animations', []))} animations, "
          f"{len(j.get('images', []))} images; structure: {'OK' if not errs else str(len(errs)) + ' problems'}")
    for e in errs[:20]:
        print("  ", e)
    if "--pose" in sys.argv:
        k = sys.argv.index("--pose")
        anim, frame = sys.argv[k + 1], int(sys.argv[k + 2])
        pts = skinned(j, b, anim, frame)
        print(f"skinned {len(pts)} vertices at {anim} frame {frame} ({hidden[0]} hidden by a 0 scale)")
        if len(sys.argv) > k + 3:
            ref = np.array([[float(x) for x in ln.split()[1:4]] for ln in open(sys.argv[k + 3]) if ln.startswith("v ")])
            worst = 0.0
            dists = []
            for c in range(0, len(pts), 2000):
                chunk = pts[c:c + 2000]
                d = np.sqrt(((chunk[:, None, :] - ref[None, :, :]) ** 2).sum(-1)).min(1)
                dists.append(d)
            d = np.concatenate(dists)
            print(f"distance to the nearest pose.py vertex: max {d.max():.2e}, mean {d.mean():.2e}, "
                  f"over 1e-4: {(d > 1e-4).sum()} of {len(d)}")
    sys.exit(1 if errs else 0)


if __name__ == "__main__":
    main()
