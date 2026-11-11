#!/usr/bin/env python3
"""Faces: the rigid expression models and the skinned face with its 33-bone rig.

Read from main.bin (rob code; names are placeholders in the decompile):
  costume table  character record +0x48, per costume k (0 / 1):
                   +0x20 k + 0x08  4 rigid face model ids (set << 16 | node):
                                   0 neutral, 1 for 10 frames after a voice
                                   cue (0x0C067800), 2 while rob +0x52C bit 3
                                   is held (set by 0x0C075600 / 0x0C076520),
                                   3 a blink, a 1-in-100 chance per frame,
                                   0-8 frames long (0x0C069760)
                   +0x80 + 4 k     the skinned face model (a type 0x0D node)
                   +0x88           face rig: 33 bone records (0 = LAU's,
                                   0x0C192E30)
                   +0x8C           float[3] face offset
Drawing (0x0C0D0AA0), in the head frame (matrix slot 3):
  no face motion (rob +0x3AC = 0)   the rigid face picked above
  a face motion (the *_KAO motions) the skinned face:
    rig      0x0C0975E0 / 0x0C097640, from identity, per bone by its flags:
               0x20000000  scale by the next entry of (10, 10, 10),
                           (0.01626, 0.01626, 0.01626) (0x0C2A0500)
               0x100 / 0x200  rest translate / rotate (Z, Y, X)
               0x400       motion translate x 0.1 (the motions store 10 x
                           the rest position, so translate keys are offsets
                           from it in effect)
               0x800       motion rotate; bones 31 / 32 (c_eye_r / l) only
                           copy their angles to rob +0x52E.. for the eyes
               0x1000      store the matrix (rob +0x58 + 0x40 x bone)
             then each stored matrix is premultiplied by translate(0x0C37F92C,
             never written: 0) x translate(costume +0x8C) (0x0C0948A0)
    skin     0x0C0D8480 over node +0x80: groups of { n, 0, n x (float
             weight, bone, float bind[16]) } (n = 1: { 1, 0, 0, bone,
             bind[16] }), each followed by vertex records { c, 0, float
             normal[3], float position[3], c x (model block offset, header
             flags) } ending in 0, then 0; n = 0 ends. A vertex is
             sum(weight x bone x bind) x position, its normal packed into
             the header's low 24 bits (x, y, z bytes x 127)
    static   0x0C0D8660, right after: { offset, header, x, y, z, float
             normal[3] } records ending in offset 0; offset, header and
             position are copied over the skinned result
  eyes       0x0C0CC4A0, before either face: 0x0C2B0B1C, 0x20 bytes per
             character id x 2 + costume, { model id, float offset[3] } for
             eye 0 and eye 1 (-1: none; SHUN's faces carry their own). An
             eye is drawn at head x translate(offset) x translate(node +0x04)
             x rotate Z, Y, X (node +0x18 / +0x14 / +0x10) x rotate Z, Y, X
             by its angles (0x0C0CC3C0). The node angles are radians but
             the code truncates them (ftrc) and passes them as 16-bit angle
             units, so the 1.5708 / 3.1416 that half the eye nodes carry turn
             them by 1 or 3 / 65536 of a turn: in effect not at all. Eye 0 takes
             c_eye_r's motion angles, eye 1 c_eye_l's (both mirrored when the
             fighter faces left); a move start zeroes them (0x0C0659E0).
             The look-at (eyes_look_at, 0x0C093340; while a move's timed
             command 41 sets rob +0x52D: bit 0 the camera, bit 1 the
             opponent's head) replaces the Y and Z angles of both eyes
             (eye_look; pose.py / gltf.py --eyes)
"""
import math
import struct

import numpy as np

import rig

BASE = 0x0C020000
DEFAULT_RIG = 0x0C192E30  # LAU_face_skl, used when a costume table has none
FACE_SCALE = 0x0C2A0500   # two float[3] scales for the 0x20000000 bones
FACE_BONES = 0x21
EYES = 0x0C2B0B1C          # 0x20 bytes per character id x 2 + costume
EYE_BONES = (31, 32)       # c_eye_r -> eye 0, c_eye_l -> eye 1


def _u(prog, a):
    return struct.unpack_from("<I", prog, a - BASE)[0]


def costume_table(prog, char_index):
    return _u(prog, rig.CHARS + 0x4C * char_index + 0x48)


def info(prog, char_index, set_index):
    """The costume of a character whose face lives in set_index:
    {costume, face (model id), rigid (4 model ids), rig (bone defs), offset} or None."""
    t = costume_table(prog, char_index)
    if not BASE <= t < BASE + len(prog):
        return None
    for k in range(2):
        mid = _u(prog, t + 0x80 + 4 * k)
        if mid == 0xFFFFFFFF or mid >> 16 != set_index:
            continue
        rl = _u(prog, t + 0x20 * k + 8)
        return {"costume": k, "face": mid,
                "rigid": [_u(prog, rl + 4 * j) for j in range(4)] if rl else [],
                "rig": _u(prog, t + 0x88) or DEFAULT_RIG,
                "offset": list(struct.unpack_from("<3f", prog, t + 0x8C - BASE))}
    return None


def eyes(prog, char_id, costume):
    """[(model id, offset)] for eye 0 and eye 1, skipping absent ones."""
    a = EYES + 0x20 * (char_id * 2 + costume)
    out = []
    for j in range(2):
        mid = _u(prog, a + 16 * j)
        if mid != 0xFFFFFFFF:
            out.append((mid, list(struct.unpack_from("<3f", prog, a + 16 * j + 4 - BASE))))
    return out


def angle(a):
    """A node angle as eye_draw uses it: the radians truncated to an integer, read as 16-bit units."""
    return int(a) * 6.2831855 / 65536.0


def eye_matrix(node, offset, ang):
    """Head-frame matrix of an eye model (node from mdl.nodes_of, ang = (x, y, z) radians)."""
    r = node["rotation"]
    M = rig.trans(offset) @ rig.trans(node["position"])
    for ax, a in (("z", angle(r[2])), ("y", angle(r[1])), ("x", angle(r[0]))):
        M = M @ rig.rot_axis(ax, np.sin(a), np.cos(a))
    return M @ rig.rot_zyx(ang)


def eye_look(head, mid, p):
    """eyes_look_at: the (Y, Z) eye angles in radians toward the point p (the
    head's space), seen from mid (halfway between the eyes, in the head frame
    head): yaw atan2(z, x) held to -0x1200 .. 0x1000 and pitch held to
    +-0x1C00, the yaw scaled by 1 - |pitch| / 0x1C00; both eyes take minus
    each (rob +0x52E / +0x530, +0x532 / +0x534)."""
    x, y, z = (np.linalg.inv(head @ rig.trans(mid)) @ np.array([*p, 1.0]))[:3]
    yaw = max(-0x1200, min(0x1000, rig.ang16(math.atan2(z, x))))
    pitch = max(-0x1C00, min(0x1C00, rig._s16(rig.ang16(math.atan2(math.sqrt(x * x + z * z), y)) - 0x4000)))
    yaw = int(yaw * (1.0 - abs(pitch) / 7168.0))
    # radians that rig.rot_zyx turns back into these 16-bit angles exactly
    return [(-v + (0.5 if -v > 0 else -0.5 if -v < 0 else 0.0)) / 10430.378 for v in (yaw, pitch)]


def eye_angles(curves, frame):
    """(x, y, z) of c_eye_r and c_eye_l at a frame of a face motion."""
    from motion import sample
    return [[sample(curves[b][c], frame) if c in curves.get(b, {}) else 0.0 for c in ("ch3", "ch4", "ch5")]
            for b in EYE_BONES]


def bones(prog, defs):
    return rig.bones_at(prog, defs, FACE_BONES)


def scales(prog):
    return [list(struct.unpack_from("<3f", prog, FACE_SCALE + 12 * k - BASE)) for k in range(2)]


def pose(bones_, curves, frame, offset, scale):
    """Face bone matrices (bone index -> 4x4) in the head frame for a face motion at a frame.

    Every bone is returned (glTF needs the whole tree); the game stores only
    the 0x1000 bones, the only ones the skin uses."""
    from motion import sample

    def channels(i, names):
        ch = curves.get(i, {})
        return [sample(ch[n], frame) if n in ch else 0.0 for n in names]

    out, k = {}, [0]
    pre = rig.trans(offset)

    def walk(i, M):
        b = bones_[i]
        f = b["flags"]
        if f & 0x20000000:
            M = M @ np.diag([*scale[k[0]], 1.0])
            k[0] += 1
        if f & 0x100:
            M = M @ rig.trans(b["pos"])
        if f & 0x200:
            M = M @ rig.rot_zyx(b["rot"])
        if f & 0x400:
            M = M @ rig.trans([0.1 * c for c in channels(i, ("ch0", "ch1", "ch2"))])
        if f & 0x800:
            M = M @ rig.rot_zyx(channels(i, ("ch3", "ch4", "ch5")))
        out[i] = pre @ M
        for c in b["children"]:
            walk(c, M)

    walk(0, np.eye(4))
    return out


def rest_curves(bones_):
    """Constant translate channels at 10 x each 0x400 bone's rest position: the
    neutral face (every *_KAO motion starts from these values)."""
    return {b["index"]: {f"ch{j}": [[0, 10.0 * b["pos"][j], 0.0, 0.0]] for j in range(3)}
            for b in bones_ if b["flags"] & 0x400}


def skin(obj, node_off):
    """(groups, static) of a type 0x0D face node; binds as column-vector 4x4."""
    w = lambda o: struct.unpack_from("<I", obj, o)[0]
    fl = lambda o: struct.unpack_from("<f", obj, o)[0]
    p = w(node_off + 0x80)
    groups = []
    while w(p):
        n = w(p)
        infl = []
        if n == 1:
            infl.append((w(p + 12) & 0xFFFFFF, 1.0, np.array(struct.unpack_from("<16f", obj, p + 16)).reshape(4, 4).T))
            p += 80
        else:
            p += 8
            for _ in range(n):
                infl.append((w(p + 4) & 0xFFFFFF, fl(p), np.array(struct.unpack_from("<16f", obj, p + 8)).reshape(4, 4).T))
                p += 72
        verts = []
        while w(p):
            c = w(p)
            verts.append({"normal": struct.unpack_from("<3f", obj, p + 8),
                          "pos": struct.unpack_from("<3f", obj, p + 20),
                          "outputs": [(w(p + 32 + 8 * k), w(p + 36 + 8 * k)) for k in range(c)]})
            p += 32 + 8 * c
        groups.append({"influences": infl, "vertices": verts})
        p += 8
    static = []
    p += 4
    while w(p):
        static.append({"offset": w(p), "header": w(p + 4), "pos": struct.unpack_from("<3f", obj, p + 8),
                       "normal": struct.unpack_from("<3f", obj, p + 20)})
        p += 32
    return groups, static


def apply(groups, static, mats):
    """model block offset -> head-frame position (static records last, as the game)."""
    posed = {}
    for g in groups:
        M = sum(wt * mats[b] @ bind for b, wt, bind in g["influences"])
        for v in g["vertices"]:
            p = (M @ np.array([*v["pos"], 1.0]))[:3]
            for off, flags in v["outputs"]:
                posed[off] = p
    for s in static:
        posed[s["offset"]] = np.array(s["pos"])
    return posed
