#!/usr/bin/env python3
"""Decode the motion banks (motdat_*.bin) to keyframe curves (JSON).

Format (read from the driver code in main.bin; see docs/asset-formats.md):
  main.bin 0x0C18BD18  21 records { motdat, smooth_ or 0, mothead or 0,
                       s16 first id, s16 end id }: motion ids are global
                       (CMN 1..1029, AKI 1030..1292, ...)
  motdat_EP.bin        loaded at boot (motep_load_addr); +8 is the directory,
                       0x18 bytes per motion id - 1 (0x0C092040): six u32
                       offsets into that motion's motdat_ bank:
    [0] s16 frame count
    [1] (bone, channel mask) byte pairs, bones ascending (0x0C091D40);
        mask bits 0x40 0x20 0x10 0x08 0x04 0x02 are channels 0-5,
        bone work +0x4C..+0x60
    [2] key counts: u8 from +1, or u16 from +2 when the first byte is 0
    [3] key times in frames: u8 when the motion is under 256 frames, else u16
    [4] key codes, 2 bits per key, high bits first
    [5] key data, halfwords
  motdat_<bank>.bin    u32 count, the motion names (sorted, NUL-ended), data
  mothead_<bank>.bin   u32 offset per motion of the bank (id - first) to a
                       move record (0x0C093640, 0x0C0659E0): +0x00 u32
                       flags, +0x04 / +0x05 bytes, +0x08 command list of
                       { u32 argument offset, u32 command } pairs ending in
                       0, run through the table at 0x0C1746D4 when the move
                       starts (+0x0C: a timed list for 0x0C1747A4); command
                       36 (0x0C066D20) sets the face motion
                       (an s16 motion id at the argument; the *_KAO motion
                       right after the body motion in every bank)
  A channel with count c has c + 2 keys: key 0 at frame 0, keys 1..c at the
  stored times, the last at frame count - 1 (0x0C091E60, 0x0C094A00). A
  key's code is how many halfwords it stores (0x0C094E20):
    0 nothing (value 0), 1 value, 2 tangent (in = out) then value,
    3 in tangent, out tangent, value
  Halfwords are IEEE half floats without denormals (0x0C094960). Between
  keys the value is a cubic Hermite with tangents per second (0x0C094EE0),
  time in seconds = frame / 30.

usage: motion.py <asset dir> <main.bin> <out dir>
"""
import json
import os
import struct
import sys

BASE = 0x0C020000
TABLE = 0x0C18BD18
CHANNELS = ("ch0", "ch1", "ch2", "ch3", "ch4", "ch5")  # mask 0x40 .. 0x02
MASKS = (0x40, 0x20, 0x10, 0x08, 0x04, 0x02)


def half(h):
    """0x0C094960: half -> float, no denormal / infinity handling."""
    if h == 0:
        return 0.0
    bits = (h & 0x8000) << 16 | (((h << 1) & 0xFFFF) << 12) + 0x38000000
    return struct.unpack("<f", struct.pack("<I", bits & 0xFFFFFFFF))[0]


def bank_table(prog):
    u = lambda a: struct.unpack_from("<I", prog, a - BASE)[0]
    cs = lambda a: prog[a - BASE:prog.index(b"\0", a - BASE)].decode() if a else None
    out = []
    for i in range(21):
        a = TABLE + 16 * i
        lo, hi = struct.unpack_from("<hh", prog, a + 12 - BASE)
        out.append({"motdat": cs(u(a)), "smooth": cs(u(a + 4)), "mothead": cs(u(a + 8)), "first": lo, "end": hi})
    return out


FACE_MOTION = 36  # move command: rob +0x3AC = s16 face motion id


def face_motions(head, first, end):
    """body motion id -> face motion id from a mothead_ file's command 36s."""
    u = lambda o: struct.unpack_from("<I", head, o)[0]
    out = {}
    for i in range(end - first):
        r = u(4 * i)
        if not r:
            continue
        c = u(r + 8)
        while c and u(c):
            if u(c + 4) == FACE_MOTION:
                out[first + i] = struct.unpack_from("<h", head, u(c))[0]
            c += 8
    return out


def names(d):
    n, o, out = struct.unpack_from("<I", d, 0)[0], 4, []
    for _ in range(n):
        e = d.index(b"\0", o)
        out.append(d[o:e].decode("latin1"))
        o = e + 1
    return out


def decode(d, rec):
    """One motion: rec = six offsets into bank d."""
    frames = struct.unpack_from("<h", d, rec[0])[0]
    bones = []
    for p in range(rec[1], rec[2] - 1, 2):  # the pairs fill [1]..[2]; 0-mask pairs are padding
        b, m = d[p], d[p + 1]
        if m == 0:
            break
        bones.append((b, m))
    wide = d[rec[2]] == 0
    cnt = rec[2] + (2 if wide else 1)
    times = rec[3]
    codes = rec[4]
    vals = rec[5]
    long_times = frames >= 0x100
    out = {}
    for b, m in bones:
        chans = {}
        for name, bit in zip(CHANNELS, MASKS):
            if not m & bit:
                continue
            if wide:
                c = struct.unpack_from("<H", d, cnt)[0]
                cnt += 2
            else:
                c = d[cnt]
                cnt += 1
            n = c + 2
            keys = []
            for k in range(n):
                code = d[codes + (k >> 2)] >> (6 - 2 * (k % 4)) & 3
                if k == 0:
                    t = 0
                elif k == n - 1:
                    t = frames - 1
                else:
                    t = (struct.unpack_from("<H", d, times + 2 * (k - 1))[0] if long_times else d[times + k - 1])
                hw = struct.unpack_from(f"<{code}H", d, vals) if code else ()
                vals += 2 * code
                if code == 0:
                    v, ti, to = 0.0, 0.0, 0.0
                elif code == 1:
                    v, ti, to = half(hw[0]), 0.0, 0.0
                elif code == 2:
                    v, ti, to = half(hw[1]), half(hw[0]), half(hw[0])
                else:
                    v, ti, to = half(hw[2]), half(hw[0]), half(hw[1])
                keys.append([t, round(v, 6), round(ti, 6), round(to, 6)])
            times += c * (2 if long_times else 1)
            codes += (n + 3) // 4
            chans[name] = keys
        out[b] = chans
    ends = {"counts": (cnt, rec[3]), "times": (times, rec[4]), "codes": (codes, rec[5])}
    return frames, bones, out, ends, vals


def sample(keys, frame):
    """Value of a channel at a frame (0x0C094A00 + 0x0C094EE0)."""
    t = frame / 30
    for a, b in zip(keys, keys[1:]):
        ta, tb = a[0] / 30, b[0] / 30
        if t <= tb or b is keys[-1]:
            if tb <= ta:
                return b[1]
            s = min(max((t - ta) / (tb - ta), 0.0), 1.0)
            s2, s3 = s * s, s * s * s
            return ((2 * s3 - 3 * s2 + 1) * a[1] + (3 * s2 - 2 * s3) * b[1]
                    + ((s3 - 2 * s2 + s) * a[3] + (s3 - s2) * b[2]) * (tb - ta))
    return keys[-1][1]


def main():
    src, prog_path, dest = sys.argv[1], sys.argv[2], sys.argv[3]
    prog = open(prog_path, "rb").read()
    ep = open(os.path.join(src, "motdat_EP.bin"), "rb").read()
    directory = os.path.join(src, "motdat_EP.bin")
    nrec = (struct.unpack_from("<I", ep, 4)[0] - 8) // 0x18
    os.makedirs(dest, exist_ok=True)
    total = nface = 0
    slack = {"counts": {}, "times": {}, "codes": {}}
    for bank in bank_table(prog):
        path = os.path.join(src, bank["motdat"])
        d = open(path, "rb").read()
        nm = names(d)
        faces = {}
        if bank["mothead"]:
            head = open(os.path.join(src, bank["mothead"]), "rb").read()
            faces = face_motions(head, bank["first"], bank["end"])
        motions = []
        for i, mid in enumerate(range(bank["first"], bank["end"])):
            rec = struct.unpack_from("<6I", ep, 8 + 0x18 * (mid - 1))
            frames, bones, curves, ends, vend = decode(d, rec)
            for k, (got, want) in ends.items():
                slack[k][want - got] = slack[k].get(want - got, 0) + 1
            m = {"id": mid, "name": nm[i] if i < len(nm) else None, "frames": frames,
                 "bones": {str(b): c for b, c in curves.items()}}
            if mid in faces:
                m["face"] = faces[mid]
            motions.append(m)
            total += 1
        with open(os.path.join(dest, bank["motdat"].replace(".bin", ".json")), "w") as f:
            json.dump({"bank": bank, "motions": motions}, f)
        nface += len(faces)
    print("stream end slack (bytes: motions):", {k: dict(sorted(v.items())) for k, v in slack.items()}, file=sys.stderr)
    print(f"{nface} motions with a face motion", file=sys.stderr)
    print(f"{total} motions from {nrec} directory records written to {dest} ({directory})", file=sys.stderr)


if __name__ == "__main__":
    main()
