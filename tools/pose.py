#!/usr/bin/env python3
"""Pose a character model set with a motion frame and write an OBJ.

Uses tools/mdl.py (geometry), tools/motion.py (curves) and tools/rig.py
(skeleton evaluation and skin data). The body node (type 0x15 / 0x13) lists
162 matrix slots at +0x88 (slot 0 the envelope, 1 j_mune, 2 mune, 3 kao,
4.. the arms, 14.. the legs); each slot's node name names its rig bone. The
envelope's bind matrices confirm the pairing: their inverses put each slot at
its bone's T-pose position (mune at hips + 0.389 = t_mune, kata / ude / te at
shoulder / elbow / wrist, ...).
  rigid pieces   world = bone matrix x vertex (pieces are in bone space)
  envelope       world = sum(weight x bone matrix x bind) x vertex, written to
                 every model-block offset the skin record lists (type 0x12,
                 0x0C0E8200; type 0x10 the same without UVs, 0x0C0EA5E0)
  face           in the head frame (slot 3): the skinned face (tools/face.py)
                 when the motion sets a face motion (motion.py "face"), else
                 the costume's rigid neutral face; the eyes always
The slots are filled from the game's table (rig.body_slots); the wrists
(slots 8 / 13) take rob_eval_output's wrist matrix (rig.wrist). Swing bones
and cloth are simulated (tools/physics.py). Slots with neither (ex_* bones
the game moves with code not emulated) follow the nearest slot that has a
bone (by T-pose position), and the script lists each one it approximated.
The options set what the game takes from the fight (rig.pose_passes; the
weights step once a motion frame, as at the game's rate of 1): the mirror,
the end-of-move blend to the stance pose (the smooth_ file's block for the
move's end stance, over the last 9 frames of moves long enough), the
motion-change blend (7 frames, 5 when the move has flag 0x4000) and the
head's and eyes' look-at.

usage: pose.py <main.bin> <unpacked asset dir> <texture dir> <motion json>
               <set> <motion name> <frame> <out.obj> [options]
  --mirror             the motion mirrored (the fighter on the other side)
  --smooth             the end-of-move blend to the stance pose
  --from MOTION FRAME  the motion-change blend from MOTION, left at FRAME
  --look X Y Z [FADE]  the head aims at the point (in the fighter's placed
                       frame: its position and facing, y up), blended by
                       FADE (default 1)
  --eyes X Y Z         the eyes aim at the point (eyes_look_at)
  --moves DIR          where mothead_<bank>.json are (default: the motion
                       json's ../moves; --smooth and --from read the move)
  --wind STAGE [FRAME [HEADING]]
                       the stage's wind on the swing bones and cloth
                       (physics.Wind; 0 ROME, 1 NYC, 2 CAS, 4 YAM, 17 GARDEN
                       and 19 HBR blow), FRAME game frames into its swing
                       (default 0), for a fighter at HEADING (rob +0x0C, a
                       16-bit angle; default 0)
  --no-lift            without the height lift (JEFFRY, WOLF, PAI, AOI and
                       SHUN carry the body up or down by their character
                       record's +0x24 except in a few states: rig.lift_on)
  --reach C0 C1 C2 C3 C4 OUT
                       the attack reach as rob_attack_reach left it toward
                       the opponent (reach.current at rob +0x414, out at
                       +0x44C): the hips rise by C0 x OUT, the shoulder and
                       thigh IK aims by C1-C4 x OUT
"""
import glob
import json
import os
import re
import struct
import sys

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mdl  # noqa: E402
import face  # noqa: E402
import physics  # noqa: E402
import pvr_tex  # noqa: E402
import rig  # noqa: E402
import smooth as smooth_mod  # noqa: E402

NAMES = 0x0C292E04  # model set filename table (obj_, mdl_, tex_ per set)

# The fighter fills slots 1-22 from the game's table whatever meshes a set
# has (rig.body_slots; sets without, say, a waki piece still have swing
# chains on slot 4): 1 j_mune, 2 k_mune, 3 j_kubi, 4-7 k_waki_l, j_kata_l,
# j_ude_l, k_te_l, 8 the left wrist, 9-13 the right arm, 14-22 k_kosi and
# the legs. The node names of every set that has the pieces agree with it.
PIECE = {"kosi": "k_kosi", "momo": "j_momo", "sune": "j_sune", "asi": "k_asi", "toe": "k_toe",
         "mune": "k_mune", "kao": "j_kubi", "waki": "k_waki", "kata": "j_kata", "ude": "j_ude", "te": "k_te"}
CHAR_OF_SET = {"ak": "AKIRA", "ao": "AOI", "bl": "BLACK", "bo": "BOZE", "du": "DURAL", "ja": "JACKY",
               "jd": "JDK", "je": "JEFFRY", "ka": "KAGE", "la": "LAU", "li": "LION", "mo": "MOT", "mu": "MUE",
               "pa": "PAI", "sa": "SARAH", "sh": "SHUN", "wo": "WOLF"}  # chr_<xx?> set prefix -> character
BANKS = ["AKI", "SAR", "LAU", "SHN", "JEF", "PAI", "JAC", "KAG", "LIO", "WLF", "AOI", "LEI", "VAN", "DUR", "JDK",
         "MUE"]  # character id -> its mothead bank (the ctrl_ order)
STANCE_BLOCK = {0: 0, 0x80: 2, 1: 4, 0x81: 6, 3: 8}  # end stance (move byte 5, rob +0x285) -> smooth_ block
RSMOOTH_FRAMES = 9.0  # rsmooth_frames


def parse_options(a):
    """Take the motion options out of an argument list -> (the rest, options):
    --mirror               the motion mirrored (rob +0x318 bit 4)
    --smooth               the end-of-move blend to the stance pose
    --from MOTION FRAME    the motion-change blend from MOTION at FRAME
    --look X Y Z [FADE]    the head's look-at point (model space), FADE 1
    --eyes X Y Z           the eyes' look-at point (model space)
    --moves DIR            the mothead_*.json (default: motion dir/../moves)
    --wind STAGE [FRAME [HEADING]]  the stage wind, FRAME game frames in,
                           the fighter at HEADING (defaults 0)
    --no-lift              no height lift (the states that clear it)
    --reach C0..C4 OUT     the attack reach (rob +0x414, +0x44C)"""
    def num(x):
        try:
            float(x)
            return True
        except ValueError:
            return False
    o, rest, i = {}, [], 0
    while i < len(a):
        if a[i] in ("--mirror", "--smooth", "--no-lift"):
            o[a[i][2:]] = True
            i += 1
        elif a[i] == "--from":
            o["from"] = (a[i + 1], float(a[i + 2]))
            i += 3
        elif a[i] == "--eyes":
            o["eyes"] = np.array([float(x) for x in a[i + 1:i + 4]])
            i += 4
        elif a[i] == "--reach":
            v = [float(x) for x in a[i + 1:i + 7]]
            if len(v) < 6:
                sys.exit("--reach needs C0 C1 C2 C3 C4 OUT")
            o["reach"] = (v[:5], v[5])
            i += 7
        elif a[i] == "--wind":
            j = i + 1
            while j < len(a) and j < i + 4 and re.fullmatch(r"-?\d+", a[j]):
                j += 1
            if j == i + 1:
                sys.exit("--wind needs STAGE [FRAME [HEADING]]")
            o["wind"] = tuple(int(x) for x in a[i + 1:j])
            i = j
        elif a[i] == "--moves":
            o["moves"] = a[i + 1]
            i += 2
        elif a[i] == "--look":
            j = i + 1
            while j < len(a) and j < i + 5 and num(a[j]):
                j += 1
            v = [float(x) for x in a[i + 1:j]]
            if len(v) < 3:
                sys.exit("--look needs X Y Z [FADE]")
            o["look"] = (np.array(v[:3]), v[3] if len(v) > 3 else 1.0)
            i = j
        else:
            rest.append(a[i])
            i += 1
    return rest, o


def bone_for(name, byname):
    m = re.search(r"_[ok]_(asi|kosi|momo|sune|toe|mune|kao|waki|kata|ude|te)(?:_([lr]))?$", name)
    if not m:
        return None
    b = PIECE[m.group(1)] + (f"_{m.group(2)}" if m.group(2) else "")
    return byname.get(b)


class Character:
    """A character model set bound to its rig (shared by pose.py and gltf.py).

    rigid      node index -> rig bone: pieces in bone space
    on_slot    node index -> slot, for pieces on a swing-bone (ex_*) slot
    envelopes  node index -> {slot: rig bone} for the envelope's body
    slot_bone  slot -> rig bone, over every body (one 162-slot table)
    swing      physics.SwingBones over the bodies' ex_* chains
    """

    def __init__(self, prog, src, s):
        self.prog, self.set = prog, s
        self.obj = open(os.path.join(src, f"obj_{s}.bin"), "rb").read()
        self.mdl = open(os.path.join(src, f"mdl_{s}.bin"), "rb").read()
        self.nodes = mdl.nodes_of(self.obj)
        self.offs = mdl.zlist(self.obj, mdl.u32(self.obj, 0))
        chars = {c["name"]: c["rig"] for c in rig.characters(prog)}
        self.who = CHAR_OF_SET.get(s.split("_")[1][:2])
        if self.who is None:
            sys.exit(f"{s}: not a character set")
        self.bones = rig.rig(prog, chars[self.who])
        self.rig0 = rig.rig(prog, 0)  # what rob_eval_ik solves every fighter's IK with
        self.byname = {b["name"]: b["index"] for b in self.bones}
        table, self.wrist_slots = rig.body_slots(prog, self.bones)  # slot -> bone / hand bone
        self.rigid, self.on_slot, self.envelopes, self.slot_bone = {}, {}, {}, {}
        self.cloth, self.cloth_note = {}, []
        idx = {o: i for i, o in enumerate(self.offs)}
        P = physics.Prog(prog)
        names = [physics.Prog(prog).u(NAMES + 12 * k) for k in range(0x200)]
        set_index = next((k for k, a in enumerate(names) if BASE_ <= a < BASE_ + len(prog)
                          and prog[a - BASE_:a - BASE_ + len(s) + 8] == f"obj_{s}.bin".encode()), None)
        char = next(c for c in rig.characters(prog) if c["name"] == self.who)
        self.char_id = char["id"]
        # the height lift (rig.pose_passes lift=), on as in a fight; --no-lift
        self.lift = (char["height"], char["id"]) if rig.lift_on(char["height"]) else None
        self.reach = None  # the attack reach (rig.pose_passes reach=); --reach
        self.costume = 1 if s[-1] in "249" else 0  # chr_xxx / xx8 costume 0, xx2 / xx9 / sh4 costume 1
        items = physics.item_records(P)
        bodies = []
        for nd in self.nodes:  # each body node's 162 (or 123) matrix slots
            if nd["type"] not in (0x13, 0x15):
                continue
            base = mdl.u32(self.obj, self.offs[nd["index"]] + 0x88)
            slots = [mdl.u32(self.obj, base + 4 * k) for k in range(0xA2 if nd["type"] == 0x15 else 0x7B)]
            # slot 1 is the spine base j_mune: in chr_je2, whose envelope uses
            # it, its bind matrix puts it at the hips with t_mune's rotation
            # (slot 2 is the same frame 0.41 higher); no node names it
            smap = dict(table)
            for k, o in enumerate(slots):
                if o in idx and k >= 2:
                    bi = bone_for(self.nodes[idx[o]]["name"], self.byname)
                    if bi is not None:
                        if smap.get(k, bi) != bi:
                            raise ValueError(f"{s}: node {self.nodes[idx[o]]['name']} on slot {k} is not "
                                             f"{self.bones[table[k]]['name']}")
                        smap[k] = bi
                        self.rigid[idx[o]] = bi
                    elif "_o_ex_" in self.nodes[idx[o]]["name"]:
                        self.on_slot[idx[o]] = k
            self.slot_bone.update(smap)
            env = idx.get(slots[0])
            if env is not None and self.nodes[env]["type"] in (0x10, 0x12):  # 0x10: no UVs in the skin
                self.envelopes[env] = smap
            mid = (set_index << 16 | nd["index"]) if set_index is not None else None
            if nd["name"].startswith("CH_"):
                spec = physics.CHARACTER.get((char["id"], self.costume))
            else:
                rec = items.get(mid)
                spec = physics.item_spec(rec) if rec else None
            bodies.append((self.obj, self.offs[nd["index"]], mid, spec))
            # cloth nodes (type 0x0F) in the body's +0x8C list, one 0x18 param record each
            l8c = mdl.u32(self.obj, self.offs[nd["index"]] + 0x8C)
            if l8c:
                if nd["name"].startswith("CH_"):
                    plist = physics.CLOTH.get((char["id"], self.costume))
                    if plist is None:
                        self.cloth_note.append(f"{nd['name']}: no cloth params for this costume, default used")
                        plist = physics.DEFAULT_CLOTH
                else:
                    rec = items.get(mid)
                    plist = rec[3] if rec and rec[3] else physics.DEFAULT_CLOTH
                k = 0
                for co in mdl.zlist(self.obj, l8c):
                    ci = idx.get(co)
                    if ci is None or self.nodes[ci]["type"] != 0x0F:
                        continue
                    a = plist + 0x18 * k
                    if P.u(a) == 0xFFFFFFFF:
                        a = physics.DEFAULT_CLOTH
                    self.cloth[ci] = physics.Cloth(P, self.obj, co, physics.cloth_params(P, a))
                    k += 1
        self.swing = physics.SwingBones(prog, bodies)
        self.swing_slots = {j["slot"] for c in self.swing.chains for j in c["joints"]}
        for nd in self.nodes:  # not in the slot list: the hands (k_te)
            if nd["index"] in self.rigid or not nd["name"].startswith("CH_"):
                continue
            if "_k_te_" in nd["name"]:
                bi = bone_for(nd["name"], self.byname)
                if bi is not None:
                    self.rigid[nd["index"]] = bi
        # the face (tools/face.py), drawn in the head frame (slot 3): the
        # costume's rigid neutral face unless a face motion plays, then the
        # skinned face on its 33-bone rig; and the two eyes
        self.face, self.eyes = None, []
        fi = face.info(prog, char["index"], set_index) if set_index is not None else None
        if fi:
            if fi["costume"] != self.costume:
                raise ValueError(f"{s}: costume table says costume {fi['costume']}, set name says {self.costume}")
            node = fi["face"] & 0xFFFF
            groups, static = face.skin(self.obj, self.offs[node])
            self.face = dict(fi, node=node, bones=face.bones(prog, fi["rig"]), groups=groups, static=static,
                             rigid=[m & 0xFFFF for m in fi["rigid"]], scale=face.scales(prog))
            self.eyes = [(mid & 0xFFFF, off) for mid, off in face.eyes(prog, char["id"], fi["costume"])
                         if mid >> 16 == set_index]
        self.head = table[3]  # the face and eyes are drawn in slot 3's frame

    def face_pose(self, curves, frame):
        """Head-frame face bone matrices for a face motion at a frame (all 33 bones)."""
        f = self.face
        return face.pose(f["bones"], curves, frame, f["offset"], f["scale"])

    def eye_matrices(self, curves=None, frame=0, target=None, head=None):
        """Head-frame matrix of each eye (node, matrix); angles from a face motion's c_eye bones, else 0.
        target (model space) with head (the head's matrix): the eyes look at it
        (face.eye_look, from the point halfway between them)."""
        ang = face.eye_angles(curves, frame) if curves else [[0.0] * 3] * 2
        if target is not None and self.eyes:
            mid = np.mean([np.add(off, self.nodes[n]["position"]) for n, off in self.eyes], axis=0)
            y, z = face.eye_look(head, mid, target)
            ang = [[a[0], y, z] for a in ang]
        return [(n, face.eye_matrix(self.nodes[n], off, ang[j])) for j, (n, off) in enumerate(self.eyes)]

    def block(self, i):
        b = mdl.u32(self.mdl, 4 * i) if 4 * i < mdl.u32(self.mdl, 0) else 0
        return b if b and mdl.u32(self.mdl, b) == 0x100 else 0

    def envelope(self, i):
        """(groups, binds, slot -> slot whose bone and bind it uses, approximations).

        Slots with no rig bone and no emulation (ex_* ribbon / belt bones that
        the game moves with its own cloth code) use the nearest slot that
        has one, by T-pose position: an approximation."""
        groups, binds = rig.skin(self.obj, self.offs[i])
        smap = dict(self.envelopes[i])
        smap.update({k: None for k in self.swing_slots | set(self.wrist_slots)})  # emulated: no stand-in
        used = {slot for g in groups for slot, w in g["influences"]}
        resolved = [j for j in smap if j != 1 and smap[j] is not None]
        # only the slots in use: past a 0x13 body's 123 slots the table is
        # not bind matrices (chr_ao8 / ao9 hit a singular one)
        tpose = {k: np.linalg.inv(binds[k])[:3, 3] for k in used | set(resolved)}
        use, approx = {k: k for k in smap}, []
        for k in sorted(used - set(smap)):
            near = min(resolved, key=lambda j: np.linalg.norm(tpose[j] - tpose[k]))
            use[k] = near
            approx.append(f"{self.nodes[i]['name']} slot {k} -> slot {near}")
        return groups, binds, use, approx

    def slots(self, world, wrists):
        sl = {k: world[b] for k, b in self.slot_bone.items()}
        sl.update({k: wrists[h] for k, h in self.wrist_slots.items()})
        return sl

    def motion_options(self, mot, motdir, src, o):
        """rig.pose's keyword arguments at each time t for the motion mot under
        the options o (parse_options). motdir holds the motdat_*.json, src the
        smooth_*.bin (or its parent), o["moves"] (default: motdir/../moves) the
        mothead_*.json. The weights step with the integer move frame n = t + 1
        (rob +0x0A; rob_eval evaluates the frame before it)."""
        if not {"mirror", "smooth", "from", "look"} & set(o):
            return lambda t: {}
        flip = rig.mirror_tables(self.prog) if o.get("mirror") else None
        moves = o.get("moves") or os.path.join(os.path.dirname(os.path.abspath(motdir)), "moves")
        heads = [json.load(open(p)) for p in sorted(glob.glob(os.path.join(moves, "mothead_*.json")))]
        move = next((m for h in heads for m in h["moves"] if m["id"] == mot["id"]), None)
        blen = 5 if move and int(move["flags"], 16) & 0x4000 else 7  # mot_blend_start's usual lengths
        length = int(mot["frames"])  # rob +0x2E4
        table = None
        if o.get("smooth"):
            k = STANCE_BLOCK.get(move["byte5"]) if move else None
            if k is None or length < RSMOOTH_FRAMES + blen:
                print(f"{mot['name']}: no stance blend (stance {move and move['byte5']}, {length} frames)",
                      file=sys.stderr)
            else:
                bank = next(h["bank"] for h in heads if h["bank"]["mothead"] == f"mothead_{BANKS[self.char_id]}.bin")
                path = next((p for p in (os.path.join(src, bank["smooth"]),
                                         os.path.join(os.path.dirname(os.path.abspath(src)), bank["smooth"]))
                             if os.path.exists(p)), None)
                if path is None:
                    sys.exit(f"--smooth: {bank['smooth']} is not in {src} or its parent")
                _, mtx, _ = smooth_mod.read(path)
                k += flip is not None  # the _M block (rob +0x3A6 bit 4)
                table = {i: mtx[k, j].T for j, (i, _, _) in enumerate(smooth_mod.bones(self.prog))}
        keep = None
        if "from" in o:
            name, fr = o["from"]
            prev = next((m for p in sorted(glob.glob(os.path.join(motdir, "motdat_*.json")))
                         for m in json.load(open(p))["motions"] if m["name"] == name), None)
            if prev is None:
                sys.exit(f"--from: no motion {name} in {motdir}")
            pmove = next((m for h in heads for m in h["moves"] if m["id"] == prev["id"]), None)
            st = {}
            rig.pose_passes(self.bones, {int(k): v for k, v in prev["bones"].items()}, fr, flip=flip,
                            state=st, base=self.rig0, held=bool(pmove and int(pmove["flags"], 16) & 0x10),
                            lift=self.lift, reach=self.reach)
            keep = (st["keep"], rig.root_offset(self.bones, {int(k): v for k, v in mot["bones"].items()}, flip,
                                                self.lift, self.reach))

        def at(t):
            n = int(t) + 1
            kw = {"flip": flip, "look": o.get("look")}
            if keep and n < blen:
                kw["blend"] = (keep[0], n / blen, keep[1])
            elif table and length < n + RSMOOTH_FRAMES:
                kw["smooth"] = (table, 1.0 if length < n else 1 - (length - n) / RSMOOTH_FRAMES, 0.0)
            return kw
        return at

    def frames(self, curves, last, at=None, wind=None):
        """Slot matrices of frames 0..last: the rig at 60 Hz with the swing bones
        reset at frame 0 and stepped every game frame (two per motion frame).
        at: motion_options' keyword arguments by time; wind: (stage[, frame[,
        heading]]) for physics.Wind, else none."""
        w = physics.Wind(self.prog, *wind) if wind else None
        for t in range(2 * last + 1):
            f = w.update() if w else None
            wrists = {}
            kw = at(t / 2) if at else {}
            if self.lift:
                kw = {"lift": self.lift, **kw}
            if self.reach:
                kw = {"reach": self.reach, **kw}
            world = rig.pose(self.bones, curves, t / 2, wrists, base=self.rig0, **kw)
            sl = self.slots(world, wrists)
            if t == 0:
                self.swing.reset(sl, f)
                for c in self.cloth.values():
                    c.reset(sl)
            else:
                self.swing.step(sl, wind=f)
                for c in self.cloth.values():
                    c.step(sl, f)
            if t % 2 == 0:
                yield t // 2, sl, world


BASE_ = 0x0C020000


def main():
    args, opts = parse_options(sys.argv[1:])
    if len(args) < 8:
        sys.exit(__doc__)
    prog_path, src, texdir, motjson, s, mname, frame, out = args[:8]
    prog = open(prog_path, "rb").read()
    ch = Character(prog, src, s)
    if opts.get("no-lift"):
        ch.lift = None
    ch.reach = opts.get("reach")
    nodes, m = ch.nodes, ch.mdl
    mot = next(x for x in json.load(open(motjson))["motions"] if x["name"] == mname)
    curves = {int(k): v for k, v in mot["bones"].items()}
    at = ch.motion_options(mot, os.path.dirname(os.path.abspath(motjson)), src, opts)
    for f, slots, world in ch.frames(curves, int(frame), at, opts.get("wind")):
        pass
    # the face motion this move sets (motion.py "face"), else the rigid face
    fmats, fcurves = None, None
    if ch.face and "face" in mot:
        fm = next(x for x in json.load(open(motjson))["motions"] if x["id"] == mot["face"])
        fcurves = {int(k): v for k, v in fm["bones"].items()}
        fmats = ch.face_pose(fcurves, int(frame))
    eyes = dict(ch.eye_matrices(fcurves, int(frame), opts.get("eyes"), world[ch.head]))

    descs = pvr_tex.descriptors(os.path.join(src, f"obj_{s}.bin"))
    texnames = [f"{i:03d}_{w}x{h}_{pvr_tex.PIXEL[f & 0xFF]}_{pvr_tex.TYPE[f & 0xF00]}.png"
                for i, (w, h, f, o_, x) in enumerate(descs)]
    approx = set()
    lines, mats_used, base_i = [f"# {s} posed with {mname} frame {frame} by tools/pose.py", f"mtllib {os.path.basename(out)[:-4]}.mtl"], {}, 1
    for nd in nodes:
        i = nd["index"]
        b = ch.block(i)
        if not b:
            continue
        posed = {}
        if i in ch.envelopes:
            groups, binds, use, ap = ch.envelope(i)
            approx.update(ap)
            smap = ch.envelopes[i]
            for g in groups:
                M = sum(w * slots[use[slot]] @ binds[use[slot]] for slot, w in g["influences"])
                for v in g["vertices"]:
                    p = M @ np.array([*v["pos"], 1.0])
                    for off, flags in v["outputs"]:
                        posed[off] = p[:3]
        elif i in ch.rigid:
            M = world[ch.rigid[i]]
        elif i in ch.on_slot and ch.on_slot[i] in slots:
            M = slots[ch.on_slot[i]]
        elif ch.face and i == ch.face["node"]:
            if fmats is None:
                continue
            posed = {o: (world[ch.head] @ np.array([*q, 1.0]))[:3] for o, q in face.apply(ch.face["groups"], ch.face["static"], fmats).items()}
        elif ch.face and i == ch.face["rigid"][0]:
            if fmats is not None:
                continue
            M = world[ch.head]
        elif i in eyes:
            M = world[ch.head] @ eyes[i]
        elif i in ch.cloth:
            cl = ch.cloth[i]
            for mp in cl.maps:
                for k, outs in enumerate(mp):
                    for off, flags in outs:
                        posed[off] = cl.pos[k]
        else:
            continue
        lines.append(f"o {nd['name']}")
        for tex, kind, verts, tris in mdl.model(m, b):
            mat = f"tex{tex:03d}_{kind}" if tex is not None else f"untextured_{kind}"
            mats_used[mat] = (tex, kind)
            for v in verts:
                p = posed.get(v["offset"]) if posed or i in ch.envelopes or i in ch.cloth else (M @ np.array([*v["p"], 1.0]))[:3]
                if p is None:
                    p = v["p"]
                uv = v["uv"] or (0.0, 0.0)
                uv = tuple(c if c == c else 0.0 for c in uv)
                lines.append(f"v {p[0]:.6f} {p[1]:.6f} {p[2]:.6f}")
                lines.append(f"vt {uv[0]:.6f} {uv[1]:.6f}")
            lines.append(f"usemtl {mat}")
            for t in tris:
                lines.append("f " + " ".join(f"{base_i + k}/{base_i + k}" for k in t))
            base_i += len(verts)
    for a in sorted(approx) + ch.cloth_note:
        print("approximated:", a, file=sys.stderr)
    os.makedirs(os.path.dirname(os.path.abspath(out)), exist_ok=True)
    open(out, "w").write("\n".join(lines) + "\n")
    rel = os.path.relpath(os.path.join(texdir, s), os.path.dirname(os.path.abspath(out)))
    with open(out[:-4] + ".mtl", "w") as f:
        for mat, (tex, kind) in sorted(mats_used.items()):
            f.write(f"newmtl {mat}\nKd 1 1 1\n")
            if tex is not None and tex < len(texnames):
                f.write(f"map_Kd {rel}/{texnames[tex]}\n")
            f.write("\n")


if __name__ == "__main__":
    main()
