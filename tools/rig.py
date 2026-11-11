#!/usr/bin/env python3
"""Character skeletons (main.bin) and posing them with a motion.

Skeletons (read from main.bin; see docs/asset-formats.md):
  0x0C1678E4  character records, 0x4C bytes: +0x00 name, +0x24 height
              offset (the lift below), +0x30 character id (the switches in
              the rob code use it), +0x31 body rig
              index, +0x48 costume table (+0x88 of it: the face rig)
  0x0C1664A0  13 body rigs as { bone *defs, u32 count = 47 }
  bone record, 0x2C bytes (0x0C18BF54 onward): +0x00 name, +0x04 flags,
              +0x08 IK bend sign, +0x0C position, +0x18 rotation (radians),
              +0x24 child count, +0x28 child index bytes
Evaluation (0x0C0953C0 / 0x0C095860 / 0x0C095B40, a matrix stack walked
from bone 0; a child's matrix is parent x local, rotations applied Z, Y, X
with angles wrapped to 16 bits as the game does):
  plain bones   0x100 rest translate, 0x200 rest rotate, 0x400 motion
                translate (channels 0-2), 0x800 motion rotate (channels 3-5)
  0x2000000     (gblctr) this matrix is the global frame G
  0x80          IK, with
    0x1000000   aim: turn local X at the first child's target (its motion
                position, in G) (0x0C096660)
    0x800000    chain root: aim at the first grandchild's target, then bend
                by the law-of-cosines angle (0x0C0966C0) for lengths
                (child pos x, distance, grandchild pos x)
    0x200000    chain middle: translate by own pos, bend by pi - interior
                angle of (own pos x, child pos x, distance)
  The bend's sine is negative when +0x08 is 0.
  0x8000000 /   the hand bones (k_te_l / k_te_r) also give a wrist matrix
  0x4000000     (rob_eval_output): the hand's matrix before its rotation,
                turned about X by atan2(|m.z|, m.y) - 90 degrees, m the
                hand's local Z axis through its motion rotation (pose(),
                wrists=)
Body slots (body_slots): the fighter's 31 bone matrices (rob +0x4C) are the
bones of the table at 0x0C18BCB8, the wrist matrices at entries 9 / 14
(rob_eval_output stores them at +0x240 / +0x380); rob_skeleton_update
copies the first 24 into the body's 162 slots by the s16 table at
0x0C18BCD8 (-1: none).
pose() runs the game's passes (pose_passes): the IK solved with rig 0's
proportions for every fighter, the mirror, the end-of-move blend to the
stance pose (tools/smooth.py), the motion-change blend and the head
look-at, the height lift (pose_passes lift=) and the attack reach
(reach=). Not emulated: the reach's limb targets and the smooth blend's
down and turned stances. The face rig is tools/face.py.

usage: rig.py dump <main.bin> <out dir>     characters + the 13 body rigs to JSON
       rig.py <main.bin> <motion json> <motion name> <frame> [rig index]
                                           prints each bone's world position
"""
import json
import math
import struct
import sys

import numpy as np

sys.path.insert(0, __import__("os").path.dirname(__import__("os").path.abspath(__file__)))
import motion as motion_mod  # noqa: E402

BASE = 0x0C020000
CHARS = 0x0C1678E4
RIGS = 0x0C1664A0
MTX_BONES = 0x0C18BCB8   # 31 s8: the bone of each rob +0x4C matrix (negative: none)
MTX_SLOTS = 0x0C18BCD8   # 24 s16: the body slot each of the first 24 is copied to
WRIST_ENTRY = {0x8000000: 9, 0x4000000: 14}  # hand flag -> its wrist matrix (+0x240 / +0x380)
THIGH_AIM = {3: -0.02, 10: 0.025}  # rob_eval_output's extra on the lowered thigh aims: SHUN, AOI
REACH_AIM = {34: 3, 27: 4, 18: 1, 10: 2}  # rob_eval_output: chain root bone -> reach.current index


def lift_on(height):
    """rob_attack_reach: reach flag 4 is on in a fight for a height offset
    above 0.001 or below 0 (off in the states that clear it: rob +0x20 0x800
    or 0x18000000, some crouching, rob +0x24 0x40, +0x2C 0x10; a negative one
    also in some menu modes)."""
    return height > 0.001 or height < 0.0
MIRROR_POS = 0x0C2A0614     # rob_pose_mirror (0x0C097AA0): position pairs
MIRROR_FLIPS = 0x0C2A05C4   # angle flips
MIRROR_PAIRS = 0x0C2A05E8   # angle pairs


def _u(prog, a):
    return struct.unpack_from("<I", prog, a - BASE)[0]


def _cs(prog, a):
    return prog[a - BASE:prog.index(b"\0", a - BASE)].decode("latin1")


def characters(prog):
    out = []
    for i in range(22):
        a = CHARS + 0x4C * i
        if not BASE <= _u(prog, a) < BASE + len(prog):
            break
        out.append({"index": i, "name": _cs(prog, _u(prog, a)), "id": prog[a + 0x30 - BASE],
                    "rig": prog[a + 0x31 - BASE],
                    "height": struct.unpack_from("<f", prog, a + 0x24 - BASE)[0]})
    return out


def rig(prog, index):
    tab, n = struct.unpack_from("<II", prog, RIGS + 8 * index - BASE)
    return bones_at(prog, tab, n)


def body_slots(prog, bones):
    """(slot -> bone index, slot -> hand bone index whose wrist matrix fills it)."""
    owner = struct.unpack_from("<31b", prog, MTX_BONES - BASE)
    slot = struct.unpack_from("<24h", prog, MTX_SLOTS - BASE)
    hands = {WRIST_ENTRY[b["flags"] & 0xC000000]: b["index"] for b in bones if b["flags"] & 0xC000000}
    slots, wrists = {}, {}
    for e in range(24):
        if slot[e] < 0:
            continue
        if owner[e] >= 0:
            slots[slot[e]] = owner[e]
        elif e in hands:
            wrists[slot[e]] = hands[e]
    return slots, wrists


def bones_at(prog, tab, n):
    bones = []
    for i in range(n):
        a = tab + 0x2C * i
        nk, kp = _u(prog, a + 0x24), _u(prog, a + 0x28)
        bones.append({"index": i, "name": _cs(prog, _u(prog, a)), "flags": _u(prog, a + 4),
                      "bend": _u(prog, a + 8),
                      "pos": list(struct.unpack_from("<3f", prog, a + 0xC - BASE)),
                      "rot": list(struct.unpack_from("<3f", prog, a + 0x18 - BASE)),
                      "children": list(prog[kp - BASE:kp - BASE + nk]) if nk else []})
    return bones


def _wrap(a):
    """radians -> the game's 16-bit angle -> radians (0x0C0968A0, 0x0C0F5AB0)."""
    return (int(a * 10430.378) & 0xFFFF) * 6.2831855 / 65536.0


def rot_axis(axis, s, c):
    m = np.eye(4)
    i, j = {"x": (1, 2), "y": (2, 0), "z": (0, 1)}[axis]
    m[i, i], m[i, j], m[j, i], m[j, j] = c, -s, s, c
    return m


def rot_zyx(r):
    m = np.eye(4)
    for ax, a in (("z", r[2]), ("y", r[1]), ("x", r[0])):
        a = _wrap(a)
        m = m @ rot_axis(ax, math.sin(a), math.cos(a))
    return m


def trans(p):
    m = np.eye(4)
    m[:3, 3] = p
    return m


def aim(v):
    """0x0C096660: turn local X toward v."""
    x, y, z = v
    rxy2 = x * x + y * y
    rxy = math.sqrt(rxy2)
    if rxy < 1e-12:
        return np.eye(4)
    r = math.sqrt(z * z + rxy2)
    return rot_axis("z", y / rxy, x / rxy) @ rot_axis("y", -z / r, rxy / r)


def cos_law(a, b, c, sign):
    """0x0C0966C0 -> (sin, cos) of the angle between a and b, opposite c."""
    cs = (a * a + b * b - c * c) / (2 * a * b)
    cs = min(1.0, max(-1.0, cs))
    s = math.sqrt(1 - cs * cs)
    return (s if sign else -s), cs


def wrist(pre, rot):
    """rob_eval_output's wrist matrix: pre (the hand before its rotation) turned
    about X by the twist of the hand's local rotation rot (mtx_xform_dir of
    (0, 0, 1) is rot @ e_z; a negative z is folded to positive)."""
    m = rot[:3, :3] @ np.array([0.0, 0.0, 1.0])
    r = _wrap(math.atan2(abs(m[2]), m[1]) - math.pi / 2)
    return pre @ rot_axis("x", math.sin(r), math.cos(r))


def look_clamp(chest, p):
    """look_target_clamp (0x0C0978A0): the look-at point p in the frame of the
    chest (j_mune's output matrix) turned 0x4000 about Y and X, its yaw held to
    +-0x3800 and its pitch to -0x800 .. 0x3000, back in p's space."""
    K = chest @ rot16([0x4000, 0x4000, 0])
    x, y, z = (np.linalg.inv(K) @ np.array([*p, 1.0]))[:3]
    a = ang16(math.atan2(x, z))
    if a > 0x3800 or a < -0x3800:
        x, _, z = rot16([0, (0x3800 if a > 0 else -0x3800) - a, 0])[:3, :3] @ np.array([x, 0.0, z])
    e = _s16(0x4000 - ang16(math.atan2(math.sqrt(x * x + z * z), y)))
    if e > 0x3000 or e < -0x800:
        x, y, z = rot16([e - 0x3000 if e > 0 else e + 0x800, 0, 0])[:3, :3] @ np.array([x, y, z])
    return (K @ np.array([x, y, z, 1.0]))[:3]


def rot_lerp(a, b, t):
    """mtx_rot_lerp (0x0C094140): the rotation between a (t = 1) and b. In
    this column-vector layout the game's X / Y / Z columns are the rows: X the
    normalized blend of the first rows, Y the second rows' blend made
    orthogonal through two cross products, Z = X x Y."""
    pa = t * a[0, :3] + (1 - t) * b[0, :3]
    pc = t * a[1, :3] + (1 - t) * b[1, :3]
    d = np.cross(pa, pc)
    out = np.eye(4)
    for row, v in enumerate((pa, np.cross(d, pa), d)):
        n = float(np.linalg.norm(v))
        out[row, :3] = v / n if n > 1.1920929e-07 else v
    return out


def _s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def ang16(a):
    """radians -> the game's signed 16-bit angle (truncated, as 0x0C0968A0 and atan2_s16)."""
    return _s16(int(a * 10430.378))


def rot16(a):
    """mtx_rot_zyx_s16: Z, then Y, then X by 16-bit angles."""
    m = np.eye(4)
    for ax, v in (("z", a[2]), ("y", a[1]), ("x", a[0])):
        r = v * 6.2831855 / 65536.0
        m = m @ rot_axis(ax, math.sin(r), math.cos(r))
    return m


def euler16(M):
    """mtx_to_euler (0x0C096760): the rotation of M as three 16-bit angles."""
    s = M[2, 0]
    y = _s16(int(math.asin(max(-1.0, min(1.0, -s))) * 10430.378))
    if abs(s) < 0.9999999:
        return [ang16(math.atan2(M[2, 1], M[2, 2])), y, ang16(math.atan2(M[1, 0], M[0, 0]))]
    z = ang16(math.atan2(M[1, 2], M[1, 1]))
    return [0, y, z if s < 0 else _s16(-z)]


def mirror_tables(prog):
    """rob_pose_mirror's three lists (4-byte entries ended by a negative first
    byte): position pairs (a, b, xyz negate bits), angle flips (bone, mode x3:
    0 keep, 1 negate, 2/3 -0x8000 - a), angle pairs (a, b, xyz negate bits)."""
    def read(a, fmt):
        out = []
        while prog[a - BASE] < 0x80:
            out.append(struct.unpack_from(fmt, prog, a - BASE))
            a += 4
        return out
    return read(MIRROR_POS, "<bbh"), read(MIRROR_FLIPS, "<b3B"), read(MIRROR_PAIRS, "<bbh")


def mirror(tables, A, P):
    """rob_pose_mirror on the sampled angles A and positions P (bone -> 3 values)."""
    pos, flips, pairs = tables
    for a, b, f in pos:
        if a in P and b in P:
            pa = [-v if f >> k & 1 else v for k, v in enumerate(P[a])]
            pb = [-v if f >> k & 1 else v for k, v in enumerate(P[b])]
            P[a], P[b] = pb, pa
    for bone, *modes in flips:
        A[bone] = [v if m == 0 else _s16(-v) if m == 1 else _s16(-0x8000 - v) if m in (2, 3) else 0
                   for v, m in zip(A[bone], modes)]
    for a, b, f in pairs:
        ra = [_s16(-v) if f >> k & 1 else v for k, v in enumerate(A[a])]
        rb = [_s16(-v) if f >> k & 1 else v for k, v in enumerate(A[b])]
        A[a], A[b] = rb, ra


def pose(bones, curves, frame, wrists=None, flip=None, base=None, **opts):
    """World matrices of every bone for a motion (bone index -> channels) at a frame.
    wrists, a dict, receives hand bone index -> wrist matrix. base, rig 0's
    bones, is what the game solves the IK with first (pose_passes); for a
    fighter on another rig, with flip (the mirror_tables: the motion
    mirrored, rob +0x318 bit 4) or with any of pose_passes' blends (opts:
    smooth, look, blend), the pose goes through the game's passes. One pass
    gives the same for rig 0 without them."""
    opts = {k: v for k, v in opts.items() if v is not None}
    if flip is not None or opts or base is not None and any(a["pos"] != b["pos"] for a, b in zip(base, bones)):
        return pose_passes(bones, curves, frame, wrists, flip, base=base, **opts)

    def channels(b, names):
        ch = curves.get(b, {})
        return [motion_mod.sample(ch[n], frame) if n in ch else 0.0 for n in names]

    world, G, dist = {}, [np.eye(4)], {}

    def target_local(M, b):
        p = channels(b, ("ch0", "ch1", "ch2"))
        w = G[0] @ np.array([*p, 1.0])
        return (np.linalg.inv(M) @ w)[:3]

    def walk(i, M):
        b = bones[i]
        f = b["flags"]
        if not f & 0x80:
            if f & 0x100:
                M = M @ trans(b["pos"])
            if f & 0x200:
                M = M @ rot_zyx(b["rot"])
            if f & 0x400:
                M = M @ trans(channels(i, ("ch0", "ch1", "ch2")))
            if f & 0x800:
                R = rot_zyx(channels(i, ("ch3", "ch4", "ch5")))
                if f & 0xC000000 and wrists is not None:
                    wrists[i] = wrist(M, R)
                M = M @ R
            if f & 0x2000000:
                G[0] = M
        else:
            if f & 0x1000000:
                M = M @ aim(target_local(M, b["children"][0]))
            if f & 0x800000:
                kid = bones[b["children"][0]]
                end = bones[kid["children"][0]]
                v = target_local(M, end["index"])
                d = float(np.linalg.norm(v))
                dist[kid["index"]] = d
                s, c = cos_law(kid["pos"][0], d, end["pos"][0], b["bend"])
                M = M @ aim(v) @ rot_axis("z", s, c)
            if f & 0x200000:
                M = M @ trans(b["pos"])
                kid = bones[b["children"][0]]
                s, c = cos_law(b["pos"][0], kid["pos"][0], dist.get(i, 0.0), b["bend"])
                M = M @ rot_axis("z", s, -c)
        world[i] = M
        for k in b["children"]:
            walk(k, M)

    walk(0, np.eye(4))
    return world


def pose_passes(bones, curves, frame, wrists=None, flip=None, state=None, base=None, smooth=None,
                look=None, blend=None, held=False, lift=None, reach=None):
    """pose() as the game does it (rob_eval, 0x0C095300):
      rob_eval_ik       samples every bone's 16-bit angles A and position P; the
                        IK bones' solved rotations through mtx_to_euler
      rob_pose_mirror   with flip (mirror_tables): the pose mirrored
      rob_eval_matrix   rebuilds the matrices (W2) from A and P and takes the
                        IK end positions T (the 0x2000 bones); the root
                        (k_hara, 0x8000) blends here
      rob_bone_pos_blend  blends T
      rob_eval_output   solves the IK again toward T; the other rotations
                        blend here; the look-at
    The world matrices of gblctr and t_hara (no output transform) are
    rob_eval_matrix's.

    rob_eval_ik takes its lengths and rest offsets from bone_defs (0x0C18BF54,
    rig 0: rob_pose_eval passes it for every fighter) and only the flags from
    the fighter's own rig, so a fighter on another rig solves its IK angles
    with rig 0's proportions; the later passes use its own. base is rig 0's
    bones (default: bones).

    smooth = (table, w, y): the end-of-move blend toward a stance pose
    (rob +0x3A6 bit 3; tools/smooth.py), table bone -> matrix, w the weight
    (rob +0x3D4), y the ground frame's height (0; -0.6 when rob +0x20 has
    0x40000000 without 0x400). rsmooth_targets_set puts each table bone's
    translation in the ground frame (the root's matrix, its translation's y
    set to y); the root's translation, T and the rotations move toward the
    table by w. Only the plain case: the down and turned stances (+0x3A6
    0x5040 / 0x8000 / 0x400) turn the frame and rebuild bones 2 and 25.
    look = (point, fade): the head (bone 5) aims at the point (the rob's
    model space; rob_attack_reach: the opponent's head) clamped by
    look_clamp, blended in by fade (rob +0x4E0).
    blend = (keep, k, o): the motion-change blend (rob +0x3A6 bit 0, k =
    rob +0x3D0) from keep, the state of the last frame of the previous
    motion, placed at o, the new motion's root x / z at its first frame
    (root_offset; rob +0x3E0 / +0x3E8): root_mtx_blend_prev,
    bone_pos_blend_prev and the rotations by mtx_rot_lerp. Takes precedence
    over smooth, as in the game.

    state, a dict, receives A, P, T, W2, the stance positions S and "keep":
    what the next motion blends from (root_mtx_keep: the root without its
    x / z translation; bone_pos_keep: T less the root offset; bone_rot_keep:
    the output rotations). held: this motion's move keeps its root offset
    (state word rob +0x20 bit 4, from the move record's flags): then
    root_mtx_keep leaves the x / z translation and bone_pos_keep T as they are
    (checked live: Kage's KAGE_TA_KAMAE_VFX, flags 0x1018, into a jab).
    lift = (height, character id): reach flag 4 (rob +0x3F1), which
    rob_attack_reach keeps on for a fighter whose character record has a
    height offset (+0x24: JEFFRY 0.065, WOLF 0.05, PAI -0.02, AOI -0.04, SHUN
    -0.06) outside some states (lift_on): rob_eval_matrix moves t_hara up by
    it, so the body above the hips rises; for a negative one rob_eval_output
    raises the thigh IK aims (bones 27 / 34) by as much (SHUN 0.02 less,
    AOI 0.025 more) so the feet stay down.
    reach = (current, out): the attack reach (rob +0x3F0: current[5] at
    +0x24, out at +0x5C; rob_attack_reach eases them toward the opponent):
    rob_eval_matrix also moves t_hara up by current[0] x out, and
    rob_eval_output raises the IK aims of the thighs and shoulders by
    current[i] x out (REACH_AIM). Its limb targets (+0x60) are not
    emulated."""
    geo = base or bones

    def channels(b, names):
        ch = curves.get(b, {})
        return [motion_mod.sample(ch[n], frame) if n in ch else 0.0 for n in names]

    A, P, G, dist = {}, {}, [np.eye(4)], {}

    def target_local(M, b):
        w = G[0] @ np.array([*channels(b, ("ch0", "ch1", "ch2")), 1.0])
        return (np.linalg.inv(M) @ w)[:3]

    def sample(i, M):
        b = geo[i]
        f = bones[i]["flags"]
        if not f & 0x80:
            if f & 0x100:
                M = M @ trans(b["pos"])
            if f & 0x200:
                M = M @ rot_zyx(b["rot"])
            if f & 0x400:
                P[i] = channels(i, ("ch0", "ch1", "ch2"))
                M = M @ trans(P[i])
            if f & 0x800:
                A[i] = [ang16(a) for a in channels(i, ("ch3", "ch4", "ch5"))]
                M = M @ rot16(A[i])
            if f & 0x2000000:
                G[0] = M
        else:
            if f & 0x1000000:
                m = aim(target_local(M, b["children"][0]))
                A[i] = euler16(m)
                M = M @ m
            if f & 0x800000:
                kid = geo[b["children"][0]]
                end = geo[kid["children"][0]]
                v = target_local(M, end["index"])
                d = float(np.linalg.norm(v))
                dist[kid["index"]] = d
                s, c = cos_law(kid["pos"][0], d, end["pos"][0], b["bend"])
                m = aim(v) @ rot_axis("z", s, c)
                A[i] = euler16(m)
                M = M @ m
            if f & 0x200000:
                M = M @ trans(b["pos"])
                kid = geo[b["children"][0]]
                s, c = cos_law(b["pos"][0], kid["pos"][0], dist.get(i, 0.0), b["bend"])
                m = rot_axis("z", s, -c)
                A[i] = euler16(m)
                M = M @ m
        for k in b["children"]:
            sample(k, M)

    sample(0, np.eye(4))
    if flip is not None:
        mirror(flip, A, P)

    W2, T, S, root, off = {}, {}, {}, [np.eye(4)], [np.zeros(3)]
    w = smooth[1] if smooth else 0.0
    if blend:
        keep, k, o = blend
        o = np.array([o[0], 0.0, o[1]])

    def rebuild(i, M):
        b = bones[i]
        f = b["flags"]
        if f & 0x1980000:
            M = M @ rot16(A[i])
        if f & 0x200000:
            M = M @ trans(b["pos"]) @ rot16(A[i])
        if f & 0x100:
            M = M @ trans(b["pos"])
        if f & 0x200:
            M = M @ rot_zyx(b["rot"])
        if f & 0x400:
            M = M @ trans(P[i])
        if f & 0x800:
            M = M @ rot16(A[i])
        if f & 0x2000:
            T[i] = M[:3, 3].copy()
        if f & 0x4000:
            if lift:
                M = M @ trans([0.0, lift[0], 0.0])
            if reach:
                M = M @ trans([0.0, reach[0][0] * reach[1], 0.0])
            off[0] = np.array([M[0, 3], 0.0, M[2, 3]])
        if f & 0x8000:
            if blend:
                frm = trans(o) @ keep["root"]
                root[0] = rot_lerp(M, frm, k)
                root[0][:3, 3] = k * M[:3, 3] + (1 - k) * frm[:3, 3]
            elif smooth:
                F = M.copy()
                F[1, 3] = smooth[2]
                S.update({j: (F @ np.array([*m[:3, 3], 1.0]))[:3] for j, m in smooth[0].items()})
                M = M.copy()
                M[:3, 3] = w * S[i] + (1 - w) * M[:3, 3]
                root[0] = M
            else:
                root[0] = M
        W2[i] = M
        for j in b["children"]:
            rebuild(j, M)

    rebuild(0, np.eye(4))
    for i in T:
        if blend:
            T[i] = k * T[i] + (1 - k) * (o + keep["pos"][i])
        elif smooth:
            T[i] = w * S[i] + (1 - w) * T[i]

    world, dist, chest, rots = {}, {}, [np.eye(4)], {}

    def local(M, p):
        return (np.linalg.inv(M) @ np.array([*p, 1.0]))[:3]

    def output(i, M):
        b = bones[i]
        f = b["flags"]
        if f & 0x2004000:
            world[i] = W2[i]
        elif f & 0x8000:
            M = M @ root[0]
        elif f & 0x80:
            if f & 0x1000000:
                m = aim(local(M, T[b["children"][0]]))
                if i == 5 and look:
                    m = rot_lerp(aim(local(M, look_clamp(chest[0], look[0]))), m, look[1])
                M = M @ m
                if i == 3:
                    chest[0] = M
            if f & 0x800000:
                kid = bones[b["children"][0]]
                end = bones[kid["children"][0]]
                t = T[end["index"]]
                if lift and lift[0] < 0 and i in (27, 34):
                    t = t + np.array([0.0, -lift[0] + THIGH_AIM.get(lift[1], 0.0), 0.0])
                if reach and i in REACH_AIM:
                    t = t + np.array([0.0, reach[0][REACH_AIM[i]] * reach[1], 0.0])
                v = local(M, t)
                d = float(np.linalg.norm(v))
                dist[kid["index"]] = d
                s, c = cos_law(kid["pos"][0], d, end["pos"][0], b["bend"])
                M = M @ aim(v) @ rot_axis("z", s, c)
            if f & 0x200000:
                M = M @ trans(b["pos"])
                kid = bones[b["children"][0]]
                s, c = cos_law(b["pos"][0], kid["pos"][0], dist.get(i, 0.0), b["bend"])
                M = M @ rot_axis("z", s, -c)
        else:
            if f & 0x100:
                M = M @ trans(b["pos"])
            if f & 0x200:
                M = M @ rot_zyx(b["rot"])
            if f & 0x400:
                M = M @ trans(P[i])
            if f & 0x800:
                R = rot16(A[i])
                if f & 0xC000000 and wrists is not None:
                    wrists[i] = wrist(M, R)
                if not f & 0x2008000:
                    if blend:
                        R = rot_lerp(R, keep["rot"][i], k)
                    elif smooth:
                        R = rot_lerp(smooth[0][i], R, w)
                    rots[i] = R
                M = M @ R
        world.setdefault(i, M)
        for j in b["children"]:
            output(j, M)

    output(0, np.eye(4))
    if state is not None:
        kept = root[0].copy()
        if not held:
            kept[0, 3] = kept[2, 3] = 0.0
        state.update(A=A, P=P, T=T, W2=W2, S=S,
                     keep={"root": kept, "pos": {i: p if held else p - off[0] for i, p in T.items()}, "rot": rots})
    return world


def root_offset(bones, curves, flip=None, lift=None, reach=None):
    """The motion's root x / z at its first frame (rob_base_mtx_eval at time 0:
    gblctr's translation and rotation, then t_hara's translation; the
    motion-change blend places the previous pose there)."""
    st = {}
    pose_passes(bones, curves, 0.0, flip=flip, state=st, lift=lift, reach=reach)
    t = next(i for i, b in enumerate(bones) if b["flags"] & 0x4000)
    return st["W2"][t][0, 3], st["W2"][t][2, 3]


def skin(obj, node_off):
    """Skin data of an envelope node at node_off in obj_ (envelope_update).

    node +0x80: groups, each [n, 0, n x (float weight, slot, slot * 0x40, 0)]
    then vertex records [c, 0, float normal[3], float position[3], float u, v,
    c x (byte offset of the vertex in the model block, header flags)] ended
    by 0, and [0, next n]; n == 0 ends. Type 0x12 (envelope_skin, 0x0C0E8200)
    as above; type 0x10 (envelope_skin_nouv, 0x0C0EA5E0) has no u, v in the
    vertex records and leaves the model's UVs as they are (uv is None).
    node +0x88: 162 bind matrices (0x40 bytes, row-vector layout), the
    inverse of each slot's T-pose matrix.
    """
    w = lambda o: struct.unpack_from("<I", obj, o)[0]
    fl = lambda o: struct.unpack_from("<f", obj, o)[0]
    kind = w(node_off + 0x3C)
    if kind not in (0x10, 0x12):
        raise ValueError(f"node type {kind:#x} is not an envelope")
    rec = 40 if kind == 0x12 else 32
    p = w(node_off + 0x80)
    groups = []
    n = w(p)
    p += 4
    while n:
        p += 4
        infl = []
        for _ in range(n):
            infl.append((w(p + 4), fl(p)))
            p += 16
        verts = []
        while w(p):
            c = w(p)
            nrm = struct.unpack_from("<3f", obj, p + 8)
            pos = struct.unpack_from("<3f", obj, p + 20)
            uv = struct.unpack_from("<2f", obj, p + 32) if rec == 40 else None
            p += rec
            outs = [(w(p + 8 * k), w(p + 8 * k + 4)) for k in range(c)]
            p += 8 * c
            verts.append({"normal": nrm, "pos": pos, "uv": uv, "outputs": outs})
        groups.append({"influences": infl, "vertices": verts})
        n = w(p + 8)
        p += 12
    base = w(node_off + 0x88)
    # 162 for a 0x15 body; a 0x13 body's table is shorter (123) and can end
    # the file, so read only what is there
    n = min(0xA2, (len(obj) - base) // 0x40)
    binds = [np.array(struct.unpack_from("<16f", obj, base + 0x40 * i)).reshape(4, 4).T for i in range(n)]
    binds += [np.eye(4)] * (0xA2 - n)
    return groups, binds


def dump(prog, dest):
    import os
    os.makedirs(dest, exist_ok=True)
    chars = characters(prog)
    rigs = [rig(prog, i) for i in range(13)]
    with open(os.path.join(dest, "skeletons.json"), "w") as f:
        json.dump({"characters": chars,
                   "rigs": [{"index": i, "users": [c["name"] for c in chars if c["rig"] == i], "bones": r}
                            for i, r in enumerate(rigs)]}, f, indent=1)
    print(f"{len(chars)} characters, {len(rigs)} rigs of {len(rigs[0])} bones -> {dest}/skeletons.json",
          file=sys.stderr)


def main():
    if sys.argv[1] == "dump":
        dump(open(sys.argv[2], "rb").read(), sys.argv[3])
        return
    prog = open(sys.argv[1], "rb").read()
    data = json.load(open(sys.argv[2]))
    m = next(x for x in data["motions"] if x["name"] == sys.argv[3])
    frame = int(sys.argv[4])
    bones = rig(prog, int(sys.argv[5]) if len(sys.argv) > 5 else 0)
    curves = {int(k): v for k, v in m["bones"].items()}
    world = pose(bones, curves, frame)
    for i in sorted(world):
        p = world[i][:3, 3]
        print(f"{i:2} {bones[i]['name']:12} {p[0]:7.3f} {p[1]:7.3f} {p[2]:7.3f}")


if __name__ == "__main__":
    main()
