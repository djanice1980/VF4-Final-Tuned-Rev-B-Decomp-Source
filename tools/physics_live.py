#!/usr/bin/env python3
"""physics_live.py RAM0 RAM1 [RAM2 ...]: tools/physics.py against the game, one
frame at a time.

The RAM images are consecutive flycast_live.py phys dumps (32 MB of main RAM
at the entry of osage_calc_all 0x0C0D3640, frames in a row). Each holds the
chains and the cloth as the previous frame left them, the skeleton this
frame steps them on (rob +0x54, 162 slot matrices) and the stage wind before
its update. For each pair and each fighter: the game's node positions and
velocities of the first go into physics.SwingBones / physics.Cloth (built
from the fighter's model set as tools/pose.py builds them, with our
parameter tables), one step runs on the first image's slots and the wind
record's own update, and the result is compared with the second image:

  wind     our update of the first image's stage_wind (0x0C3AD21C) against
           the second's osage_wind (0x0C2FF5BC)
  params   our osage_param entries against the rob's j_ex_00 table, and our
           cloth parameters against the slot's strength / air / sideways
  chains   per chain, the worst tip, velocity and slot matrix difference
           (game nodes: osage_chains[2][64] 0x0C39C450, 0x2C-byte nodes)
  cloth    per cloth slot (cloth_slots 0x0C4DD4A8), the worst particle
           position and velocity difference; the anchors' slots are the
           first image's, with the chain slots the game had just stepped
           (the second image's), as cloth_calc_all runs after the chains

Globals that change the step (world gravity, the gravity add, air_reg,
strength, yoko, the root offset, the angle table flag, the collision switch)
are printed beside each pair; the step assumes the .data / boot values."""
import os
import struct
import sys

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import physics  # noqa: E402
import pose  # noqa: E402

RAM_BASE = 0x0C000000
ROBS, ROB_SIZE = 0x0C37BA30, 0x86C
CHAINS = 0x0C39C450
CLOTH_SLOTS = 0x0C4DD4A8
STAGE_WIND = 0x0C3AD21C
OSAGE_WIND = 0x0C2FF5BC
GLOBALS = {"world_gravity": (0x0C371988, "f"), "osage_gravity_add": (0x0C2FF5DC, "f"),
           "air_reg": (0x0C2B0FE8, "f"), "strength": (0x0C2B0FE4, "f"), "yoko": (0x0C2B0FEC, "f"),
           "root_offset": (0x0C2FF5E0, "3f"), "chain_table_flags": (0x0C2FF5EC, "I"),
           "osage_coli_off": (0x0C2FF5C8, "i"), "mode_timer_rate": (0x0C292678, "f"),
           "cloth_rest_pull": (0x0C2FF6E4, "f"), "cloth_asm_flags": (0x0C2FF6F0, "i")}
R_ = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(R_, "work", "assets", "unpacked")


class Ram:
    def __init__(self, path):
        self.b = open(path, "rb").read()
        self.path = path

    def get(self, fmt, a):
        v = struct.unpack_from("<" + fmt, self.b, a - RAM_BASE)
        return v[0] if len(v) == 1 else v

    def vec(self, a):
        return np.array(self.get("3f", a))

    def mtx(self, a):
        return np.array(self.get("16f", a)).reshape(4, 4).T

    def cstr(self, a):
        o = a - RAM_BASE
        return self.b[o:self.b.index(b"\0", o)].decode("latin-1")


def game_nodes(r, side, robmtx):
    """slot -> node address for every joint of the rob's used chains, and the
    chains as [(root slot, [node addresses])]."""
    by_slot, chains = {}, []
    for j in range(0x40):
        c = CHAINS + (side * 0x40 + j) * 0x1C
        if not r.get("i", c):
            continue
        cmds, node = r.get("I", c + 8), r.get("I", c + 0xC)
        nodes = []
        while True:
            tag = r.get("i", cmds)
            cmds += 8
            if tag == 2:
                continue
            nodes.append(node)
            m = r.get("I", node + 4)
            if m:
                by_slot[(m - robmtx) // 0x40] = node
            node += 0x2C
            if tag in (0, 4):
                break
        chains.append((r.get("i", c + 4), nodes))
    return by_slot, chains


def worst(pairs):
    d = [float(np.abs(a - b).max()) for a, b in pairs]
    return max(d) if d else 0.0


def wind_of(r):
    w = physics.Wind.__new__(physics.Wind)
    a = STAGE_WIND
    w.bias = np.float32(r.get("f", a + 0xC))
    cyc, rat = r.get("8f", a + 0x10), r.get("8f", a + 0x30)
    w.waves = [(np.float32(c), np.float32(q)) for c, q in zip(cyc, rat)]
    w.phase = [np.float32(p) for p in r.get("8f", a + 0x54)]
    w.R = (physics.rot("y", r.get("h", a + 0x52)) @ physics.rot("x", r.get("h", a + 0x50)))[:3, :3]
    w.vec = np.zeros(3)
    return w


characters = {}


def character(prog, r, body_model):
    name = r.cstr(r.get("I", pose.NAMES + 12 * (body_model >> 16)))
    s = name[4:name.index(".bin")]  # obj_<set>.bin
    if s not in characters:
        characters[s] = pose.Character(prog, SRC, s)
    return s, characters[s]


def compare(prog, a, b):
    print("== %s -> %s" % (os.path.basename(a.path), os.path.basename(b.path)))
    g = {k: a.get(f, ad) for k, (ad, f) in GLOBALS.items()}
    print("   globals:", ", ".join("%s %s" % (k, ("%.9g" % v) if isinstance(v, float) else v) for k, v in g.items()))
    if abs(g["world_gravity"] - physics.GRAVITY) > 1e-9:
        print("   world_gravity is not physics.GRAVITY %.9g" % physics.GRAVITY)
    w = wind_of(a)
    f = w.update()
    gw = b.vec(OSAGE_WIND)
    print("   wind: ours %s game %s diff %.2e" % (np.round(f, 6), np.round(gw, 6), np.abs(f - gw).max()))
    for side in range(2):
        rob = ROBS + side * ROB_SIZE
        if not a.get("I", rob + 0x18) & 1:
            continue
        flags = a.get("I", rob + 0x18)
        robmtx = a.get("I", rob + 0x54)
        bm = a.get("I", rob + 0x540)
        s, ch = character(prog, a, bm)
        physics.FLOOR = a.get("f", rob + 0x7C)
        print("   side %d: %s (chara %d costume %d) move %d frame_pos %.3f -> %.3f floor %.4f%s" % (
            side, s, a.get("B", rob + 1), a.get("B", rob + 2), a.get("H", rob + 8), a.get("f", rob + 0x70),
            b.get("f", rob + 0x70), physics.FLOOR, "  RESET (+0x18 0x100000)" if flags & 0x100000 else ""))
        slots_a = {k: a.mtx(robmtx + 0x40 * k) for k in range(0xA2)}
        slots_b = {k: b.mtx(robmtx + 0x40 * k) for k in range(0xA2)}
        by_slot, gchains = game_nodes(a, side, robmtx)
        sw = ch.swing
        ours = {j["slot"]: (c, j) for c in sw.chains for j in c["joints"]}
        print("     chains: game %d (%d joints), ours %d (%d joints); joint slots only in game %s, only ours %s" % (
            len(gchains), len(by_slot), len(sw.chains), len(ours),
            sorted(set(by_slot) - set(ours)), sorted(set(ours) - set(by_slot))))
        # parameters
        bad = []
        for slot, (c, j) in ours.items():
            if slot not in by_slot:
                continue
            pa = a.get("I", by_slot[slot])  # node num = the param index
            if pa != j["param"]:
                bad.append((slot, "param index game %d ours %d" % (pa, j["param"])))
        # the rob's parameter table: the chain slot's +0x10
        ptabs = {a.get("I", CHAINS + (side * 0x40 + k) * 0x1C + 0x10) for k in range(0x40)
                 if a.get("i", CHAINS + (side * 0x40 + k) * 0x1C)}
        for pt in ptabs:
            for slot, (c, j) in ours.items():
                q = pt + 0x18 * j["param"]
                gv = (a.get("H", q), a.get("h", q + 2), a.get("f", q + 4), a.get("f", q + 8), a.get("f", q + 12),
                      a.get("h", q + 16), a.get("h", q + 18), a.get("I", q + 20) != 0)
                p = sw._param(c, j)
                ov = (p["type"], p["limit"], p["stiffness"], p["damping"], p["gravity"], p["ry"], p["rz"],
                      bool(p["collision"]))
                if any(abs(x - y) > 1e-6 for x, y in zip(gv[:7], ov[:7])) or gv[7] != ov[7]:
                    bad.append((slot, "j_ex_%02d game %s ours %s" % (j["param"], gv, ov)))
        print("     params: %d differ%s" % (len(bad), "".join("\n       slot %d: %s" % x for x in bad[:8])))
        # load the game's state, step, compare
        for slot, (c, j) in ours.items():
            n = by_slot.get(slot)
            if n is not None:
                j["rec"]["tip"] = a.vec(n + 8)
                j["rec"]["vel"] = a.vec(n + 0x14)
        sl = dict(slots_a)
        sw.step(sl, wind=f)
        for k, c in enumerate(sw.chains):
            js = [j for j in c["joints"] if j["slot"] in by_slot]
            if not js:
                continue
            tip = worst((j["rec"]["tip"], b.vec(by_slot[j["slot"]] + 8)) for j in js)
            vel = worst((j["rec"]["vel"], b.vec(by_slot[j["slot"]] + 0x14)) for j in js)
            mtx = worst((sl[j["slot"]], slots_b[j["slot"]]) for j in js)
            moved = worst((a.vec(by_slot[j["slot"]] + 8), b.vec(by_slot[j["slot"]] + 8)) for j in js)
            print("     chain %2d root slot %3d, %d joints (slots %d-%d): tip %.2e vel %.2e mtx %.2e (moved %.2e)" % (
                k, c["root"]["parent"], len(js), js[0]["slot"], js[-1]["slot"], tip, vel, mtx, moved))
        # cloth
        csl = dict(slots_a)
        csl.update({k: slots_b[k] for k in ours})
        mine = list(ch.cloth.values())
        for i in range(8):
            cs = CLOTH_SLOTS + side * 0x1C0 + i * 0x38
            if not a.get("i", cs):
                continue
            obj, hdr = a.get("I", cs + 0xC), a.get("I", cs + 0x28)
            fb = a.get("I", obj + 0x38)
            cols, rows = a.get("i", hdr), a.get("i", hdr + 4)
            parts = fb + a.get("I", hdr + 0x10)
            n = cols * rows
            rest = np.array([a.vec(parts + 0x50 * k + 0x28) for k in range(n)])
            cl = next((x for x in mine if x.cols == cols and x.rows == rows and np.abs(np.array(x.rest) - rest).max() < 1e-6), None)
            gpar = (a.get("f", cs + 0x10), a.get("f", cs + 0x14), a.get("f", cs + 0x18))
            if cl is None:
                print("     cloth slot %d (%dx%d): no matching cloth in %s" % (i, cols, rows, s))
                continue
            opar = (cl.params["strength"], cl.params["damping"], cl.params["sideways"])
            cl.pos = np.array([a.vec(parts + 0x50 * k + 4) for k in range(n)])
            cl.vel = np.array([a.vec(parts + 0x50 * k + 0x1C) for k in range(n)])
            cl.dir = np.array([a.vec(parts + 0x50 * k + 0x10) for k in range(cols)])
            cl.step(csl, f)
            gpos = np.array([b.vec(parts + 0x50 * k + 4) for k in range(n)])
            gvel = np.array([b.vec(parts + 0x50 * k + 0x1C) for k in range(n)])
            moved = np.abs(gpos - np.array([a.vec(parts + 0x50 * k + 4) for k in range(n)])).max()
            print("     cloth slot %d (%dx%d): pos %.2e (row 0 %.2e) vel %.2e (moved %.2e); params game %s ours %s" % (
                i, cols, rows, np.abs(cl.pos - gpos).max(), np.abs(cl.pos[:cols] - gpos[:cols]).max(),
                np.abs(cl.vel - gvel).max(), moved, tuple(round(x, 6) for x in gpar),
                tuple(round(x, 6) for x in opar)))


def main():
    paths = sys.argv[1:]
    if len(paths) < 2:
        sys.exit(__doc__)
    prog = open(os.path.join(R_, "work", "main.bin"), "rb").read()
    rams = [Ram(p) for p in paths]
    for a, b in zip(rams, rams[1:]):
        compare(prog, a, b)


if __name__ == "__main__":
    main()
