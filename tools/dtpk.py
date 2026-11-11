#!/usr/bin/env python3
"""Extract the waves in DTPK sound banks (bg_, se_, vo_) to WAV, each at the
playback rate the sound driver gives it.

Format (measured on all 80 banks, read from the driver aicadrv.bin; see
docs/asset-formats.md):
  +0x00 "DTPK", +0x04 track/bank number, +0x08 file size, +0x10 type word
  +0x20 eight section offsets; section 7 is the wave table:
    u32 last index (count - 1), then 16-byte records { A, B, C, D }:
      A bits 0-22   sample offset in the file (the AICA 23-bit start address)
      A bits 23-25  AICA channel register 0 bits 7-9 (A >> 16 is written to
                    it): PCMS 0 PCM16, 1 PCM8 (signed), 2 4-bit ADPCM, then
                    LPCTL (loop enable)
      B             loop start (low 16) and loop end (high 16), in samples
      C bit 7       stereo: the right channel follows the left, D bytes each
      D             bytes per channel
    the samples tile the file from the end of the table
  section 4, one program (type byte at +0, layer offsets at +8; see
    tools/dtpk_seq.py): a layer is a 0x40-byte header (u16 last tone index,
    velocity range at +2/+3) and 64-byte tones. Tone +2 (u16) is the wave: 0x100 + n is wave n of the
    bank, below 0x100 a built-in driver wave. Pitch (driver 0x4314, 0x33DC):
      P = 256 * (note - 60) + 256 * s8(tone[0xA]) + 4 * s8(tone[0xB])
          (+ track transpose / detune and LFO at run time)
      rate = 44100 * 2 ** (P / 3072)
  section 5, a second voice kind (driver 0x64A8, 0x6BCC, 0x6EFC): u32 last
    index, u32 offsets to entries; an entry is 8 bytes then entry[3]
    sub-records [len, ?, wave u16, key lo, key hi, ...]; sub + 0xC holds
    (count, offset) pairs for the parameters, each pointing at key-sorted
    points. Parameter 4 is the pitch, points [key, ?, u16 value, s16 slope]:
      rate = 44100 * 2 ** ((value - 0xC00) / 384)

A wave's rate is the one its tones give at note 60 (the most common, when
tones disagree; manifest.tsv lists every one). 57 waves (stage ambience) have
no tone in sections 4 and 5; their tones are in section 1, the program table
(tools/dtpk_seq.py decodes sections 0-3): u8 last bank, pad, u16 offsets
(from the section) of bank tables { u8 last program, pad, u16 offsets (from
the table) of programs }; a program is u8 type, ..., u16 layer offsets at
+8/+10/+12/+14 (from the program); a layer is 0x40 bytes then 64-byte
tones, for type 0 +0 u8 last tone and tone +0 its top note. Type 0 follows
the note (P gets 256 * (note - 60), driver 0x4320). All 57 sit in type 0
layers; where every tone of a layer gives the same rate at its "ladder
note" (the note above the previous tone's top; for the first tone, its top
- 11), the stage songs play the tones exactly there, so that rate is
written (source "ladder"); otherwise the rate at note 60 (source "note60",
not settled). No bank sets the wave link word +0x50 (driver 0x4518), so no
wave comes from another bank.

usage: dtpk.py <asset dir> <out dir> [--rate N]
"""
import glob
import os
import struct
import sys
import wave
from collections import Counter

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import aica_adpcm  # noqa: E402

FORMATS = {0: "pcm16", 1: "pcm8", 2: "adpcm"}
NOTE = 60  # the driver plays a tone at its own pitch at note 60 (0x3C)


def s8(x):
    return x - 256 if x & 0x80 else x


def sections(d):
    return struct.unpack_from("<8I", d, 0x20)


def waves(d):
    s7 = sections(d)[7]
    n = struct.unpack_from("<I", d, s7)[0] + 1
    for i in range(n):
        a, b, c, dlen = struct.unpack_from("<4I", d, s7 + 4 + 16 * i)
        yield {"index": i, "offset": a & 0x7FFFFF, "format": (a >> 23) & 3, "lpctl": (a >> 25) & 1,
               "loop_start": b & 0xFFFF, "loop_end": b >> 16, "stereo": bool(c & 0x80), "bytes": dlen}


def tone_rates(d):
    """(wave, rate, source) for every voice in sections 4 and 5 that uses a wave of this bank."""
    so = sections(d)
    out = []
    if so[4]:
        for p in range(struct.unpack_from("<I", d, so[4])[0] + 1):
            off = struct.unpack_from("<I", d, so[4] + 4 + 4 * p)[0]
            if not off:
                continue
            prog = so[4] + off
            for t in range(struct.unpack_from("<H", d, prog)[0] + 1):
                r = prog + 0x40 + 64 * t
                w = struct.unpack_from("<H", d, r + 2)[0]
                if w >= 0x100:
                    pitch = 256 * (NOTE - 60) + 256 * s8(d[r + 0xA]) + 4 * s8(d[r + 0xB])
                    out.append((w - 0x100, 44100 * 2 ** (pitch / 3072), f"p{p}t{t}"))
    if so[5]:
        for e in range(struct.unpack_from("<I", d, so[5])[0] + 1):
            ent = so[5] + struct.unpack_from("<I", d, so[5] + 4 + 4 * e)[0]
            sub = ent + 8
            for _ in range(d[ent + 3]):
                w = struct.unpack_from("<H", d, sub + 2)[0]
                if w >= 0x100:
                    out.append((w - 0x100, 44100 * 2 ** ((sec5_pitch(d, sub, NOTE) - 0xC00) / 384), f"s5e{e}"))
                sub += d[sub]
    return out


def section1_rates(d):
    """(wave, rate, source) for the bank waves of section 1 tones, for waves
    sections 4 and 5 do not rate. Type 0 layers of two or more such tones
    whose rates at their ladder notes agree give that rate ("ladder"); other
    tones give the rate at note 60 ("note60")."""
    s1 = sections(d)[1]
    out = []
    if not s1:
        return out
    seen = set()
    for b in range(d[s1] + 1):
        bo = struct.unpack_from("<H", d, s1 + 2 + 2 * b)[0]
        if not bo:
            continue
        bt = s1 + bo
        for p in range(d[bt] + 1):
            po = struct.unpack_from("<H", d, bt + 2 + 2 * p)[0]
            if not po:
                continue
            prog = bt + po
            for k in range(4):
                lo = struct.unpack_from("<H", d, prog + 8 + 2 * k)[0]
                if not lo or prog + lo in seen:
                    continue
                lay = prog + lo
                seen.add(lay)
                typ = d[prog]
                n = (struct.unpack_from("<H", d, lay)[0] if typ == 2 else d[lay]) + 1
                tones = [] if typ == 1 else [lay + 0x40 + 64 * i for i in range(n)]
                rows, prev = [], None
                for r in tones:
                    top, w = d[r], struct.unpack_from("<H", d, r + 2)[0]
                    ladder = top - 11 if prev is None else prev + 1
                    prev = top
                    pitch = 256 * s8(d[r + 0xA]) + 4 * s8(d[r + 0xB])
                    if w >= 0x100:
                        rows.append((w - 0x100, 44100 * 2 ** ((pitch + 256 * (ladder - 60)) / 3072),
                                     44100 * 2 ** (pitch / 3072)))
                same = typ == 0 and len(rows) > 1 and len({round(lr) for _, lr, _ in rows}) == 1
                for w, lr, r60 in rows:
                    out.append((w, lr, "ladder") if same else (w, r60, "note60"))
    return out


def sec5_pitch(d, sub, key):
    """Parameter 4 of a section 5 sub-record at a key (driver 0x6F48)."""
    table = sub + 0xC
    count, off = d[table + 8], d[table + 9]
    pt, lo = table + off, d[sub + 4]
    while key > d[pt] and count > 1:
        lo, count, pt = d[pt] + 1, count - 1, pt + 6
    value, slope = struct.unpack_from("<Hh", d, pt + 2)
    return value + ((slope * (key - lo)) >> 8)


def decode(data, fmt):
    if fmt == 0:
        return np.frombuffer(data[:len(data) & ~1], dtype="<i2").copy()
    if fmt == 1:
        return np.frombuffer(data, dtype=np.int8).astype(np.int16) * 256
    if fmt == 2:
        return aica_adpcm.decode(data)
    raise ValueError(f"format {fmt}")


def write_wav(path, chans, rate, loop=None):
    frames = np.stack(chans, axis=1) if len(chans) > 1 else chans[0]
    with wave.open(path, "wb") as w:
        w.setnchannels(len(chans))
        w.setsampwidth(2)
        w.setframerate(rate)
        w.writeframes(frames.astype("<i2").tobytes())
    if loop:  # append a RIFF 'smpl' chunk with the loop points
        start, end = loop
        smpl = struct.pack("<9I", 0, 0, int(1e9 / rate), 60, 0, 0, 0, 1, 0)
        smpl += struct.pack("<6I", 0, 0, start, end, 0, 0)
        with open(path, "r+b") as f:
            f.seek(0, 2)
            f.write(b"smpl" + struct.pack("<I", len(smpl)) + smpl)
            size = f.tell() - 8
            f.seek(4)
            f.write(struct.pack("<I", size))


def main():
    src, dest = sys.argv[1], sys.argv[2]
    fallback = int(sys.argv[sys.argv.index("--rate") + 1]) if "--rate" in sys.argv else 22050
    os.makedirs(dest, exist_ok=True)
    man = open(os.path.join(dest, "manifest.tsv"), "w")
    man.write("bank\twave\tformat\tstereo\tbytes\tsamples\tlpctl\tloop_start\tloop_end\t"
              "rate_written\trate_source\ttone_rates\n")
    total = unused = settled = section1 = 0
    for path in sorted(glob.glob(os.path.join(src, "bg_*.bin")) + glob.glob(os.path.join(src, "se_*.bin"))
                       + glob.glob(os.path.join(src, "vo_*.bin"))):
        d = open(path, "rb").read()
        bank = os.path.basename(path)[:-4]
        rates = {}
        for w, r, who in tone_rates(d):
            rates.setdefault(w, Counter())[round(r)] += 1
        sec1 = {}
        for w, r, src in section1_rates(d):
            if w not in rates:
                sec1.setdefault(w, Counter())[(round(r), src)] += 1
        os.makedirs(os.path.join(dest, bank), exist_ok=True)
        stream, stream_rates = [], Counter()
        for wv in waves(d):
            o, n = wv["offset"], wv["bytes"]
            chans = [decode(d[o:o + n], wv["format"])]
            if wv["stereo"]:
                chans.append(decode(d[o + n:o + 2 * n], wv["format"]))
            m = min(len(c) for c in chans)
            chans = [c[:m] for c in chans]
            if wv["index"] in rates:
                # ties go to the earliest tone (most_common keeps insertion
                # order): the character banks pair each tone with a variant
                # a semitone lower, and the first of each pair is the base
                rate, source = rates[wv["index"]].most_common(1)[0][0], "tone"
            elif wv["index"] in sec1:
                (rate, source), _ = sec1[wv["index"]].most_common(1)[0]
                section1 += 1
                settled += source == "ladder"
            else:
                rate, source = fallback, "none"
                unused += 1
            # LPCTL set: the hardware loops. One-shots set it too, on a
            # 24-sample tail loop the envelope cuts off; only real loops go
            # in the WAV
            tail = wv["loop_end"] - wv["loop_start"] == 24 and wv["loop_end"] == m - 4
            loop = (wv["loop_start"], min(wv["loop_end"], m - 1)) if wv["lpctl"] and not tail else None
            write_wav(os.path.join(dest, bank, f"{wv['index']:03d}_{FORMATS[wv['format']]}"
                                   f"{'_stereo' if wv['stereo'] else ''}.wav"), chans, rate, loop)
            if bank.startswith("bg_"):
                stream.append(chans[0])
                stream_rates[rate] += 1
            all_rates = ",".join(f"{r}x{k}" for r, k in sorted(rates.get(wv["index"], {}).items()))
            man.write(f"{bank}\t{wv['index']}\t{FORMATS[wv['format']]}\t{int(wv['stereo'])}\t{n}\t{m}\t"
                      f"{wv['lpctl']}\t{wv['loop_start']}\t{wv['loop_end']}\t{rate}\t{source}\t{all_rates}\n")
            total += 1
        if stream:  # music: the chunks in order, one continuous stream
            if len(stream_rates) > 1:
                print(f"{bank}: chunks at several rates {dict(stream_rates)}", file=sys.stderr)
            write_wav(os.path.join(dest, bank, "stream.wav"), [np.concatenate(stream)],
                      stream_rates.most_common(1)[0][0])
    man.close()
    print(f"{total} waves written to {dest}; {section1} rated by section 1 tones only ({settled} at their "
          f"ladder rate, {section1 - settled} at note 60); {unused} used by no program in their bank, "
          f"written at {fallback} Hz (see manifest.tsv)", file=sys.stderr)


if __name__ == "__main__":
    main()
