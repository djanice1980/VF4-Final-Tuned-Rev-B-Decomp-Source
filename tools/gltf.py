#!/usr/bin/env python3
"""Export a character with its motions as one binary glTF 2.0 file (.glb).

Built from the decoded data (tools/mdl.py geometry, tools/motion.py curves,
tools/rig.py skeleton and skin, tools/pose.py's Character binding):
  joints      the 47 bones of the character's rig, in the rig's tree
  rigid skin  each rigid piece is skinned 1:1 to its bone; its vertices are
              placed in the rest pose and the inverse bind matrix is the
              inverse of that bone's rest matrix
  envelopes   one skin each, with the game's own bind matrices as inverse
              bind matrices and the skin record's weights, so a viewer
              computes sum(weight x bone x bind) x vertex as the game does
              (envelope_skin, 0x0C0E8200)
  animations  one per motion, baked: the game solves arms, legs, spine and
              neck by IK every frame (rig.pose emulates rob_eval_ik), which
              glTF cannot express, so each frame's solved local transforms
              are keyed at frame / 30 s. Constant channels get one key.
  cloth       type 0x0F cloth (headband tails, cloth panels; tools/physics.py
              Cloth) is simulated alongside: each particle is a root joint at
              the particle, rotated by the shortest arc from its rest normal
              to its current one, and each cloth vertex follows its particle
  swing       the ex_* swing bones (belt tails, sash, headband knot; see
              tools/physics.py) are simulated at 60 Hz from each motion's
              first frame and exported as extra root joints keyed with their
              world matrix; envelope weights and rigid pieces on those slots
              use them
  materials   one per texture and list type: punch-through lists MASK,
              translucent BLEND; the TSP clamp / flip bits give the sampler
              wrap (CLAMP_TO_EDGE / MIRRORED_REPEAT / REPEAT); single or
              double sided by the ISP cull mode (below).
              Base color as the Elan picks it (Flycast setModelColors): when
              the GMP's paramSelect bit 0 (d0) is set the GMP diffuse color
              (baseColorFactor), else the vertex color of VUR lists
              (COLOR_0). Every character list has d0 set with white diffuse;
              the VUR colors in the envelopes are 0 and unused. The GMP
              specular / offset color is not exported.
Winding: glTF front faces are counter-clockwise. Measured on every list, the
strip winding (tools/mdl.py, as Flycast emits it) opposes the vertex normals
exactly in the ISP cull mode 2 lists and agrees in the cull mode 3 lists, so
cull 2 lists are flipped, cull 2 / 3 lists stay single-sided (the hardware
culls them), and cull 0 / 1 lists (no orientation culling) are oriented per
triangle by their normals and double-sided.
Texture v: the hardware's v = 0 is the first stored row, which is the bottom
of the PNG (pvr_tex.py flips rows), and glTF's v = 0 is the top: v' = 1 - v.
  face        under the head joint: a "face" node parenting the face rig
              joints (face_*) and carrying the static vertices, a
              "face_rigid" node with the rigid neutral face, and the eye_*
              joints (tools/face.py); the skinned face uses the game's bind
              matrices. The game draws the skinned face only while the move's
              face motion plays, so each animation keys the other face's
              node scale to 0 (the defaults follow the rest motion)
  wrists      slots 8 / 13 (rig.wrist, the matrix rob_eval_output makes from
              each hand) are joints under the forearm, keyed per frame
pose.py's options apply to every animation (--from does not: it needs one
motion to blend from). Face skin groups with more than 4 bones keep the largest 4 (glTF viewers read one
JOINTS set), and the script lists each one.

usage: gltf.py <main.bin> <unpacked asset dir> <texture dir> <motion dir>
               <set> <out.glb> [--motions all|NAME,NAME,...] [--common]
               [--rest NAME] [--mirror] [--smooth] [--look X Y Z [FADE]]
               [--eyes X Y Z] [--moves DIR] [--wind STAGE [FRAME [HEADING]]]
               [--no-lift] [--reach C0 C1 C2 C3 C4 OUT]
  --motions  which of the character's motions (default all)
  --common   also the common bank (motdat_mask_CMN, 1029 motions)
  --rest     the motion whose frame 0 is the default pose (default: the
             character's *_SY_KAMAE* stance, else its first motion)
  the rest   as pose.py
"""
import json
import math
import os
import struct
import sys

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import face  # noqa: E402
import mdl  # noqa: E402
import pose  # noqa: E402
import pvr_tex  # noqa: E402
import rig  # noqa: E402

BANK = {"AKIRA": "AKI", "AOI": "AOI", "JACKY": "JAC", "JEFFRY": "JEF", "KAGE": "KAG", "LAU": "LAU",
        "LION": "LIO", "PAI": "PAI", "SARAH": "SAR", "SHUN": "SHN", "WOLF": "WLF", "BOZE": "LEI",
        "BLACK": "VAN", "JDK": "JDK", "MUE": "MUE", "DURAL": "DUR",
        "MOT": "CMN"}  # character -> motion bank (mot_banks); MOT, the test character, has none: the common bank
FLOAT, U8, U16, U32 = 5126, 5121, 5123, 5125
REPEAT, MIRROR, CLAMP = 10497, 33648, 33071


def quat(r):
    """Rotation matrix -> unit quaternion (x, y, z, w)."""
    t = r[0, 0] + r[1, 1] + r[2, 2]
    if t > 0:
        s = math.sqrt(t + 1) * 2
        q = ((r[2, 1] - r[1, 2]) / s, (r[0, 2] - r[2, 0]) / s, (r[1, 0] - r[0, 1]) / s, s / 4)
    elif r[0, 0] > r[1, 1] and r[0, 0] > r[2, 2]:
        s = math.sqrt(1 + r[0, 0] - r[1, 1] - r[2, 2]) * 2
        q = (s / 4, (r[0, 1] + r[1, 0]) / s, (r[0, 2] + r[2, 0]) / s, (r[2, 1] - r[1, 2]) / s)
    elif r[1, 1] > r[2, 2]:
        s = math.sqrt(1 + r[1, 1] - r[0, 0] - r[2, 2]) * 2
        q = ((r[0, 1] + r[1, 0]) / s, s / 4, (r[1, 2] + r[2, 1]) / s, (r[0, 2] - r[2, 0]) / s)
    else:
        s = math.sqrt(1 + r[2, 2] - r[0, 0] - r[1, 1]) * 2
        q = ((r[0, 2] + r[2, 0]) / s, (r[1, 2] + r[2, 1]) / s, s / 4, (r[1, 0] - r[0, 1]) / s)
    q = np.array(q)
    return q / np.linalg.norm(q)


def arc(a, b):
    """Shortest-arc quaternion (x, y, z, w) turning unit vector a onto b."""
    a = a / max(np.linalg.norm(a), 1e-12)
    b = b / max(np.linalg.norm(b), 1e-12)
    d = float(a @ b)
    if d < -0.999999:
        axis = np.cross(a, [1, 0, 0]) if abs(a[0]) < 0.9 else np.cross(a, [0, 1, 0])
        q = np.array([*(axis / np.linalg.norm(axis)), 0.0])
    else:
        q = np.array([*np.cross(a, b), 1.0 + d])
    return q / np.linalg.norm(q)


def trs(m):
    """4x4 (rotation, translation, unit scale) -> translation, quaternion, scale."""
    t = m[:3, 3]
    sc = np.linalg.norm(m[:3, :3], axis=0)
    return t, quat(m[:3, :3] / sc), sc


class Writer:
    """Accumulates the JSON and the binary buffer of a .glb."""

    def __init__(self):
        self.bin = bytearray()
        self.j = {"asset": {"version": "2.0", "generator": "vf4ft-decomp tools/gltf.py"},
                  "buffers": [{}], "bufferViews": [], "accessors": [], "nodes": [], "meshes": [],
                  "skins": [], "materials": [], "textures": [], "images": [], "samplers": [],
                  "animations": [], "scenes": [{"nodes": []}], "scene": 0}

    def view(self, data, target=None):
        while len(self.bin) % 4:
            self.bin.append(0)
        v = {"buffer": 0, "byteOffset": len(self.bin), "byteLength": len(data)}
        if target:
            v["target"] = target
        self.bin += data
        self.j["bufferViews"].append(v)
        return len(self.j["bufferViews"]) - 1

    def accessor(self, arr, ctype, kind, target=None, minmax=False, normalized=False):
        arr = np.ascontiguousarray(arr)
        dt = {FLOAT: "<f4", U8: "u1", U16: "<u2", U32: "<u4"}[ctype]
        a = {"bufferView": self.view(arr.astype(dt).tobytes(), target), "componentType": ctype,
             "count": int(arr.shape[0]), "type": kind}
        if normalized:
            a["normalized"] = True
        if minmax:
            flat = arr.reshape(arr.shape[0], -1)
            a["min"] = [float(x) for x in flat.min(axis=0)]
            a["max"] = [float(x) for x in flat.max(axis=0)]
        self.j["accessors"].append(a)
        return len(self.j["accessors"]) - 1

    def glb(self, path):
        while len(self.bin) % 4:
            self.bin.append(0)
        self.j["buffers"][0]["byteLength"] = len(self.bin)
        for k in [k for k, v in self.j.items() if v == []]:
            del self.j[k]
        js = json.dumps(self.j, separators=(",", ":")).encode()
        js += b" " * (-len(js) % 4)
        with open(path, "wb") as f:
            f.write(struct.pack("<III", 0x46546C67, 2, 12 + 8 + len(js) + 8 + len(self.bin)))
            f.write(struct.pack("<II", len(js), 0x4E4F534A) + js)
            f.write(struct.pack("<II", len(self.bin), 0x004E4942) + bytes(self.bin))


def wrap(tsp, flip_bit, clamp_bit):
    if tsp >> clamp_bit & 1:
        return CLAMP
    return MIRROR if tsp >> flip_bit & 1 else REPEAT


def export(prog, src, texdir, motdir, s, out, names=None, common=False, rest=None, opts=None):
    ch = pose.Character(prog, src, s)
    if (opts or {}).get("no-lift"):
        ch.lift = None
    ch.reach = (opts or {}).get("reach")
    bones = ch.bones
    bank = json.load(open(os.path.join(motdir, f"motdat_mask_{BANK[ch.who]}.json")))["motions"]
    motions = [m for m in bank if names is None or m["name"] in names]
    if common:
        motions += json.load(open(os.path.join(motdir, "motdat_mask_CMN.json")))["motions"]
    if rest is None:
        rest = next((m["name"] for m in bank if "_SY_KAMAE" in m["name"]), bank[0]["name"])
    rest_m = next(m for m in bank + motions if m["name"] == rest)
    curves = lambda m: {int(k): v for k, v in m["bones"].items()}
    by_id = {m["id"]: m for m in bank}
    _, S0, W0 = next(ch.frames(curves(rest_m), 0))  # rest: frame 0, swing bones reset

    w = Writer()
    parent = {k: b["index"] for b in bones for k in b["children"]}
    # bones the rig walk never reaches (the STN_* attach points have no
    # parent; the game places them from code) are unanimated root joints
    live = set(W0)
    for b in bones:  # joint nodes 0..46 = bone indices
        if b["index"] not in live:
            w.j["nodes"].append({"name": b["name"]})
            continue
        local = W0[b["index"]] if b["index"] not in parent else np.linalg.inv(W0[parent[b["index"]]]) @ W0[b["index"]]
        t, q, sc = trs(local)
        nd = {"name": b["name"], "translation": [float(x) for x in t], "rotation": [float(x) for x in q]}
        if not np.allclose(sc, 1, atol=1e-4):
            nd["scale"] = [float(x) for x in sc]
        if b["children"]:
            nd["children"] = list(b["children"])
        w.j["nodes"].append(nd)
    w.j["scenes"][0]["nodes"] += [b["index"] for b in bones if b["index"] not in parent]
    slot_joint = {k: b for k, b in ch.slot_bone.items()}
    # the wrists (slots 8 / 13, rig.wrist): joints under the hand's parent
    # (the forearm), keyed with the wrist matrix in that frame
    wrist_par = {}
    for k, hand in sorted(ch.wrist_slots.items()):
        slot_joint[k] = len(w.j["nodes"])
        wrist_par[slot_joint[k]] = (k, parent[hand])
        t, q, sc = trs(np.linalg.inv(W0[parent[hand]]) @ S0[k])
        w.j["nodes"].append({"name": f"slot{k:03d}_wrist", "translation": [float(x) for x in t],
                             "rotation": [float(x) for x in q]})
        w.j["nodes"][parent[hand]].setdefault("children", []).append(slot_joint[k])
    # swing-bone slots (ex_*): root joints carrying the simulated world matrix
    swing = sorted(ch.swing_slots)
    for k in swing:
        slot_joint[k] = len(w.j["nodes"])
        t, q, sc = trs(S0[k])
        w.j["nodes"].append({"name": f"slot{k:03d}_swing", "translation": [float(x) for x in t],
                             "rotation": [float(x) for x in q]})
        w.j["scenes"][0]["nodes"].append(slot_joint[k])
    # cloth particles (type 0x0F nodes): root joints at the particle, turned
    # by the shortest arc from its rest normal to its current one
    cloth_joint, cloth_n0 = {}, {}
    for ci in sorted(ch.cloth):
        cl = ch.cloth[ci]
        cloth_n0[ci] = cl.normals().copy()
        for k in range(len(cl.pos)):
            cloth_joint[(ci, k)] = len(w.j["nodes"])
            w.j["nodes"].append({"name": f"{ch.nodes[ci]['name'][-16:]}_p{k:03d}",
                                 "translation": [float(x) for x in cl.pos[k]]})
            w.j["scenes"][0]["nodes"].append(cloth_joint[(ci, k)])
    cloth_p0 = {ci: ch.cloth[ci].pos.copy() for ci in ch.cloth}
    # the face (tools/face.py): under the head joint, a "face" node holding
    # the face rig and the static vertices, a "face_rigid" node holding the
    # rigid neutral face, and one node per eye. The game draws the skinned
    # face only while a face motion plays and the rigid face otherwise, so
    # each animation keys the hidden one's node scale to 0 (and the default
    # follows the rest motion)
    face_nodes, face_joint, eye_joint = {}, {}, {}
    head = ch.head
    face_rest = {}
    if ch.face:
        fb = ch.face["bones"]
        face_rest = ch.face_pose(face.rest_curves(fb), 0)
        fparent = {k: b["index"] for b in fb for k in b["children"]}
        shown = "face" in rest_m

        def add(name, local, scale=None):
            t, q, sc = trs(local)
            nd = {"name": name, "translation": [float(x) for x in t], "rotation": [float(x) for x in q]}
            sc = sc if scale is None else scale
            if not np.allclose(sc, 1, atol=1e-6):
                nd["scale"] = [float(x) for x in sc]
            w.j["nodes"].append(nd)
            return len(w.j["nodes"]) - 1

        face_nodes["face"] = add("face", np.eye(4), [1.0] * 3 if shown else [0.0] * 3)
        fb = [b for b in fb if b["index"] in face_rest]  # o_face / o_eye_*: no parent, never evaluated
        for b in fb:
            local = face_rest[b["index"]] if b["index"] not in fparent else \
                np.linalg.inv(face_rest[fparent[b["index"]]]) @ face_rest[b["index"]]
            face_joint[b["index"]] = add(f"face_{b['name']}", local)
        for b in fb:
            kids = [face_joint[k] for k in b["children"]]
            if kids:
                w.j["nodes"][face_joint[b["index"]]]["children"] = kids
        w.j["nodes"][face_nodes["face"]]["children"] = [face_joint[0]]
        face_nodes["rigid"] = add("face_rigid", np.eye(4), [0.0] * 3 if shown else [1.0] * 3)
        for n, M in ch.eye_matrices():
            eye_joint[n] = add(f"eye_{ch.nodes[n]['name'][-5:]}", M)
        w.j["nodes"][head].setdefault("children", []).extend(
            [face_nodes["face"], face_nodes["rigid"]] + list(eye_joint.values()))
    wrists = [slot_joint[k] for k in sorted(ch.wrist_slots)]
    joints = list(range(len(bones))) + wrists + [slot_joint[k] for k in swing] + [cloth_joint[x] for x in sorted(cloth_joint)]
    joints += list(face_nodes.values()) + [face_joint[k] for k in sorted(face_joint)] + list(eye_joint.values())
    jindex = {n: i for i, n in enumerate(joints)}  # node -> position in the skin's joint list
    rest_of = {**{b: W0[b] for b in live}, **{slot_joint[k]: S0[k] for k in swing + sorted(ch.wrist_slots)}}
    for (ci, k), n in cloth_joint.items():
        rest_of[n] = np.eye(4)
        rest_of[n][:3, 3] = cloth_p0[ci][k]
    if ch.face:
        rest_of[face_nodes["face"]] = rest_of[face_nodes["rigid"]] = W0[head]
        for k, n in face_joint.items():
            rest_of[n] = W0[head] @ face_rest[k]
        for n, M in ch.eye_matrices():
            rest_of[eye_joint[n]] = W0[head] @ M

    def ibm_default():
        return [np.linalg.inv(rest_of[n]) if n in rest_of else np.eye(4) for n in joints]

    # textures and materials
    descs = pvr_tex.descriptors(os.path.join(src, f"obj_{s}.bin"))
    texnames = [f"{i:03d}_{dw}x{dh}_{pvr_tex.PIXEL[f & 0xFF]}_{pvr_tex.TYPE[f & 0xF00]}.png"
                for i, (dw, dh, f, o_, x) in enumerate(descs)]
    images, samplers, textures, materials = {}, {}, {}, {}

    def material(tex, kind, info):
        tsp = info["tsp"]
        ws, wt = wrap(tsp, 18, 16), wrap(tsp, 17, 15)
        diffuse = info["diffuse"] if info["select"] & 1 else 0xFFFFFFFF
        double = (info["isp"] >> 27 & 3) < 2
        key = (tex, kind, ws, wt, diffuse, double)
        if key in materials:
            return materials[key]
        mat = {"name": f"tex{tex:03d}_{kind}" if tex is not None else f"untextured_{kind}",
               "doubleSided": double, "pbrMetallicRoughness": {"metallicFactor": 0.0, "roughnessFactor": 1.0}}
        if diffuse != 0xFFFFFFFF:  # ARGB -> RGBA factor (colors in glTF are linear; these are as stored)
            mat["pbrMetallicRoughness"]["baseColorFactor"] = [(diffuse >> 16 & 255) / 255, (diffuse >> 8 & 255) / 255,
                                                              (diffuse & 255) / 255, (diffuse >> 24 & 255) / 255]
        path = os.path.join(texdir, s, texnames[tex]) if tex is not None and tex < len(texnames) else None
        if path and os.path.exists(path):
            if tex not in images:
                images[tex] = len(w.j["images"])
                w.j["images"].append({"name": texnames[tex][:-4], "mimeType": "image/png",
                                      "bufferView": w.view(open(path, "rb").read())})
            if (ws, wt) not in samplers:
                samplers[(ws, wt)] = len(w.j["samplers"])
                w.j["samplers"].append({"magFilter": 9729, "minFilter": 9729, "wrapS": ws, "wrapT": wt})
            tk = (images[tex], samplers[(ws, wt)])
            if tk not in textures:
                textures[tk] = len(w.j["textures"])
                w.j["textures"].append({"source": tk[0], "sampler": tk[1]})
            mat["pbrMetallicRoughness"]["baseColorTexture"] = {"index": textures[tk]}
            if kind == "punch_through":
                mat["alphaMode"], mat["alphaCutoff"] = "MASK", 0.5
            elif kind == "translucent":
                mat["alphaMode"] = "BLEND"
        materials[key] = len(w.j["materials"])
        w.j["materials"].append(mat)
        return materials[key]

    def add_mesh(name, prims, ibm):
        """prims: material -> lists of (pos, nrm, uv, col, joints, weights, tris)."""
        out = []
        for mi, parts in prims.items():
            P, N, T, C, J, Wt, I = [], [], [], [], [], [], []
            base = 0
            for pos, nrm, uv, col, jn, wt, tris in parts:
                P += pos; N += nrm; T += uv; C += col; J += jn; Wt += wt
                I += [base + k for t in tris for k in t]
                base += len(pos)
            P, N, T = np.array(P, np.float32), np.array(N, np.float32), np.array(T, np.float32)
            n = np.linalg.norm(N, axis=1, keepdims=True)
            N = np.where(n > 1e-6, N / np.maximum(n, 1e-6), [0, 1, 0]).astype(np.float32)
            Wt = np.array(Wt, np.float32)
            Wt /= Wt.sum(axis=1, keepdims=True)
            attr = {"POSITION": w.accessor(P, FLOAT, "VEC3", 34962, minmax=True),
                    "NORMAL": w.accessor(N, FLOAT, "VEC3", 34962),
                    "TEXCOORD_0": w.accessor(T, FLOAT, "VEC2", 34962),
                    "JOINTS_0": (w.accessor(np.array(J, np.uint8), U8, "VEC4", 34962) if len(joints) < 256 else
                                 w.accessor(np.array(J, np.uint16), U16, "VEC4", 34962)),
                    "WEIGHTS_0": w.accessor(Wt, FLOAT, "VEC4", 34962)}
            if any(c is not None for c in C):
                attr["COLOR_0"] = w.accessor(np.array([c if c is not None else (1, 1, 1, 1) for c in C], np.float32),
                                             FLOAT, "VEC4", 34962)
            idx = np.array(I)
            ctype = U16 if base < 65536 else U32
            out.append({"attributes": attr, "indices": w.accessor(idx, ctype, "SCALAR", 34963), "material": mi})
        if not out:
            return
        w.j["meshes"].append({"name": name, "primitives": out})
        w.j["skins"].append({"name": name, "joints": joints,
                             "inverseBindMatrices": w.accessor(np.array([m.T.reshape(16) for m in ibm], np.float32),
                                                               FLOAT, "MAT4")})
        w.j["nodes"].append({"name": name, "mesh": len(w.j["meshes"]) - 1, "skin": len(w.j["skins"]) - 1})
        w.j["scenes"][0]["nodes"].append(len(w.j["nodes"]) - 1)

    def vertex_extra(v, info):
        uv = v["uv"] or (0.0, 0.0)
        uv = [c if c == c else 0.0 for c in uv]
        col = None
        if v["rgb"] is not None and not info["select"] & 1:  # d0: the GMP diffuse replaces it
            c = v["rgb"]
            col = ((c >> 16 & 255) / 255, (c >> 8 & 255) / 255, (c & 255) / 255, (c >> 24 & 255) / 255)
        return [uv[0], 1.0 - uv[1]], col

    def orient(tris, pos, nrm, info):
        """glTF winding (see the module notes)."""
        cull = info["isp"] >> 27 & 3
        if cull == 3:
            return tris
        if cull == 2:
            return [(a, c, b) for a, b, c in tris]
        P, N, out = np.array(pos), np.array(nrm), []
        for a, b, c in tris:
            f = np.cross(P[b] - P[a], P[c] - P[a])
            out.append((a, c, b) if np.dot(f, N[a] + N[b] + N[c]) < 0 else (a, b, c))
        return out

    approx = []
    # rigid pieces: one skin, vertices in the rest pose
    rigid = {}
    attach = {**{i: bone for i, bone in ch.rigid.items()},
              **{i: slot_joint[k] for i, k in ch.on_slot.items() if k in slot_joint}}
    if ch.face:
        attach[ch.face["rigid"][0]] = face_nodes["rigid"]
        attach.update(eye_joint)
    for i, node in sorted(attach.items()):
        b = ch.block(i)
        if not b:
            continue
        M = rest_of[node]
        bone = jindex[node]
        for tex, kind, verts, tris, info in mdl.model(ch.mdl, b, with_tsp=True):
            pos, nrm, uv, col = [], [], [], []
            for v in verts:
                pos.append(list((M @ np.array([*v["p"], 1.0]))[:3]))
                nrm.append(list(M[:3, :3] @ np.array(v["n"])))
                t, c = vertex_extra(v, info)
                uv.append(t); col.append(c)
            rigid.setdefault(material(tex, kind, info), []).append(
                (pos, nrm, uv, col, [[bone, 0, 0, 0]] * len(verts), [[1, 0, 0, 0]] * len(verts),
                 orient(tris, pos, nrm, info)))
    add_mesh(f"{s}_rigid", rigid, ibm_default())

    # envelopes: one skin each, the game's bind matrices
    for i in sorted(ch.envelopes):
        b = ch.block(i)
        if not b:
            continue
        groups, binds, use, ap = ch.envelope(i)
        approx += ap
        ibm = ibm_default()
        for slot in set(use.values()):
            if slot in slot_joint:
                ibm[jindex[slot_joint[slot]]] = binds[slot]
        at = {}
        for g in groups:
            jw = {}
            for slot, wt in g["influences"]:
                bone = jindex[slot_joint[use[slot]]]
                jw[bone] = jw.get(bone, 0.0) + wt
            jw = sorted(jw.items(), key=lambda x: -x[1])[:4]
            jn = [j for j, _ in jw] + [0] * (4 - len(jw))
            wt = [x for _, x in jw] + [0.0] * (4 - len(jw))
            for v in g["vertices"]:
                for off, flags in v["outputs"]:
                    at[off] = (v["pos"], v["normal"], jn, wt)
        prims = {}
        for tex, kind, verts, tris, info in mdl.model(ch.mdl, b, with_tsp=True):
            pos, nrm, uv, col, jj, ww = [], [], [], [], [], []
            for v in verts:
                p_, n_, jn, wt = at.get(v["offset"], (v["p"], v["n"], [0, 0, 0, 0], [1, 0, 0, 0]))
                pos.append(list(p_)); nrm.append(list(n_)); jj.append(jn); ww.append(wt)
                t, c = vertex_extra(v, info)
                uv.append(t); col.append(c)
            prims.setdefault(material(tex, kind, info), []).append(
                (pos, nrm, uv, col, jj, ww, orient(tris, pos, nrm, info)))
        add_mesh(ch.nodes[i]["name"], prims, ibm)

    # the skinned face: the game's bind matrices as inverse bind matrices of
    # the face bones; the static vertices ride the "face" node (head frame)
    if ch.face:
        f = ch.face
        ibm = ibm_default()
        for g in f["groups"]:
            for bi, wt, bind in g["influences"]:
                ibm[jindex[face_joint[bi]]] = bind
        ibm[jindex[face_nodes["face"]]] = np.eye(4)
        at = {}
        for g in f["groups"]:
            jw = {}
            for bi, wt in ((bi, wt) for bi, wt, _ in g["influences"]):
                jw[jindex[face_joint[bi]]] = jw.get(jindex[face_joint[bi]], 0.0) + wt
            if len(jw) > 4:  # glTF viewers read one set of 4 joints: keep the 4 largest, renormalized
                approx.append(f"{ch.nodes[f['node']]['name']}: {sum(len(v['outputs']) for v in g['vertices'])} "
                              f"vertices with {len(jw)} face bones keep the largest 4")
            jw = sorted(jw.items(), key=lambda x: -x[1])[:4]
            jn = [j for j, _ in jw] + [0] * (4 - len(jw))
            wt = [x for _, x in jw] + [0.0] * (4 - len(jw))
            for v in g["vertices"]:
                for off, flags in v["outputs"]:
                    at[off] = (v["pos"], v["normal"], jn, wt)
        for st in f["static"]:
            at[st["offset"]] = (st["pos"], st["normal"], [jindex[face_nodes["face"]], 0, 0, 0], [1.0, 0, 0, 0])
        prims = {}
        for tex, kind, verts, tris, info in mdl.model(ch.mdl, ch.block(f["node"]), with_tsp=True):
            pos, nrm, uv, col, jj, ww = [], [], [], [], [], []
            for v in verts:
                p_, n_, jn, wt = at[v["offset"]]
                pos.append(list(p_)); nrm.append(list(n_)); jj.append(jn); ww.append(wt)
                t, c = vertex_extra(v, info)
                uv.append(t); col.append(c)
            prims.setdefault(material(tex, kind, info), []).append(
                (pos, nrm, uv, col, jj, ww, orient(tris, pos, nrm, info)))
        add_mesh(ch.nodes[f["node"]]["name"], prims, ibm)

    # cloth: vertices follow their particle (the maps), normals per face
    for ci in sorted(ch.cloth):
        b = ch.block(ci)
        if not b:
            continue
        cl = ch.cloth[ci]
        side = {}
        sign = (-1.0, 1.0) if cl.params["order"] == 0 else (1.0, -1.0)  # 0x0C0CDA20: which map gets -normal
        for mi_, mp in enumerate(cl.maps):
            for k, outs in enumerate(mp):
                for off, flags in outs:
                    side[off] = (k, sign[mi_])
        prims = {}
        for tex, kind, verts, tris, info in mdl.model(ch.mdl, b, with_tsp=True):
            pos, nrm, uv, col, jj, ww = [], [], [], [], [], []
            for v in verts:
                k, sg = side.get(v["offset"], (0, 1.0))
                pos.append(list(cloth_p0[ci][k])); nrm.append(list(sg * cloth_n0[ci][k]))
                jj.append([jindex[cloth_joint[(ci, k)]], 0, 0, 0]); ww.append([1, 0, 0, 0])
                t, c = vertex_extra(v, info)
                uv.append(t); col.append(c)
            prims.setdefault(material(tex, kind, info), []).append(
                (pos, nrm, uv, col, jj, ww, orient(tris, pos, nrm, info)))
        add_mesh(ch.nodes[ci]["name"], prims, ibm_default())

    # animations, baked from the IK emulation
    for m in motions:
        cv = curves(m)
        frames = max(int(m["frames"]), 1)
        n_nodes = len(w.j["nodes"])
        T = np.zeros((frames, n_nodes, 3))
        Q = np.zeros((frames, n_nodes, 4))
        swing_node = {slot_joint[k]: k for k in swing}
        cloth_node = {n: ck for ck, n in cloth_joint.items()}
        animated = sorted(live) + sorted(wrist_par) + sorted(swing_node) + sorted(cloth_node)
        heads = []
        for f, sl, W in ch.frames(cv, frames - 1, ch.motion_options(m, motdir, src, opts), opts.get("wind")):
            heads.append(W[ch.head])
            normals = {ci: ch.cloth[ci].normals() for ci in ch.cloth}
            for k in animated:
                if k in cloth_node:
                    ci, pk = cloth_node[k]
                    t, q = ch.cloth[ci].pos[pk], arc(cloth_n0[ci][pk], normals[ci][pk])
                    if f and np.dot(q, Q[f - 1, k]) < 0:
                        q = -q
                    T[f, k], Q[f, k] = t, q
                    continue
                if k < len(bones):
                    local = W[k] if k not in parent else np.linalg.inv(W[parent[k]]) @ W[k]
                elif k in wrist_par:
                    local = np.linalg.inv(W[wrist_par[k][1]]) @ sl[wrist_par[k][0]]
                else:
                    local = sl[swing_node[k]]
                t, q, sc = trs(local)
                if f and np.dot(q, Q[f - 1, k]) < 0:  # keep quaternions on one hemisphere
                    q = -q
                T[f, k], Q[f, k] = t, q
        S = {}
        if ch.face:  # the face motion this move sets, else the rigid face
            fm = by_id.get(m.get("face"))
            if "face" in m and fm is None:
                raise ValueError(f"{m['name']}: face motion {m['face']} not in the bank")
            fcv = curves(fm) if fm else None
            on, off = np.ones((frames, 3)), np.zeros((frames, 3))
            S[face_nodes["face"]], S[face_nodes["rigid"]] = (on, off) if fm else (off, on)
            for f in range(frames):
                FM = ch.face_pose(fcv, f) if fm else face_rest
                for b in fb:
                    k = face_joint[b["index"]]
                    local = FM[b["index"]] if b["index"] not in fparent else \
                        np.linalg.inv(FM[fparent[b["index"]]]) @ FM[b["index"]]
                    t, q, sc = trs(local)
                    if f and np.dot(q, Q[f - 1, k]) < 0:
                        q = -q
                    T[f, k], Q[f, k] = t, q
                for n, M in ch.eye_matrices(fcv, f, (opts or {}).get("eyes"), heads[f]):
                    k = eye_joint[n]
                    t, q, sc = trs(M)
                    if f and np.dot(q, Q[f - 1, k]) < 0:
                        q = -q
                    T[f, k], Q[f, k] = t, q
            animated = animated + [face_joint[k] for k in sorted(face_joint)] + list(eye_joint.values())
        times = np.arange(frames, dtype=np.float32) / 30
        tin = w.accessor(times, FLOAT, "SCALAR", minmax=True)
        one = w.accessor(np.zeros(1, np.float32), FLOAT, "SCALAR", minmax=True)
        samplers_, channels = [], []
        for k, data in S.items():
            const = np.allclose(data, data[0])
            samplers_.append({"input": one if const else tin, "interpolation": "STEP" if not const else "LINEAR",
                              "output": w.accessor(data[:1] if const else data, FLOAT, "VEC3")})
            channels.append({"sampler": len(samplers_) - 1, "target": {"node": k, "path": "scale"}})
        for k in animated:
            for path, data, kind in (("translation", T[:, k], "VEC3"), ("rotation", Q[:, k], "VEC4")):
                const = np.allclose(data, data[0], atol=1e-6)
                samplers_.append({"input": one if const else tin, "interpolation": "LINEAR",
                                  "output": w.accessor(data[:1] if const else data, FLOAT, kind)})
                channels.append({"sampler": len(samplers_) - 1, "target": {"node": k, "path": path}})
        w.j["animations"].append({"name": f"{m['id']:04d}_{m['name']}", "samplers": samplers_, "channels": channels})

    os.makedirs(os.path.dirname(os.path.abspath(out)), exist_ok=True)
    w.glb(out)
    return len(motions), approx, rest


def main():
    a, opts = pose.parse_options(sys.argv)
    if "from" in opts:
        sys.exit("--from is pose.py's (one motion); gltf.py takes --mirror, --smooth, --look, --eyes, --wind, "
                 "--no-lift, --reach")
    if len(a) < 7:
        sys.exit(__doc__)
    prog_path, src, texdir, motdir, s, out = a[1:7]
    names = None
    if "--motions" in a and a[a.index("--motions") + 1] != "all":
        names = set(a[a.index("--motions") + 1].split(","))
    rest = a[a.index("--rest") + 1] if "--rest" in a else None
    n, approx, rest = export(open(prog_path, "rb").read(), src, texdir, motdir, s, out, names, "--common" in a, rest,
                             opts)
    for x in approx:
        print("approximated:", x, file=sys.stderr)
    print(f"{out}: {n} animations, rest pose {rest}, {os.path.getsize(out):,} bytes", file=sys.stderr)


if __name__ == "__main__":
    main()
