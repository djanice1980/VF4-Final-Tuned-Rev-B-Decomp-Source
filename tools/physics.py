#!/usr/bin/env python3
"""Swing bones (the ex_* "j_ex" chains): data and simulation, as the game runs them.

Read from main.bin (libpoly / rob code, 0x0C0D2120-0x0C0DCA00; names are
placeholders in the decompile):
  chains     a body node's +0x80 list (pol_uti relocates it): one command
             list per chain, (tag, record) pairs:
               1  root  { u32 parent slot, float offset[3], s16 rx, ry, rz }
               3  joint { float length, u32 ?, u32 slot, u32 param index }
               0  last joint (same record)
  params     100 entries of 0x18 bytes per fighter (j_ex_00 ...): { u16 type,
             s16 limit, float stiffness, float damping, float gravity, s16
             init ry, s16 init rz, shape *collision }, from a per-character
             table (the switch in 0x0C0D24C0, CHARACTER below) or the item
             record of an item body (chara_item_table, item_records below),
             dense (100 entries) or sparse ({index, ..., collision} lists
             ending in -1); every other entry is 0
  cross      some costumes / items tie joints of neighbouring chains with a
             maximum distance (0x0C0D2460, CROSS below)
  collision  28-byte shapes { s16 type, s16 slot, float[6] } ending in type 0,
             in the slot's frame: 1 sphere (centre, radius), 2 / 3 / 4
             capsule along z / x / y (centre of the other two axes, radius,
             axis range); a point inside is pushed to the surface (0x0C0DC740)
Simulation, per 60 Hz game frame after the rig (0x0C0D3640, 0x0C0D9240,
0x0C0D9440), current = the parent slot's matrix:
  root      move to parent x offset, rotate Z, Y, X by the record's angles
  joint     prev = tip; tip += wind + velocity; pull toward the next joint and the
            previous one by gravity (0x0C0DC61C) and keep within the length
            of both; tip += stiffness x current X axis; keep within length of
            the previous joint and of a cross-linked one; collide, floor
            y >= 0.01; aim: type 0 yaw + pitch (Y then Z), 1 Z then Y, 2 Y
            only, 3 Z only, each clamped to +-limit when limit != 0; store the
            slot; step out by the length; tip = there; velocity = (tip - prev)
            / (damping + 1 + air_reg), y minus (gravity + param gravity)
  reset     each joint hangs along its init angles plus gravity, then two
            settling steps with zero velocity (0x0C0D8FE0, DAT_0c2ff664 = 2)
Constants: gravity 0x3B443E72 per frame (set at boot), air_reg / strength /
yoko 0 (debug). Wind: the stage's (Wind below), else 0.
"""
import math
import struct

import numpy as np

BASE = 0x0C020000
GRAVITY = struct.unpack("<f", struct.pack("<I", 0x3B443E72))[0]
AIR_REG = 0.0
SETTLE = 2
FLOOR = 0.0

# (character id, costume) -> (kind, params, collision list); 0x0C0D24C0
CHARA_ITEMS = 0x0C2DD3D8  # chara_item_table
EDIT_PARAMS = 0x0C2DD868  # the debug edit parameters (rot_type ...), 0.02 stiffness each
DEFAULT_COLL = 0x0C2DE1C8
CHARACTER = {
    (0, 0): ("sparse", 0x0C2DE240, None), (0, 1): ("dense", 0x0C2DE9AC, DEFAULT_COLL),
    (1, 0): ("dense", 0x0C2DF0D8, 0x0C2DF070), (1, 1): ("dense", 0x0C2DF794, 0x0C2DF72C),
    (2, 0): ("sparse", 0x0C2DFE04, None), (2, 1): ("dense", 0x0C2E0500, DEFAULT_COLL),
    (3, 0): ("dense", 0x0C2EA1D8, 0x0C2EA138), (3, 1): ("dense", 0x0C2EA914, 0x0C2EA89C),
    (4, 0): ("dense", 0x0C2E1AF8, 0x0C2E1AD8), (4, 1): ("dense", 0x0C2E2158, 0x0C2E2130),
    (5, 0): ("dense", 0x0C2E2758, DEFAULT_COLL), (5, 1): ("dense", 0x0C2E2E14, 0x0C2E2DAC),
    (6, 0): ("dense", 0x0C2E3414, DEFAULT_COLL), (6, 1): ("dense", 0x0C2E3A14, DEFAULT_COLL),
    (7, 0): ("dense", 0x0C2E4014, DEFAULT_COLL), (7, 1): ("dense", 0x0C2E4614, DEFAULT_COLL),
    (8, 0): ("dense", 0x0C2E4E3C, 0x0C2E4DD4), (8, 1): ("dense", 0x0C2E543C, DEFAULT_COLL),
    (9, 0): ("dense", 0x0C2E5B10, 0x0C2E5A90), (9, 1): ("dense", 0x0C2E6130, 0x0C2E6110),
    (10, 0): ("dense", 0x0C2E68AC, 0x0C2E6864), (10, 1): ("dense", 0x0C2E708C, 0x0C2E6FC4),
    (11, 0): ("dense", 0x0C2E79E4, 0x0C2E78F4), (11, 1): ("dense", 0x0C2E80A4, 0x0C2E801C),
    (12, 0): ("dense", 0x0C2E86A4, DEFAULT_COLL), (12, 1): ("dense", 0x0C2E8D78, 0x0C2E8CF8),
    (13, 0): ("dense", 0x0C2E9378, DEFAULT_COLL), (13, 1): ("dense", 0x0C2E9978, DEFAULT_COLL),
    (14, 0): ("sparse", 0x0C2ECE78, None), (14, 1): ("sparse", 0x0C2ED97C, None),
    (15, 0): ("dense", 0x0C2EB05C, 0x0C2EAF4C), (15, 1): ("sparse", 0x0C2EB95C, None),
}
# model id -> (distance global, [(joint, linked joint)]): 0x0C0D24C0, 0x0C0D3220
CROSS = {
    0x580000: (0x0C2EDAA8, [(0x1B, 0x19), (0x15, 0x17), (0x15, 0x1B)]),
    0x590000: (0x0C2EDAAC, [(0x21, 0x23), (0x27, 0x25), (0x27, 0x21)]),
    0x420000: (0x0C2EDAA4, [(4, 1), (5, 2), (7, 1), (8, 2), (10, 4), (11, 5), (13, 7), (14, 8)]),
    0x2B0000: (0x0C2EDA94, [(0xD, 1), (0xE, 2), (0xF, 3), (0x10, 4), (0x11, 5), (7, 1), (8, 2), (9, 3),
                            (10, 4), (11, 5), (0x1F, 0x13), (0x20, 0x14), (0x21, 0x15), (0x22, 0x16),
                            (0x23, 0x17), (0x19, 0x13), (0x1A, 0x14), (0x1B, 0x15), (0x1C, 0x16),
                            (0x1D, 0x17)]),
    0x2E0001: (0x0C2EDA98, [(0x12, 0x1A), (0x13, 0x1A), (0x14, 0x1B), (0x16, 0x1A), (0x17, 0x1B),
                            (0x18, 0x1C), (5, 9), (6, 10), (7, 0xB), (1, 9), (2, 9), (3, 10)]),
    0x2F0001: (0x0C2EDA9C, [(0xB, 0x10), (0xC, 0x11), (0xD, 0x12), (0xE, 0x13), (6, 1), (7, 2), (8, 3),
                            (9, 4), (0x10, 1), (2, 0x11), (0x12, 3), (4, 0x13)]),
    0x650004: (0x0C2EDAB0, [(0x1A, 0x16), (0x1B, 0x17), (0x1E, 0x1A), (0x1F, 0x1B), (0x22, 0x1E),
                            (0x23, 0x1F), (0x26, 0x22), (0x27, 0x23), (0x2A, 0x26), (0x2B, 0x27),
                            (0x12, 0x16), (0x13, 0x17), (0xE, 0x12), (0xF, 0x13), (10, 0xE), (0xB, 0xF),
                            (6, 10), (7, 0xB), (2, 6), (3, 7)]),
    0x500008: (0x0C2EDAA0, [(0x10, 10), (0x11, 0xB), (0xD, 10), (0xE, 0xB), (4, 7), (5, 8), (1, 7),
                            (2, 8)]),
}


class Prog:
    def __init__(self, prog):
        self.p = prog

    def u(self, a):
        return struct.unpack_from("<I", self.p, a - BASE)[0]

    def f(self, a):
        return struct.unpack_from("<f", self.p, a - BASE)[0]

    def s(self, a):
        return struct.unpack_from("<h", self.p, a - BASE)[0]


def item_records(P):
    """model id -> (flags, params, collision, cloth params) of every model item
    (kind 1) in chara_item_table (0x0C2DD3D8, 16 bytes per character: { s16
    count, 12-byte entries { slot, kind[2], -, data[2] } by costume, own
    items[2] } with 4 x { kind, -, -, -, data } own items per costume), the
    records rob_items_setup puts in the rob's item slots: { u32 flags, model
    id, params, collision, cloth params }."""
    out = {}
    top = BASE + len(P.p)
    for ch in range(20):
        row = CHARA_ITEMS + 16 * ch
        datas = []
        ent = P.u(row + 4)
        for i in range(P.s(row) if ent else 0):
            e = ent + 12 * i
            datas += [P.u(e + 4 + 4 * c) for c in range(2) if P.p[e + 1 + c - BASE] == 1]
        for c in range(2):
            own = P.u(row + 8 + 4 * c)
            datas += [P.u(own + 8 * i + 4) for i in range(4 if own else 0) if P.p[own + 8 * i - BASE] == 1]
        for d in datas:
            if BASE <= d < top and P.u(d + 4) != 0xFFFFFFFF:
                out.setdefault(P.u(d + 4), (P.u(d) & 0xFF, P.u(d + 8), P.u(d + 12), P.u(d + 0x10)))
    return out


def item_spec(rec):
    """An item record -> params_table's (kind, table, collision), as
    rob_osage_items_init picks: flag bit 1 or no table -> dense
    (osage_chains_add_params; no table: the debug edit table EDIT_PARAMS),
    else a sparse list (osage_chains_add_list), which takes no collision list."""
    flags, params, coll = rec[:3]
    if flags & 2 or not params:
        return ("dense", params or EDIT_PARAMS, coll)
    return ("sparse", params, None)


def shapes(P, a):
    out = []
    while a and P.s(a) != 0:
        out.append({"type": P.s(a), "slot": P.s(a + 2), "v": [P.f(a + 4 + 4 * k) for k in range(6)]})
        a += 28
    return out


def params_table(P, kind, table, coll):
    """index -> param dict (0x0C0D2120 dense: all 100 / 0x0C0D22E0 sparse: the
    listed entries only)."""
    zero = {"type": 0, "limit": 0, "stiffness": 0.0, "damping": 0.0, "gravity": 0.0, "ry": 0, "rz": 0,
            "collision": []}
    out = {i: dict(zero) for i in range(100)}

    def entry(a):
        tl = P.u(a)
        return {"type": tl & 0xFFFF, "limit": (tl >> 16) - 0x10000 if tl >> 31 else tl >> 16,
                "stiffness": P.f(a + 4), "damping": P.f(a + 8), "gravity": P.f(a + 0xC),
                "ry": P.s(a + 0x10), "rz": P.s(a + 0x12), "collision": []}
    if kind == "dense":
        for i in range(100):
            out[i] = entry(table + 0x18 * i)
        a = coll
        while a and P.u(a) < 0x80000000:
            out[P.u(a)]["collision"] = shapes(P, P.u(a + 4))
            a += 8
    else:
        out = {}
        a = table
        while P.u(a) < 0x80000000:
            i = P.u(a)
            out[i] = entry(a + 4)
            out[i]["collision"] = shapes(P, P.u(a + 0x18))
            a += 28
    return out


def chains_of(obj, node_off):
    """Chains of a body node (its +0x80 list), each {root, joints}."""
    u = lambda x: struct.unpack_from("<I", obj, x)[0]
    f = lambda x: struct.unpack_from("<f", obj, x)[0]
    s = lambda x: struct.unpack_from("<h", obj, x)[0]
    out = []
    lst = u(node_off + 0x80)
    if not lst:
        return out
    while u(lst):
        p, ch = u(lst), {"root": None, "joints": []}
        while True:
            tag, rec = u(p), u(p + 4)
            if tag == 1:
                ch["root"] = {"parent": u(rec), "offset": [f(rec + 4 + 4 * k) for k in range(3)],
                              "rx": s(rec + 0x10), "ry": s(rec + 0x12), "rz": s(rec + 0x14)}
            elif tag in (0, 3, 4):
                ch["joints"].append({"length": f(rec), "slot": u(rec + 8), "param": u(rec + 0xC)})
            if tag in (0, 4):
                break
            p += 8
        out.append(ch)
        lst += 4
    return out


# -- matrix helpers (column vectors; the game's are row vectors, same result)
def ang(a16):
    return (a16 & 0xFFFF) * 2 * math.pi / 65536


def rot(axis, a16):
    a = ang(a16)
    c, s = math.cos(a), math.sin(a)
    m = np.eye(4)
    i, j = {"x": (1, 2), "y": (2, 0), "z": (0, 1)}[axis]
    m[i, i], m[i, j], m[j, i], m[j, j] = c, -s, s, c
    return m


F32 = np.float32
ATAN_PI, ATAN_HALF, ATAN_SCALE = F32(3.1415927410125732), F32(1.5707963705062866), F32(10430.3779296875)
ATAN_EPS = F32(1.1920929e-07)


def atan_kernel(x):
    """0x0C0F1B40's atan_kernel: a rational approximation, in float32."""
    x = F32(x)
    x2 = x * x
    n = x2 * F32(0.17441538) + F32(5.731022)
    d = x2 + F32(11.554501)
    n = n * x2 + F32(22.939714)
    d = d * x2 + F32(29.776684)
    n = n * x2 + F32(20.51093)
    d = d * x2 + F32(20.51093)
    return (x * n) / d


def atan2s(y, x):
    """atan2_s16 (0x0C0F1B40): the octant by y against x and -x, atan_kernel of
    y / x or x / y, scaled by 65536 / 2pi (atan_consts) and truncated to s16."""
    y, x = F32(y), F32(x)
    if y <= x:
        if -y <= x:
            r = F32(0) if -ATAN_EPS <= x <= ATAN_EPS else atan_kernel(y / x)
        else:
            r = -ATAN_HALF if -ATAN_EPS <= y <= ATAN_EPS else -ATAN_HALF - atan_kernel(x / y)
    elif -y <= x:
        r = ATAN_HALF if -ATAN_EPS <= y <= ATAN_EPS else ATAN_HALF - atan_kernel(x / y)
    elif -ATAN_EPS <= x <= ATAN_EPS:
        r = ATAN_PI
    elif y > 0:
        r = ATAN_PI + atan_kernel(y / x)
    else:
        r = atan_kernel(y / x) - ATAN_PI
    v = int(ATAN_SCALE * F32(r)) & 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def clamp_len(p, anchor, length):
    """0x0C0DC6E0: keep p within length of anchor."""
    v = p - anchor
    d2 = float(v @ v)
    if d2 > length * length:
        return anchor + v * (length / math.sqrt(d2))
    return p


def pull(p, prev, anchor, length, g):
    """0x0C0DC61C: move p by (anchor - prev) x (-(anchor - prev).y x g / length), then clamp."""
    d = anchor - prev
    p = p + d * (-d[1] * g / length)
    return clamp_len(p, anchor, length)


def collide(p, shp, slots):
    for sh in shp:
        M = slots.get(sh["slot"])
        if M is None:
            continue
        q = np.linalg.inv(M) @ np.array([*p, 1.0])
        v, t = sh["v"], sh["type"]
        if t == 1:
            c = np.array(v[0:3])
            d = q[:3] - c
            if d @ d < v[3] * v[3] and d @ d > 0:
                q[:3] = c + d * (v[3] / math.sqrt(d @ d))
        elif t in (2, 3, 4):
            perm = {2: (0, 1, 2), 3: (1, 2, 0), 4: (0, 2, 1)}[t]
            cx, cy = {2: (v[0], v[1]), 3: (v[1], v[2]), 4: (v[0], v[2])}[t]
            r, lo, hi = v[3], v[4], v[5]
            a, b, z = q[perm[0]] - cx, q[perm[1]] - cy, q[perm[2]]
            if z < lo or z > hi:
                cap = lo if z < lo else hi
                dz = z - cap
                d2 = a * a + b * b + dz * dz
                if r * r > d2 > 0:
                    k = r / math.sqrt(d2)
                    a, b, dz = a * k, b * k, dz * k
                z = cap + dz
            else:
                d2 = a * a + b * b
                if r * r > d2 > 0:
                    k = r / math.sqrt(d2)
                    a, b = a * k, b * k
            q[perm[0]], q[perm[1]], q[perm[2]] = a + cx, b + cy, z
        p = (M @ q)[:3]
    if p[1] < FLOOR + 0.01:
        p = p.copy()
        p[1] = FLOOR + 0.01
    return p


def aim(kind, v, limit):
    """0x0C0D9640 -> rotation matrix (applied after the current one)."""
    def cl(a):
        if limit:
            a = max(-limit, min(limit, a))
        return a
    x, y, z = v
    if kind == 0:
        return rot("y", cl(atan2s(-z, x))) @ rot("z", cl(atan2s(y, math.hypot(x, z))))
    if kind == 1:
        return rot("z", cl(atan2s(y, x))) @ rot("y", cl(atan2s(-z, math.hypot(y, x))))
    if kind == 2:
        return rot("y", cl(atan2s(-z, x)))
    if kind == 3:
        return rot("z", cl(atan2s(y, x)))
    return np.eye(4)


# ---------------------------------------------------------------------------
# Stage wind (stage_wind_set 0x0C0DD4E0, wind_update 0x0C0DD580): one 0x20 row
# per stage at 0x0C306D18 { float bias, cycle0, ratio0, cycle1, ratio1, cycle2,
# ratio2; s16 xang, yang }; stage_wind_set clears waves 2-7, so waves 0 and 1
# blow (ROME 0, NYC 1, CAS 2, YAM 4, GARDEN 17 and HBR 19 have any). Once a
# game frame, before the chains and the cloth (osage_calc_all): per wave angle =
# cycle x 0.104719736 + phase (a cycle of 1 is a swing a second), z = bias +
# sum (sin(angle) + 1) x ratio, phase = angle; the force (0x0C2FF5BC) is
# Ry(yang) Rx(xang) (0, 0, z), added to every chain tip (with the settling
# steps of a reset) and every cloth particle, beside the velocity. The phases
# run on from boot (stage_wind_set leaves them), so FRAME picks the start.
# The game simulates in world space and the tools in the fighter's placed
# frame (translate by rob +0x84, turn by minus the heading +0x0C), where only
# the wind is not the same: it comes in turned by the heading.
# ---------------------------------------------------------------------------
WIND_TABLE = 0x0C306D18
WIND_STEP = np.float32(0.104719736)


class Wind:
    def __init__(self, prog, stage, frame=0, heading=0):
        """stage: stage number; frame: wind updates already run; heading: the
        fighter's (rob +0x0C, 16-bit angle), for the force in its frame."""
        P = Prog(prog)
        a = WIND_TABLE + 0x20 * stage
        self.bias = np.float32(P.f(a))
        self.waves = [(np.float32(P.f(a + 4)), np.float32(P.f(a + 8))),
                      (np.float32(P.f(a + 12)), np.float32(P.f(a + 16)))]
        self.R = (rot("y", heading + P.s(a + 0x1E)) @ rot("x", P.s(a + 0x1C)))[:3, :3]
        self.phase = [np.float32(0.0)] * 2
        self.vec = np.zeros(3)
        for _ in range(frame):
            self.update()

    def update(self):
        """One wind_update (float32 as the game keeps it) -> the force."""
        z = self.bias
        for i, (cycle, ratio) in enumerate(self.waves):
            a = np.float32(cycle * WIND_STEP + self.phase[i])
            s16 = int(a * np.float32(65536.0) / np.float32(6.2831855) + np.float32(0.5)) & 0xFFFF
            z = np.float32((math.sin(ang(s16)) + 1.0) * ratio + z)
            self.phase[i] = a
        self.vec = self.R @ np.array([0.0, 0.0, float(z)])
        return self.vec


class SwingBones:
    """All chains of a character's bodies, simulated over the shared 162-slot table."""

    def __init__(self, prog, bodies):
        """bodies: [(obj bytes, body node offset, model id, (kind, table, coll) or None)]"""
        self.P = Prog(prog)
        self.chains = []
        # one 100-entry table per fighter, zero to start: each body, in order
        # (main body, then items), overwrites the entries its own chains use
        # (dense, 0x0C0D2120) or the entries its list names, used or not
        # (sparse, 0x0C0D22E0)
        self.params = {i: None for i in range(100)}
        for obj, off, mid, spec in bodies:
            chains = chains_of(obj, off)
            if not chains:
                continue
            table = params_table(self.P, *spec) if spec else {i: None for i in range(100)}
            if spec and spec[0] == "sparse":
                self.params.update(table)
            else:
                for c in chains:
                    for j in c["joints"]:
                        self.params[j["param"]] = table.get(j["param"])
            recs = []
            for c in chains:
                c["params"] = self.params
                c["root_rec"] = {"tip": np.zeros(3)}
                recs.append(c["root_rec"])
                for j in c["joints"]:
                    j["rec"] = {"tip": np.zeros(3), "vel": np.zeros(3), "link": None, "dist": 0.0}
                    recs.append(j["rec"])
                self.chains.append(c)
            if mid in CROSS:
                dist_at, pairs = CROSS[mid]
                dist = self.P.f(dist_at)
                for a, b in pairs:
                    if a < len(recs) and b < len(recs):
                        recs[a]["link"], recs[a]["dist"] = recs[b], dist

    def _param(self, c, j):
        p = c["params"].get(j["param"])
        return p or {"type": 0, "limit": 0, "stiffness": 0.0, "damping": 0.0, "gravity": 0.0, "ry": 0,
                     "rz": 0, "collision": []}

    def _root(self, c, slots):
        r = c["root"]
        M = slots[r["parent"]].copy()
        base = (M @ np.array([*r["offset"], 1.0]))[:3]
        M[:3, 3] = base
        M = M @ rot("z", r["rz"]) @ rot("y", r["ry"]) @ rot("x", r["rx"])
        c["root_rec"]["tip"] = base
        return M

    def reset(self, slots, wind=None):
        for c in self.chains:
            if c["root"]["parent"] not in slots:
                continue
            M = self._root(c, slots)
            prev = c["root_rec"]
            for j in c["joints"]:
                p, rec = self._param(c, j), j["rec"]
                d = (M @ rot("z", p["rz"]) @ rot("y", p["ry"]))[:3, 0].copy()
                d[1] -= GRAVITY + p["gravity"]
                d = d * (j["length"] / max(np.linalg.norm(d), 1e-12))
                tip = collide(prev["tip"] + d, p["collision"], slots) if p["collision"] else prev["tip"] + d
                rec["vel"] = np.zeros(3)
                M = M @ aim(p["type"], (np.linalg.inv(M) @ np.array([*tip, 1.0]))[:3], p["limit"])
                slots[j["slot"]] = M.copy()
                M = M @ _trans(j["length"])
                rec["tip"] = M[:3, 3].copy()
                prev = rec
        for _ in range(SETTLE):
            self.step(slots, zero_velocity=True, wind=wind)

    def step(self, slots, zero_velocity=False, wind=None):
        """wind: this frame's force (Wind.update), else none."""
        w = np.zeros(3) if wind is None else wind
        for c in self.chains:
            if c["root"]["parent"] not in slots:
                continue
            M = self._root(c, slots)
            prev_rec = c["root_rec"]
            js = c["joints"]
            for k, j in enumerate(js):
                p, rec = self._param(c, j), j["rec"]
                damp = 1.0 / (p["damping"] + 1.0 + AIR_REG)
                prev = rec["tip"].copy()
                tip = rec["tip"] + w + rec["vel"]
                if k + 1 < len(js):
                    tip = pull(tip, prev, js[k + 1]["rec"]["tip"], j["length"], GRAVITY)
                tip = pull(tip, prev, prev_rec["tip"], j["length"], GRAVITY)
                tip = tip + M[:3, :3] @ np.array([p["stiffness"], 0.0, 0.0])
                tip = clamp_len(tip, prev_rec["tip"], j["length"])
                if rec["link"] is not None:
                    tip = clamp_len(tip, rec["link"]["tip"], rec["dist"])
                if p["collision"]:
                    tip = collide(tip, p["collision"], slots)
                M = M @ aim(p["type"], (np.linalg.inv(M) @ np.array([*tip, 1.0]))[:3], p["limit"])
                slots[j["slot"]] = M.copy()
                M = M @ _trans(j["length"])
                rec["tip"] = M[:3, 3].copy()
                v = (rec["tip"] - prev) * damp
                v[1] -= GRAVITY + p["gravity"]
                rec["vel"] = np.zeros(3) if zero_velocity else v
                prev_rec = rec
        return slots


def _trans(x):
    m = np.eye(4)
    m[0, 3] = x
    return m


# ---------------------------------------------------------------------------
# Cloth: type 0x0F nodes (headband tails, cloth panels), 0x0C0DAEC0-0x0C0DCDA0
#
#   cloth data at file + node+0x80: u32 cols, rows, ?, anchors, particles, ?,
#     map A, map B, bind matrices (offsets in the file)
#   anchor (one per column, row 0) 0x98: 4 x { u32 slot (0 ends), float
#     weight, u32 bind index, ? }, float pos[3] @+0x80, dir[3] @+0x8C
#   particle (cols x rows) 0x50: u32 flags (bit 1 no spring below, 2 none
#     left, 3 none right), pos @+4, dir / normal @+0x10, velocity @+0x1C,
#     rest pos @+0x28, force dir @+0x34, spring lengths up / below / left /
#     right @+0x40
#   maps: per particle { u32 count, count x (vertex offset in the model
#     block, header flags) }, ending in 0: the vertices that particle drives
#     (map A / B are the two faces; one gets the normal, one its negation)
#   params, one 0x18 record per cloth node of a body's +0x8C list: { u32 map
#     order, float strength, float damping, float sideways, ?, shape
#     *collision }; per character (0x0C0D87A0, CLOTH below) or from the item
#     record's +0x10; the default list is 0x0C2FEE88
# Step (0x0C0DB4E0, 0x0C0DB780, the assembly 0x0C0DC520 that cloth_asm_flags
# 3 picks over the C twin 0x0C0DB920): row 0 follows its anchors
# (sum(weight x slot x bind) applied to pos / dir, velocity = the move), the
# last anchor's matrix staying current; each later row, from a copy: p = pos +
# strength x (current matrix x force dir) + velocity + wind; then toward the
# particle below, right, left (no sideways add in the assembly) and above in
# that order, each link moving p by (n - pos) x (-(n - pos).y x gravity /
# length) and keeping it within the length (pull); collide; floor;
# velocity = (p - pos) / (damping + 1 + air_reg)
# minus gravity. Reset (0x0C0DB040): row 0 from the anchors, the rest at the
# last anchor's matrix x rest pos, zero velocity.
# ---------------------------------------------------------------------------
DEFAULT_CLOTH = 0x0C2FEE88
CLOTH = {(2, 0): 0x0C2FF124, (2, 1): 0x0C2FF1F8, (3, 0): 0x0C2FF480, (3, 1): 0x0C2FF570,
         (0, 0): DEFAULT_CLOTH, (0, 1): DEFAULT_CLOTH, (1, 0): DEFAULT_CLOTH, (1, 1): DEFAULT_CLOTH,
         (5, 0): DEFAULT_CLOTH, (7, 0): DEFAULT_CLOTH, (11, 0): 0x0C2FF2D0, (11, 1): 0x0C2FF370,
         (14, 0): 0x0C2FEFA8, (14, 1): 0x0C2FEFA8,
         (15, 0): 0x0C2FEFA8, (15, 1): 0x0C2FEFA8}  # MUE runs 0x0C2FEF60 then 0x0C2FEFA8 on the same nodes


def cloth_params(P, a):
    if not a:
        a = DEFAULT_CLOTH
    return {"order": P.u(a), "strength": P.f(a + 4), "damping": P.f(a + 8), "sideways": P.f(a + 0xC),
            "collision": shapes(P, P.u(a + 0x14))}


class Cloth:
    def __init__(self, P, obj, node_off, params):
        u = lambda x: struct.unpack_from("<I", obj, x)[0]
        f3 = lambda x: np.array(struct.unpack_from("<3f", obj, x))
        d = u(node_off + 0x80)
        h = [u(d + 4 * k) for k in range(10)]
        self.cols, self.rows = h[0], h[1]
        self.params = params
        self.anchors = []
        for c in range(self.cols):
            a = h[3] + 0x98 * c
            infl = []
            for i in range(4):
                slot = u(a + 16 * i)
                if slot == 0 or slot >= 0x80000000:
                    break
                infl.append((slot, struct.unpack_from("<f", obj, a + 16 * i + 4)[0], u(a + 16 * i + 8)))
            self.anchors.append({"infl": infl, "pos": f3(a + 0x80), "dir": f3(a + 0x8C)})
        nb = 1 + max((bi for an in self.anchors for _, _, bi in an["infl"]), default=0)
        self.binds = [np.array(struct.unpack_from("<16f", obj, h[8] + 0x40 * i)).reshape(4, 4).T for i in range(nb)]
        n = self.cols * self.rows
        self.flags, self.rest, self.force, self.lens = [], [], [], []
        for k in range(n):
            a = h[4] + 0x50 * k
            self.flags.append(u(a))
            self.rest.append(f3(a + 0x28))
            self.force.append(f3(a + 0x34))
            self.lens.append(struct.unpack_from("<4f", obj, a + 0x40))
        self.maps = []
        for m in (h[6], h[7]):
            out, p = [], m
            while m and u(p):
                c = u(p)
                out.append([(u(p + 4 + 8 * j), u(p + 8 + 8 * j)) for j in range(c)])
                p += 4 + 8 * c
            self.maps.append(out)
        self.pos = np.zeros((n, 3))
        self.vel = np.zeros((n, 3))
        self.dir = np.zeros((self.cols, 3))

    def _anchor(self, c, slots):
        an = self.anchors[c]
        M = sum(w * slots[s] @ self.binds[bi] for s, w, bi in an["infl"] if s in slots)
        if isinstance(M, int):
            return None
        return M

    def reset(self, slots):
        last = None
        for c in range(self.cols):
            M = self._anchor(c, slots)
            if M is None:
                continue
            last = M
            self.pos[c] = (M @ np.array([*self.anchors[c]["pos"], 1.0]))[:3]
            self.dir[c] = M[:3, :3] @ self.anchors[c]["dir"]
        for k in range(self.cols, self.cols * self.rows):
            p = (last @ np.array([*self.rest[k], 1.0]))[:3] if last is not None else self.rest[k]
            if p[1] < FLOOR + 0.01:
                p[1] = FLOOR + 0.01
            self.pos[k] = p
        self.vel[:] = 0

    def step(self, slots, wind=None):
        pr = self.params
        w = np.zeros(3) if wind is None else wind
        damp = 1.0 / (pr["damping"] + 1.0 + AIR_REG)
        last = np.eye(4)
        for c in range(self.cols):
            M = self._anchor(c, slots)
            if M is None:
                continue
            last = M
            new = (M @ np.array([*self.anchors[c]["pos"], 1.0]))[:3]
            self.vel[c] = new - self.pos[c]
            self.pos[c] = new
            self.dir[c] = M[:3, :3] @ self.anchors[c]["dir"]
        C = self.cols
        for r in range(1, self.rows):
            row = []
            for c in range(C):
                k = r * C + c
                fl, (up, below, left, right) = self.flags[k], self.lens[k]
                old = self.pos[k]
                p = old + pr["strength"] * (last[:3, :3] @ self.force[k]) + self.vel[k] + w
                if not fl & 2 and k + C < len(self.pos):
                    p = pull(p, old, self.pos[k + C], below, GRAVITY)
                if not fl & 8 and c + 1 < C:
                    p = pull(p, old, self.pos[k + 1], right, GRAVITY)
                if not fl & 4 and c > 0:
                    p = pull(p, old, self.pos[k - 1], left, GRAVITY)
                p = pull(p, old, self.pos[k - C], up, GRAVITY)
                if pr["collision"]:
                    p = collide(p, pr["collision"], slots)
                elif p[1] < FLOOR + 0.01:
                    p = p.copy()
                    p[1] = FLOOR + 0.01
                v = (p - old) * damp
                v[1] -= GRAVITY
                row.append((p, v))
            for c, (p, v) in enumerate(row):
                self.pos[r * C + c], self.vel[r * C + c] = p, v

    def normals(self):
        """Per particle: row 0 its anchor direction, else from the grid (0x0C0CDCA0)."""
        C, R = self.cols, self.rows
        P = self.pos.reshape(R, C, 3)
        N = np.zeros((R, C, 3))
        N[0] = self.dir
        for r in range(1, R):
            for c in range(C):
                cr = min(c + 1, C - 1)
                cl = cr - 1
                a, b = P[r - 1, cl], P[r, cr]
                n = np.cross(b - a, P[r, cl] - b)
                ln = np.linalg.norm(n)
                N[r, c] = n / ln if ln > 1e-12 else N[r - 1, c]
        return N.reshape(-1, 3)
