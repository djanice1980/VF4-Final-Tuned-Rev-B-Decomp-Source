#!/usr/bin/env python3
"""Decode the rsmooth stance-pose tables (smooth_<chr>.bin).

rob_skeleton_update (source/vf4lib/libmotion/src/mot_tools.c) loads the
fighter's file into the "smooth_load_addr" heap (rob_load_heaps[rob][2]).
Near the end of a move it picks one block by stance, points 23 bones at the
block's matrices and blends the pose toward them. A file is 16 blocks of
0x5C8 bytes, in the order of e_rsmooth_no:

  block, 0x5C8 bytes
    +0x000 23 matrices, 4x4 floats each (0x40 bytes; row-major, translation
           in row 3), one per bone of rsmooth_bone_list (0x0C18BE68)
    +0x5C0 left hand shape, +0x5C4 right hand shape (floats, 0 .. 0.2)

Choosing the block (rob +0x285 is the stance; +1 when rob +0x3A6 has 0x10,
the _M blocks):
  stance 0 / 0x80 / 1 / 0x81 -> TA_KAMAE / SY_KAMAE / TA_GUARD / SY_GUARD
  stance 3 -> A_DOWN, or U_DOWN when rob +0x20 has 0x800000
  character id 12 with rob +0x550 != 0: stance 0 / 0x80 -> SP_TA_KAMAE /
  SP_SY_KAMAE, stance 3 as above, no blend in other stances
  other stances: no blend

The blend (weight w = rob +0x3D4 on the table's pose, over the move's last
rsmooth_frames, 9: w = 1 - (length +0x2E4 - frame +0x0A) / 9, rising to 1
at the end; only in moves 9 frames longer than their motion-change blend):
  rotation  rob_eval_output: bones with flag 0x800 and without 0x2008000
            take mtx_rot_lerp(table, motion, w), on the 3x3 part only
            (bones with 0x40000000 keep the motion's while +0x3A6 has 0x1000)
  position  rsmooth_targets_set stores each bone's translation, turned into
            the fighter's ground frame, at bone +0x2C; bone_pos_rsmooth_blend
            moves the flag-0x2000 bones (the t_ bones) and
            root_pos_rsmooth_blend the root (k_hara, flag 0x8000) toward it
  bones 2 / 25 (c_mune, k_kosi): rsmooth_targets_set rebuilds their whole
            matrices into rsmooth_root_mtx when the fighter lies down or
            faces away (+0x3A6 0x5040 / 0x8000 / 0x400)
  hands     the floats go to bone 13 / 21 +0x2C (s_te_l / s_te_r, flag
            0x10000000); rob_eval_output blends the hand value toward them
            by w and stores 10x them in the hand records (rob +0x504 /
            +0x518), which rob_hands_step eases toward
So the game reads the rotation of 16 bones (not k_hara or the t_ bones) and
the position of 7 (k_hara and the t_ bones); the rest is exporter output.
pose.py / gltf.py --smooth apply them (rig.pose_passes), but not the hands or
the down and turned stances.

What the files hold (the checks below):
  - only character id 12 (BLACK: smooth_blk, ctrl_VAN, the fighter with two
    control sets that rob +0x550 picks between) reaches the SP_ blocks, and
    only smooth_blk has them all. In the other 15, 0xCDCDCDCD (an MSVC debug
    fill: memory the exporter never wrote) fills SP_SY_KAMAE and all but the
    last 32 words of SP_SY_KAMAE_M; SP_TA_KAMAE and _M too in akira, dural,
    jdk, jef, lau, mue and wolf, _M in boz. aoi, kage, lion, sarah and shun
    keep a pose there no code reads; jac and pai SP_TA_KAMAE hold shifted
    data (not rotations), _M 28 words of fill.
  - each _M block is its pair mirrored: positions with x negated and l / r
    bones swapped, rotations as A R B with fixed signs per bone (A, B
    diagonal, +-1), hands swapped. Within 1e-5, but jac SY_KAMAE within
    0.002 and lion TA_KAMAE posed apart (hips 0.028 lower, up to 0.09).
  - k_hara's rotation is all zero in akira and lau TA_KAMAE (not read).

usage: smooth.py <main.bin> <smooth_*.bin>...           checks, one line a file
       smooth.py <main.bin> <smooth_*.bin>... --json <out.json>
                                                        the blocks as JSON
"""
import itertools
import json
import os
import struct
import sys

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import rig  # noqa: E402

BONE_LIST = 0x0C18BE68
BLOCK = 0x5C8
FILL = 0xCDCDCDCD
NAMES = ["TA_KAMAE", "TA_KAMAE_M", "SY_KAMAE", "SY_KAMAE_M", "TA_GUARD", "TA_GUARD_M", "SY_GUARD",
         "SY_GUARD_M", "A_DOWN", "A_DOWN_M", "U_DOWN", "U_DOWN_M", "SP_TA_KAMAE", "SP_TA_KAMAE_M",
         "SP_SY_KAMAE", "SP_SY_KAMAE_M"]
SIGNS = [np.diag(s) for s in itertools.product((1.0, -1.0), repeat=3)]


def bones(prog):
    """The 23 table bones: (rig-0 index, name, flags)."""
    rig0 = rig.rig(prog, 0)
    idx = prog[BONE_LIST - rig.BASE:BONE_LIST - rig.BASE + 23]
    return [(i, rig0[i]["name"], rig0[i]["flags"]) for i in idx]


def read(path):
    raw = open(path, "rb").read()
    assert len(raw) == 16 * BLOCK, (path, len(raw))
    words = np.frombuffer(raw, dtype="<u4").reshape(16, BLOCK // 4)
    floats = np.frombuffer(raw, dtype="<f4").reshape(16, BLOCK // 4)
    return words, floats[:, :368].reshape(16, 23, 4, 4), floats[:, 368:]


def rot_read(flags):
    return flags & 0x800 and not flags & 0x2008000


def pos_read(flags):
    return bool(flags & 0x2000 or flags & 0x8000)


def check(path, bl):
    words, mtx, hands = read(path)
    names = [b[1] for b in bl]
    swap = [names.index(n[:-1] + ("r" if n[-1] == "l" else "l")) if n[-2:] in ("_l", "_r") else i
            for i, n in enumerate(names)]
    fill = [int((words[k] == FILL).sum()) for k in range(16)]
    clean = [f == 0 for f in fill]
    not_rot = []
    for k in range(16):
        if not clean[k]:
            continue
        for j, (_, n, fl) in enumerate(bl):
            r = mtx[k, j, :3, :3]
            if rot_read(fl) and (np.abs(r @ r.T - np.eye(3)).max() > 2e-3 or abs(np.linalg.det(r) - 1) > 2e-3):
                not_rot.append((NAMES[k], n))
    mir_r = mir_p = mir_h = 0.0
    S = np.diag([-1.0, 1.0, 1.0])
    for k in range(0, 16, 2):
        if not (clean[k] and clean[k + 1]):
            continue
        for j, (_, n, fl) in enumerate(bl):
            a, b = mtx[k, swap[j], :3, :3], mtx[k + 1, j, :3, :3]
            if rot_read(fl):
                mir_r = max(mir_r, min(np.abs(A @ a @ B - b).max() for A in SIGNS for B in SIGNS))
            if pos_read(fl):
                mir_p = max(mir_p, np.abs(S @ mtx[k, swap[j], 3, :3] - mtx[k + 1, j, 3, :3]).max())
        mir_h = max(mir_h, np.abs(hands[k][::-1] - hands[k + 1]).max())
    return {"fill": fill, "not_rotations": not_rot, "mirror_rot": mir_r, "mirror_pos": mir_p,
            "mirror_hands": mir_h}


def to_json(path, bl):
    words, mtx, hands = read(path)
    out = []
    for k in range(16):
        if (words[k] == FILL).all():
            out.append({"name": NAMES[k], "fill": True})
            continue
        out.append({"name": NAMES[k], "fill_words": int((words[k] == FILL).sum()),
                    "bones": {n: {"rot": mtx[k, j, :3, :3].tolist(), "pos": mtx[k, j, 3, :3].tolist(),
                                  "read": ("rot" if rot_read(fl) else "") + ("pos" if pos_read(fl) else "")}
                              for j, (_, n, fl) in enumerate(bl)},
                    "hands": hands[k].tolist()})
    return out


def main():
    a = sys.argv[1:]
    dest = None
    if "--json" in a:
        dest = a[a.index("--json") + 1]
        del a[a.index("--json"):a.index("--json") + 2]
    prog = open(a[0], "rb").read()
    bl = bones(prog)
    if dest:
        with open(dest, "w") as f:
            json.dump({"bones": [{"index": i, "name": n, "flags": fl} for i, n, fl in bl],
                       "files": {os.path.basename(p)[7:-4]: to_json(p, bl) for p in a[1:]}}, f, indent=1)
        print(f"{len(a) - 1} files -> {dest}", file=sys.stderr)
        return
    print("rotations read: " + " ".join(n for _, n, fl in bl if rot_read(fl)))
    print("positions read: " + " ".join(n for _, n, fl in bl if pos_read(fl)))
    for p in a[1:]:
        c = check(p, bl)
        fills = ", ".join(f"{NAMES[k]} {'all' if f == BLOCK // 4 else f}" for k, f in enumerate(c["fill"]) if f)
        bad = sorted(set(c["not_rotations"]))
        print(f"{os.path.basename(p)[7:-4]:6} fill: {fills or 'none'}; read rotations not rotations: "
              f"{len(bad)}{' (' + ', '.join(sorted(set(b[0] for b in bad))) + ')' if bad else ''}; "
              f"mirror residual rot {c['mirror_rot']:.2g} pos {c['mirror_pos']:.2g} hands {c['mirror_hands']:.2g}")


if __name__ == "__main__":
    main()
