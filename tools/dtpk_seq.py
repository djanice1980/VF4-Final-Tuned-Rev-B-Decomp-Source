#!/usr/bin/env python3
"""Decode the DTPK sections 0-3 of the sound banks (bg_, se_, vo_): channel
setups, the program table, velocity curves and the sequences, as the sound
driver aicadrv.bin (ARM7, loaded at AICA RAM 0) reads them. tools/dtpk.py
decodes the container, the wave table (section 7) and writes the WAVs; the
format notes are in docs/asset-formats.md. Driver addresses in [brackets].

Section 0, channel setups [0x11F4 at bank load, 0x12D8 for A0 19 n]:
  u8 count, u8 pad, u16 offset of the default setup (applied when the bank
  is loaded), u16 offsets of setups 0 .. count-1 (selected by A0 19 n); all
  offsets from the section start. A setup: u8 last part index (0xFF none),
  u8 bank level (0xFF keep), 6 bytes, then 32-byte part records:
    +0 flags (bit 1: the program is section 4 [0x3030]), +1 low nibble MIDI
    channel [0x13D0], +2 program bank, +3 program [0x3060, 0x3088], +4/+5
    note range, +6/+7 velocity range [0x38B0], +8 .. +15 copied into the
    channel state (+10 volume, +11 pan as CC 7 / CC 10 set them, +12 s7
    transpose in semitones, +13 s7 detune in 1/64 semitone [0x4330]).
Section 1, program table [0x3050]: u8 last bank, u8 pad, u16 offsets to bank
  tables (from the section start); a bank table is u8 last program, u8 pad,
  u16 offsets to programs (from the bank table). Section 4 is one more
  program, used by parts with flag bit 1 [0x3038].
Program: u8 type [0x395C], ..., u16 offsets of up to four layers at
  +8/+10/+12/+14 (from the program; 0 = none) [0x38F4]. Every layer sounds.
  Layer: a 0x40-byte header, tones of 64 bytes from +0x40. +6 velocity
  curve (section 2) [0x3F34].
    type 0 (keyboard): +0 u8 last tone, +2/+3 velocity range; the first tone
      whose +0 (top note) >= the note plays [0x3970-0x39D4]; pitch follows
      the note (note - 60) [0x4320].
    type 1 (note map): +0x40 low note, +0x41 high note, u16 offsets (from
      the layer) of one tone per note from +0x42 [0x3A14].
    type 2 (index): +0 u16 last tone; tone = note + 128 * ext, ext from the
      4-byte note-on below [0x39D8]. Types 1 and 2 do not follow the note.
  Tone: +2 u16 wave (0x100 + n: wave n of the bank's section 7, or of the
  bank named in header word +0x50 when its bit 31 is set [0x4518]; below
  0x100 a wave built into the driver [0x4458]), +0x0A s8 semitones, +0x0B s8
  1/64 semitones [0x4340], +0x20/+0x21 velocity range (types 1 and 2).
Section 2, velocity curves [0x2FD0]: u32 last index, then 128 bytes each.
Section 3, sequence table [0x7B00]: u8 last entry, 3 pad, then
  { u16 offset of a list (from the section), u8 id, u8 class }; a list is
  u8 last number, 3 pad, u32 offsets of scripts (from the section, 0 none).
  A request word {class 0xA8-0xAD, id, number} starts script `number` of
  the entry {class, id} in whichever loaded bank has it.
Script header [0x7B8C, 0x7D80]: byte h. h bit 7: an event stream follows
  (after [id, priority] when the next byte is below 0x80); bit 6 sends the
  events to the synthesizer, bit 5 to MIDI out [0x7320]; bit 4: starting it
  takes over a running stream of the same group (h & 15) [0x7C4C]. h bit 7
  clear: a song, [h, id, priority, pad] then a list of words [0x737C]:
  0x00oooooo play the stream at header + o, 0x80ssddvv send message ss dd vv,
  0x90oooooo continue the list at header + o (loops), other: end (play the
  stream at o first when o is not 0).
Stream events [0x7280]: MIDI-style with running status; 0xFF ends the
  stream. 8x n | 9x n v | Ax a b | Bx c v | Cx p | Ex b | Fx a b; Dx n v e
  in streams (h bit 7: note-on 9x n v with the 4-bit e added to the tone
  index, [0x72BC]), Dx p in songs. When the last data byte of an event has
  bit 7 set no delta follows; else a delta in ticks: one byte, or two when
  the first has bit 7 (((b0 & 0x7F) << 7) | b1). One tick is one AICA timer A
  period of 44 samples (0xD4 written to TIMA [0x15E0, 0x0C84]), 1002.27 Hz;
  the step per tick is fixed (0x10000 [0x7D88], no tempo command).

usage:
  dtpk_seq.py FILE                 dump setups, programs and sequences
  dtpk_seq.py FILE --wav OUT.wav [--seq CLASS:ID:N] [--loops K] [--tick SAMPLES]
                                   [--start-lag TICKS] [--mono]
                                   one sequence played through the banks' waves
                                   as the driver keys them (see synth); --mono
                                   mixes as the chip's mono bit does, --start-lag
                                   sets the start race (see render, default 0)
  dtpk_seq.py FILE --midi OUT.mid [--seq CLASS:ID:N] [--loops K] [--start-lag TICKS]
                                   one sequence (default: the first) to MIDI,
                                   following the sequences it starts; the events
                                   only (type 1 / 2 notes are tone numbers)
  dtpk_seq.py all DIR OUT          dump every bank to OUT/<bank>.txt
  dtpk_seq.py check DIR            the offline checks over all banks
"""
import glob
import math
import os
import struct
import sys
import wave
from collections import Counter, defaultdict

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import dtpk  # noqa: E402

TICK_SAMPLES = 44                  # 0x100 - 0xD4 [0x0C84]
TICK_HZ = 44100 / TICK_SAMPLES
MIDI_DIVISION = 441                # ticks per quarter note ...
MIDI_TEMPO = 440000                # ... and 0.44 s per quarter: 1 tick = 44 / 44100 s
REQUEST_CLASSES = range(0xA8, 0xAE)  # sent to the sequencer [0x0764, 0x47F4]
START_LAG = 0                      # ticks a start sent at its script's time 0 waits (see render)

# controllers the driver acts on [CC table 0x4DD4]; all others are ignored
CC_NAMES = {
    0x00: "program bank",          # part +2 [0x517C]
    0x01: "modulation",            # part +20, voices [0x5198]
    0x02: "breath",                # part +21, voices [0x51B4]
    0x07: "volume",                # part +10, voices [0x51D0, 0x3F20]
    0x27: "volume (no update)",    # part +10 only
    0x0A: "pan",                   # part +11 from table 0x7E4C, voices [0x51EC]
    0x2A: "pan (no update)",
    0x0C: "DSP send select",       # part +8 low nibble, AICA ISEL [0x5214, 0x50B4]
    0x2C: "DSP send select (no update)",
    0x0D: "DSP send level",        # part +8 high nibble, AICA IMXL [0x5248, 0x50E8]
    0x2D: "DSP send level (no update)",
    0x40: "damper",                # part +19, voices [0x527C, 0x511C]
    0x41: "mono",                  # part +33 bit 7, clears the note stack [0x5298]
    0x78: "all sound off",         # channel's voices [0x505C, 0x5154]
}
# A0 dd vv, driver control [synthesizer 0x4648 / table 0x46CC; sequencer
# 0x76A0 / table 0x76C4]; dd 0 takes a sub-code vv [0x4774, 0x7734]
A0_NAMES = {   # the codes traced; others are acted on but not named here
    0x02: "pause the bank's songs",       # slot +0 bit 0 [0x795C]
    0x03: "resume the bank's songs",      # [0x79AC]
    0x09: "fade-in rate for the next song",  # table 0x79FC [0x78A4, 0x7DC4]
    0x0A: "fade out the bank's songs",    # slot +0 |= 6, +0x20 = -rate [0x78DC, 0x7164]
    0x19: "select setup",                 # section 0 [0x4A40 -> 0x12D8]
    0x1C: "bank level",                   # bank record +6, voices [0x4A8C]; the fades send it [0x7218]
    0x20: "yield to a running copy",      # sequences only [0x7518]
    0x24: "set word 0xC3B0 once",         # sequences only [0x7620]
}
# A0 00 vv: voices off [synthesizer, table 0x4774] and sequences stopped
# [sequencer, map 0x7734, handlers 0x7750]; an A0 a sequence sends gets low
# byte bit 7 [0x7500] and then stops fewer sequences. "Setup parts" are the
# parts a selected setup adds; "resident parts" have flag bit 0 (the default
# setup's; voice flag 0x40 [0x3B18])
A0_SUB = {
    0x00: "no-op",
    0x01: "all banks: all voices off, sequences stopped",
    0x02: "all banks: voices of setup parts off, songs stopped",
    0x03: "all banks: voices of resident parts off, event streams stopped",
    0x0C: "route this sequence to the synthesizer",   # slot +1 |= 0x40 [0x75FC]
    0x0D: "route this sequence to MIDI out",          # |= 0x20 [0x7608]
    0x0E: "route this sequence to both",              # |= 0x60 [0x7614]
    0x11: "bank: all voices off, sequences stopped",
    0x12: "bank: voices of setup parts off, parts reset (host: songs stopped)",
    0x13: "bank: voices of resident parts off, event streams stopped",
    0x18: "no-op (18)", 0x19: "no-op (19)",
}


def u16(d, o):
    return struct.unpack_from("<H", d, o)[0]


def u32(d, o):
    return struct.unpack_from("<I", d, o)[0]


def s7(x):
    x &= 0x7F
    return x - 128 if x & 0x40 else x


class Event:
    """One stream event: absolute offset, raw bytes, status, data, delta after."""
    __slots__ = ("off", "raw", "status", "data", "delta", "running")

    def __init__(self, off, raw, status, data, delta, running):
        self.off, self.raw, self.status, self.data, self.delta, self.running = \
            off, raw, status, data, delta, running

    @property
    def message(self):
        """(status, d1, d2, ext) as the sequencer passes it on [0x729C-0x7300]."""
        st, dt = self.status, self.data
        kind = (st >> 4) & 7
        if kind == 5 and len(dt) == 3:          # 4-byte note-on: Dx -> 9x [0x72C4]
            return (st & ~0x40, dt[0], dt[1], dt[2])
        if kind == 6:                           # Ex: the byte goes to the d2 field [0x72E4]
            return (st, 0, dt[0], 0)
        return (st, dt[0], dt[1] if len(dt) > 1 else 0, 0)


class Bank:
    def __init__(self, path):
        self.path = path
        self.name = os.path.basename(path)[:-4]
        self.d = d = open(path, "rb").read()
        self.so = dtpk.sections(d)
        self.number = u32(d, 4)
        self.link = u32(d, 0x50)                 # [0x4518]: bit 31 = take waves from bank (bits 0-30)
        self.nwaves = u32(d, self.so[7]) + 1 if self.so[7] else 0
        self.problems = []
        self.owner = {}                          # section -> list of labels per byte
        for s in range(4):
            if self.so[s]:
                self.owner[s] = [None] * (self.end(s) - self.so[s])
        self.tones = {}                          # abs offset -> tone dict
        self.layers = {}
        self.programs = {}                       # abs offset -> program dict
        self.parse_sec0()
        self.parse_sec1()
        self.parse_sec2()
        self.parse_sec3()

    # -- helpers ------------------------------------------------------------
    def end(self, s):
        start = self.so[s]
        return min([x for x in self.so if x > start] + [len(self.d)])

    def section_of(self, off):
        for s in range(4):
            if self.so[s] and self.so[s] <= off < self.end(s):
                return s
        return None

    def mark(self, off, n, what):
        for o in range(off, off + n):
            s = self.section_of(o)
            if s is None:
                self.problems.append(f"{what} at {o:#x} outside sections 0-3")
                return
            own = self.owner[s]
            if own[o - self.so[s]] is None:
                own[o - self.so[s]] = what

    # -- section 0 ----------------------------------------------------------
    def parse_sec0(self):
        d, s0 = self.d, self.so[0]
        self.setups, self.default_setup = [], None
        if not s0:
            return
        n = d[s0]
        self.mark(s0, 4 + 2 * n, "setup table")
        self.default_setup = self.parse_setup(s0 + u16(d, s0 + 2)) if u16(d, s0 + 2) else None
        for i in range(n):
            o = u16(d, s0 + 4 + 2 * i)
            self.setups.append(self.parse_setup(s0 + o) if o else None)

    def parse_setup(self, off):
        d = self.d
        last = d[off]
        parts = []
        n = 0 if last == 0xFF else last + 1
        self.mark(off, 8 + 32 * n, "setup")
        for i in range(n):
            p = off + 8 + 32 * i
            parts.append({"off": p, "flags": d[p], "channel": d[p + 1] & 15, "bank": d[p + 2],
                          "program": d[p + 3], "notes": (d[p + 4], d[p + 5]), "vels": (d[p + 6], d[p + 7]),
                          "volume": d[p + 10], "pan": d[p + 11], "transpose": s7(d[p + 12]),
                          "detune": s7(d[p + 13]), "raw": d[p:p + 32]})
        return {"off": off, "level": d[off + 1], "parts": parts}

    # -- programs (sections 1 and 4) ----------------------------------------
    def parse_sec1(self):
        d, s1 = self.d, self.so[1]
        self.prog_table = {}                     # (bank, program) -> abs offset
        if not s1:
            return
        nb = d[s1] + 1
        self.mark(s1, 2 + 2 * nb, "program banks")
        for b in range(nb):
            o = u16(d, s1 + 2 + 2 * b)
            if not o:
                continue
            bt = s1 + o
            np_ = d[bt] + 1
            self.mark(bt, 2 + 2 * np_, "program table")
            for p in range(np_):
                po = u16(d, bt + 2 + 2 * p)
                if po:
                    self.prog_table[(b, p)] = bt + po
                    self.parse_program(bt + po)
        self.sec4 = self.parse_program(self.so[4], mark=False) if self.so[4] else None

    def parse_program(self, off, mark=True):
        if off in self.programs:
            return self.programs[off]
        d = self.d
        prog = {"off": off, "type": d[off], "layers": []}
        self.programs[off] = prog
        if mark:
            self.mark(off, 16, "program")
        for i in range(4):
            lo = u16(d, off + 8 + 2 * i)
            if lo:
                prog["layers"].append(self.parse_layer(off + lo, prog["type"], mark))
        return prog

    def parse_layer(self, off, typ, mark):
        key = (off, typ)
        if key in self.layers:
            return self.layers[key]
        d = self.d
        lay = {"off": off, "type": typ, "curve": d[off + 6], "vels": (d[off + 2], d[off + 3]), "tones": []}
        self.layers[key] = lay
        if mark:
            self.mark(off, 0x40, "layer")
        if typ == 1:
            lo, hi = d[off + 0x40], d[off + 0x41]
            lay["notes"] = (lo, hi)
            if mark:
                self.mark(off + 0x40, 2 + 2 * max(0, hi - lo + 1), "note map")
            lay["map"] = []
            for k in range(max(0, hi - lo + 1)):
                to = u16(d, off + 0x42 + 2 * k)
                lay["map"].append(self.parse_tone(off + to, mark) if to else None)
            lay["tones"] = [t for t in lay["map"] if t]
        else:
            n = (u16(d, off) if typ == 2 else d[off]) + 1
            for i in range(n):
                lay["tones"].append(self.parse_tone(off + 0x40 + 64 * i, mark))
        return lay

    def parse_tone(self, off, mark):
        if off not in self.tones:
            d = self.d
            if off + 64 > len(d):
                self.problems.append(f"tone at {off:#x} past the end of the file")
                return None
            self.tones[off] = {"off": off, "top": d[off], "wave": u16(d, off + 2), "semi": dtpk.s8(d[off + 0xA]),
                               "fine": dtpk.s8(d[off + 0xB]), "vels": (d[off + 0x20], d[off + 0x21])}
        if mark:
            self.mark(off, 64, "tone")
        return self.tones[off]

    def program_for(self, part, bank=None, number=None):
        """The program a part plays [0x3020]."""
        if part["flags"] & 2:
            return self.sec4
        b = part["bank"] if bank is None else bank
        p = part["program"] if number is None else number
        o = self.prog_table.get((b, p))
        return self.programs[o] if o else None

    # -- section 2 ----------------------------------------------------------
    def parse_sec2(self):
        d, s2 = self.d, self.so[2]
        self.ncurves = 0
        if s2:
            self.ncurves = u32(d, s2) + 1
            self.mark(s2, 4 + 128 * self.ncurves, "velocity curves")

    # -- section 3 ----------------------------------------------------------
    def parse_sec3(self):
        d, s3 = self.d, self.so[3]
        self.entries = []
        self.streams = {}                        # abs offset -> (events, end, problem)
        if not s3:
            return
        n = d[s3] + 1
        self.mark(s3, 4 + 4 * n, "sequence table")
        for i in range(n):
            e = s3 + 4 + 4 * i
            lo, sid, cls = u16(d, e), d[e + 2], d[e + 3]
            L = s3 + lo
            nl = d[L] + 1
            self.mark(L, 4 + 4 * nl, "sequence list")
            scripts = []
            for k in range(nl):
                o = u32(d, L + 4 + 4 * k)
                scripts.append(self.parse_script(s3 + o, (cls, sid, k)) if o else None)
            self.entries.append({"class": cls, "id": sid, "list": L, "scripts": scripts})

    def find_script(self, cls, sid, num):
        for e in self.entries:
            if e["class"] == cls and e["id"] == sid:
                return e["scripts"][num] if num < len(e["scripts"]) else None
        return None

    def parse_script(self, off, key):
        d = self.d
        h = d[off]
        sc = {"off": off, "key": key, "header": h, "items": [], "problems": []}
        if h & 0x80:                             # event stream [0x7B9C]
            if d[off + 1] & 0x80:
                sc["id"], sc["prio"], start = 0, 0, off + 1
            else:
                sc["id"], sc["prio"], start = d[off + 1], d[off + 2], off + 3
            self.mark(off, start - off, "script header")
            sc["items"].append(("stream", start))
            self.parse_stream(start, True)
            return sc
        sc["id"] = d[off + 1]                     # song [0x7C88]
        sc["prio"] = d[off + 2] if sc["id"] else 0
        self.mark(off, 4, "script header")
        pos, seen = off + 4, set()
        while True:
            if pos in seen:
                sc["items"].append(("loop", pos))
                break
            if pos + 4 > self.end(3):
                sc["problems"].append(f"list runs past section 3 at {pos:#x}")
                break
            seen.add(pos)
            w = u32(d, pos)
            self.mark(pos, 4, "song list")
            top, low = w >> 24, w & 0xFFFFFF
            if top == 0x00:
                sc["items"].append(("stream", off + low))
                self.parse_stream(off + low, False)
                pos += 4
            elif top == 0x80:
                sc["items"].append(("message", ((low >> 16) & 0xFF, (low >> 8) & 0xFF, low & 0xFF)))
                pos += 4
            elif top == 0x90:
                sc["items"].append(("jump", off + low))
                pos = off + low
            else:
                if low:
                    sc["items"].append(("stream", off + low))
                    self.parse_stream(off + low, False)
                sc["items"].append(("end", top))
                break
        return sc

    def parse_stream(self, off, form7):
        key = (off, form7)
        if key in self.streams:
            return self.streams[key]
        d, limit = self.d, self.end(3)
        events, status, p, problem = [], None, off, None
        while True:
            if p >= limit:
                problem = f"stream at {off:#x} runs past section 3"
                break
            b = d[p]
            if b == 0xFF:
                self.mark(p, 1, "stream end")
                p += 1
                break
            start = p
            running = not (b & 0x80)
            if not running:
                status = b
                p += 1
            elif status is None:
                problem = f"data byte {b:#04x} at {p:#x} with no status (the driver would use a stale one)"
                break
            kind = (status >> 4) & 7
            n = 1
            if kind in (1, 2, 3, 7):
                n = 2
            elif kind == 5 and form7:
                n = 3
            if p + n > limit:
                problem = f"event at {start:#x} runs past section 3"
                break
            raw_data = d[p:p + n]
            p += n
            delta = 0
            if not raw_data[-1] & 0x80:
                b0 = d[p]
                p += 1
                if b0 & 0x80:
                    delta = ((b0 << 7) | d[p]) & ~0x4000
                    p += 1
                else:
                    delta = b0
            self.mark(start, p - start, "event")
            events.append(Event(start, d[start:p], status, [x & 0x7F for x in raw_data], delta, running))
        self.streams[key] = (events, p, problem)
        return self.streams[key]

    # -- description --------------------------------------------------------
    def describe(self, st, data, form7):
        hi, ch = st & 0xF0, st & 15
        if hi == 0x80:
            return f"ch{ch:2d} note off {data[0]}"
        if hi == 0x90:
            return f"ch{ch:2d} note on {data[0]} vel {data[1]}" if data[1] else f"ch{ch:2d} note off {data[0]} (vel 0)"
        if hi == 0xD0:
            if form7:
                ix = data[0] + 128 * (data[2] & 15)
                return f"ch{ch:2d} note on {data[0]} vel {data[1]} ext {data[2]} (type 2 tone {ix})"
            return f"ch{ch:2d} pressure {data[0]} (ignored)"
        if hi == 0xB0:
            return f"ch{ch:2d} cc {data[0]} {CC_NAMES.get(data[0], 'ignored')} = {data[1]}"
        if hi == 0xC0:
            return f"ch{ch:2d} program {data[0]}"
        if hi == 0xE0:
            return f"ch{ch:2d} pitch bend {data[0] - 64:+d}"
        if hi == 0xF0:
            return f"{st:02x} {data[0]:02x} {data[1]:02x} (ignored)"
        # Ax: driver classes
        if st == 0xA0:
            if data[0] == 0:
                return f"A0 00 {data[1]:02x} {A0_SUB.get(data[1], 'sub-code (ignored)')}"
            name = A0_NAMES.get(data[0], "(not traced)")
            return f"A0 {data[0]:02x} {data[1]:02x} {name}"
        if st in REQUEST_CLASSES:
            where = "" if self.find_script(st, data[0], data[1]) is not None else " (not in this bank)"
            return f"start {st:02x}:{data[0]:02x}:{data[1]}{where}"
        return f"{st:02x} {data[0]:02x} {data[1]:02x} (driver class {st:02x})"

    def dump(self, out):
        d, so = self.d, self.so
        w = out.write
        w(f"{self.name}: number {self.number}, type {u32(d, 0x10):#010x}, sections "
          + " ".join(f"{x:x}" for x in so) + f", {self.nwaves} waves\n")
        if self.link:
            w(f"  wave link word +0x50 = {self.link:#x}\n")
        w("\nsection 0, setups\n")
        for label, st in [("default", self.default_setup)] + list(enumerate(self.setups)):
            if not st:
                w(f"  setup {label}: none\n")
                continue
            w(f"  setup {label} @{st['off']:#x}: level {st['level']:#x}\n")
            for p in st["parts"]:
                prog = self.program_for(p)
                src = "section 4" if p["flags"] & 2 else f"program {p['bank']}:{p['program']}"
                w(f"    ch{p['channel']:2d} flags {p['flags']:02x} {src}"
                  f"{'' if prog else ' (missing)'} notes {p['notes'][0]}-{p['notes'][1]} vels "
                  f"{p['vels'][0]}-{p['vels'][1]} vol {p['volume']} pan {p['pan']} transpose "
                  f"{p['transpose']} detune {p['detune']}  raw {p['raw'][8:16].hex(' ')}\n")
        w(f"\nsection 1, programs ({len(self.prog_table)}); section 4: "
          f"{'type %d' % self.sec4['type'] if self.sec4 else 'none'}\n")
        items = [(f"{b}:{p}", self.programs[o]) for (b, p), o in sorted(self.prog_table.items())]
        if self.sec4:
            items.append(("sec4", self.sec4))
        for label, prog in items:
            w(f"  program {label} @{prog['off']:#x} type {prog['type']}\n")
            for lay in prog["layers"]:
                extra = f" notes {lay['notes'][0]}-{lay['notes'][1]}" if lay["type"] == 1 else ""
                w(f"    layer @{lay['off']:#x} vels {lay['vels'][0]}-{lay['vels'][1]} curve {lay['curve']}"
                  f"{extra}, {len(lay['tones'])} tones\n")
                for i, t in enumerate(lay["tones"]):
                    wv = t["wave"]
                    wn = f"wave {wv - 0x100}" if wv >= 0x100 else f"driver wave {wv}"
                    w(f"      {i:3d} top {t['top']:3d} {wn} pitch {t['semi']:+d}{t['fine']:+d}/64 "
                      f"rate {round(rate_of(t))} vels {t['vels'][0]}-{t['vels'][1]}\n")
        w(f"\nsection 2: {self.ncurves} velocity curves\n")
        w(f"\nsection 3, sequences ({len(self.entries)} entries)\n")
        for e in self.entries:
            w(f"  {e['class']:02x}:{e['id']:02x} list @{e['list']:#x}, {len(e['scripts'])} scripts\n")
            for k, sc in enumerate(e["scripts"]):
                if sc is None:
                    w(f"    {k}: none\n")
                    continue
                form7 = bool(sc["header"] & 0x80)
                w(f"    {e['class']:02x}:{e['id']:02x}:{k} @{sc['off']:#x} header {sc['header']:02x} "
                  f"({'stream' if form7 else 'song'}) id {sc['id']} priority {sc['prio']}\n")
                for item in sc["items"]:
                    kind, arg = item
                    if kind == "stream":
                        events, end, problem = self.streams[(arg, form7)]
                        w(f"      stream @{arg:#x}, {len(events)} events\n")
                        t = 0
                        for ev in events:
                            st, d1, d2, ext = ev.message
                            w(f"        {t:7d} {ev.raw.hex(' '):<18s} "
                              f"{self.describe(ev.status, ev.data, form7)}\n")
                            t += ev.delta
                        w(f"        {t:7d} end\n")
                        if problem:
                            w(f"        PROBLEM {problem}\n")
                    elif kind == "message":
                        st, a, b = arg
                        w(f"      send {st:02x} {a:02x} {b:02x}  {self.describe(st, [a, b], False)}\n")
                    elif kind == "jump":
                        w(f"      jump to list @{arg:#x}\n")
                    elif kind == "loop":
                        w(f"      (loops back to @{arg:#x})\n")
                    elif kind == "end":
                        w(f"      end (word top byte {arg:#04x})\n")
                for pr in sc["problems"]:
                    w(f"      PROBLEM {pr}\n")
        for pr in self.problems:
            w(f"PROBLEM {pr}\n")


def rate_of(tone, note=60, part_semi=0, part_fine=0, keyboard=False):
    """AICA playback rate of a tone [0x4314, 0x33DC]: P in 1/256 semitone."""
    p = 256 * tone["semi"] + 4 * tone["fine"] + 256 * part_semi + 4 * part_fine
    if keyboard:
        p += 256 * (note - 60)
    return 44100 * 2 ** (p / 3072)


# -- MIDI -----------------------------------------------------------------
def vlq(n):
    out = [n & 0x7F]
    n >>= 7
    while n:
        out.append(0x80 | (n & 0x7F))
        n >>= 7
    return bytes(reversed(out))


def render(banks, bank, key, loops=1, max_ticks=10 * 60 * int(TICK_HZ), start_lag=START_LAG):
    """Run the sequencer from one request: every script it starts (in any of
    the given banks) runs in its own slot, as the driver's 16 slots do
    [0x71D0]. Returns [(tick, kind, payload)] sorted. A script started more
    than `loops` + 1 times, or a song list that jumps back more than `loops`
    times, stops there (the driver would loop forever).

    A start request sent at its script's own time 0 starts its script
    `start_lag` ticks later. The start routine runs a new script's first
    events at once [0x7D80 -> 0x71D0], wherever the main loop is; their
    messages go to the ring [0x2B1C], which the main loop drains only at the
    top of its next pass [0x668]. If a timer tick [0x7108] falls before that
    drain, the started script counts from after it. Later events fire in the
    slot loop just after the tick that made them due, so their requests reach
    the drain within the same tick."""
    out = []
    starts = Counter()

    def start(cls, sid, num, t0, depth):
        for b in [bank] + [x for x in banks if x is not bank]:
            sc = b.find_script(cls, sid, num)
            if sc is not None:
                break
        else:
            out.append((t0, "text", f"start {cls:02x}:{sid:02x}:{num} not found"))
            return
        k = (b.name, cls, sid, num)
        starts[k] += 1
        if starts[k] > loops + 1 or depth > 64:
            out.append((t0, "marker", f"loop: {cls:02x}:{sid:02x}:{num}"))
            return
        out.append((t0, "marker", f"start {cls:02x}:{sid:02x}:{num}"))
        run_script(b, sc, t0, depth)

    def run_stream(b, off, form7, t, depth, t0):
        events, end, problem = b.streams[(off, form7)]
        for ev in events:
            if t > max_ticks:
                return t
            st, d1, d2, ext = ev.message
            hi = st & 0xF0
            if st in REQUEST_CLASSES:
                pending.append((st, d1, d2, t + (start_lag if t == t0 else 0), depth + 1))
            elif hi in (0x80, 0x90):
                out.append((t, "note", (st, d1, d2, ext, b, form7, ev)))
            elif hi in (0xB0, 0xC0, 0xE0):
                out.append((t, "midi", (st, d1, d2)))
            else:
                if st == 0xA0:
                    out.append((t, "a0", (d1, d2, b)))
                out.append((t, "text", b.describe(ev.status, ev.data, form7)))
            t += ev.delta
        return t

    def run_script(b, sc, t, depth):
        t0 = t
        if sc["header"] & 0x80:
            t = run_stream(b, sc["items"][0][1], True, t, depth, t0)
        else:                                    # walk the song list as the driver does [0x737C]
            pos, visits = sc["off"] + 4, Counter()
            while t <= max_ticks:
                visits[pos] += 1
                if visits[pos] > loops + 1:
                    out.append((t, "marker", "loop"))
                    break
                w = u32(b.d, pos)
                top, low = w >> 24, w & 0xFFFFFF
                if top == 0x00:
                    t = run_stream(b, sc["off"] + low, False, t, depth, t0)
                    pos += 4
                elif top == 0x80:
                    m = ((low >> 16) & 0xFF, (low >> 8) & 0xFF, low & 0xFF)
                    if m[0] == 0xA0:
                        out.append((t, "a0", (m[1], m[2], b)))
                    out.append((t, "text", "send " + b.describe(m[0], [m[1], m[2]], False)))
                    pos += 4
                elif top == 0x90:
                    pos = sc["off"] + low
                else:
                    if low:
                        t = run_stream(b, sc["off"] + low, False, t, depth, t0)
                    break
        out.append((t, "marker", f"end {sc['key'][0]:02x}:{sc['key'][1]:02x}:{sc['key'][2]}"))

    pending = [key + (0, 0)]
    while pending:
        cls, sid, num, t0, depth = pending.pop(0)
        start(cls, sid, num, t0, depth)
    return sorted(out, key=lambda x: x[0])


def write_midi(path, events, title):
    trk = bytearray()
    last = 0

    def ev(t, data):
        nonlocal last
        trk.extend(vlq(t - last))
        trk.extend(data)
        last = t

    def meta(t, typ, text):
        b = text.encode("latin1", "replace")
        ev(t, bytes([0xFF, typ]) + vlq(len(b)) + b)

    meta(0, 0x03, title)
    ev(0, b"\xFF\x51\x03" + MIDI_TEMPO.to_bytes(3, "big"))
    sounding = {}                                 # (ch, note) -> True
    end = 0
    for t, kind, payload in events:
        end = max(end, t)
        if kind == "note":
            st, d1, d2, ext, b, form7, e = payload
            ch = st & 15
            if st & 0xF0 == 0x90 and d2:
                if (ch, d1) in sounding:
                    ev(t, bytes([0x80 | ch, d1, 0]))
                if ext:
                    meta(t, 0x01, f"ch{ch} note {d1} ext {ext}: tone {d1 + 128 * (ext & 15)}")
                ev(t, bytes([0x90 | ch, d1, d2]))
                sounding[(ch, d1)] = True
            else:
                if (ch, d1) in sounding:
                    ev(t, bytes([0x80 | ch, d1, 0]))
                    del sounding[(ch, d1)]
        elif kind == "midi":
            st, d1, d2 = payload
            if st & 0xF0 == 0xC0:
                ev(t, bytes([st, d1]))
            elif st & 0xF0 == 0xE0:
                ev(t, bytes([st, 0, d2]))
            else:
                ev(t, bytes([st, d1, d2]))
        elif kind != "a0":                        # A0 also has a text event
            meta(t, 0x06 if kind == "marker" else 0x01, payload)
    # notes the driver leaves to the envelope: close them one tick after the end
    for ch, n in list(sounding):
        ev(end + 1, bytes([0x80 | ch, n, 0]))
    ev(end + 1, b"\xFF\x2F\x00")
    with open(path, "wb") as f:
        f.write(b"MThd" + struct.pack(">IHHH", 6, 0, 1, MIDI_DIVISION))
        f.write(b"MTrk" + struct.pack(">I", len(trk)) + trk)


# -- WAV ------------------------------------------------------------------
# What the driver does at a note-on, and the AICA as Flycast models it
# (core/hw/aica/sgc_if.cpp, the emulator the live checks use). Envelope
# times in ms per effective rate 0-63; levels in steps of 1/16 octave.
AEG_ATTACK_MS = [
    -1, -1, 8100.0, 6900.0, 6000.0, 4800.0, 4000.0, 3400.0, 3000.0, 2400.0, 2000.0, 1700.0, 1500.0,
    1200.0, 1000.0, 860.0, 760.0, 600.0, 500.0, 430.0, 380.0, 300.0, 250.0, 220.0, 190.0, 150.0, 130.0,
    110.0, 95.0, 76.0, 63.0, 55.0, 47.0, 38.0, 31.0, 27.0, 24.0, 19.0, 15.0, 13.0, 12.0, 9.4, 7.9, 6.8,
    6.0, 4.7, 3.8, 3.4, 3.0, 2.4, 2.0, 1.8, 1.6, 1.3, 1.1, 0.93, 0.85, 0.65, 0.53, 0.44, 0.40, 0.35,
    0.0, 0.0]
AEG_DSR_MS = [
    -1, -1, 118200.0, 101300.0, 88600.0, 70900.0, 59100.0, 50700.0, 44300.0, 35500.0, 29600.0, 25300.0,
    22200.0, 17700.0, 14800.0, 12700.0, 11100.0, 8900.0, 7400.0, 6300.0, 5500.0, 4400.0, 3700.0, 3200.0,
    2800.0, 2200.0, 1800.0, 1600.0, 1400.0, 1100.0, 920.0, 790.0, 690.0, 550.0, 460.0, 390.0, 340.0,
    270.0, 230.0, 200.0, 170.0, 140.0, 110.0, 98.0, 85.0, 68.0, 57.0, 49.0, 43.0, 34.0, 28.0, 25.0,
    22.0, 18.0, 14.0, 12.0, 11.0, 8.5, 7.1, 6.1, 5.4, 4.3, 3.6, 3.1]
SEND_LEVEL = [255] + [(15 - i) << 3 for i in range(1, 16)]   # DISDL / DIPAN level -> attenuation
TL_LUT = [int(32768 / 2 ** (i / 16)) for i in range(256)]     # index 255 rounds to 0: silent
EG_ONE = 1 << 16
EG_ALL = 1024 * EG_ONE - 1
VOICES = 64                        # AICA channels
FREE_LEVEL = 32 << 4               # a looped voice past attack is freed above this AEG level,
                                   # 48 dB down (work +6 = 32 [0x17C8], read at 0xA94)


def eg_steps(ms):
    """Envelope increment per sample for a decay / release time."""
    if ms < 0:
        return 0
    return EG_ALL if ms == 0 else round(EG_ALL / (44.1 * ms))


def eg_attack(ms):
    if ms < 0:
        return 0
    return EG_ONE if ms == 0 else round(EG_ONE / (1 - 1 / 0x280 ** (1 / (44.1 * ms))))


class Voice:
    """One AICA channel the driver keys on for a tone [0x3884-0x3CE0]."""

    def __init__(self, b, part, prog, lay, tone, note, vel, start, level, stats):
        d, t = b.d, tone["off"]
        self.bank, self.ch, self.note, self.start = b.name, part["channel"], note, start
        self.resident = bool(part["flags"] & 1)
        self.oneshot = bool(d[t + 1] & 0x80)      # voice flag 8 [0x3B24]: keyed off at the loop start [0xA74]
        self.keyoff = self.stop = self.end = None
        w = next(x for x in dtpk.waves(d) if x["index"] == tone["wave"] - 0x100)
        if w["stereo"]:
            stats["voices on stereo waves (left half played)"] += 1
        self.wave = dtpk.decode(d[w["offset"]:w["offset"] + w["bytes"]], w["format"]).astype(np.float64)
        self.lsa, self.lea, self.lpctl = w["loop_start"], w["loop_end"], w["lpctl"]
        if u16(d, t + 4):
            stats["tones with a start offset (ignored)"] += 1
        # pitch [0x4314]: P in 1/256 semitone; the chip takes OCT / FNS, FNS
        # from the table at 0x8050 [0x33DC], which is this rounded (all 768)
        p = 256 * tone["semi"] + 4 * tone["fine"] + 256 * part["transpose"] + 4 * part["detune"]
        if prog["type"] == 0:
            p += 256 * (note - 60)
        oct_ = p // 3072
        fns = round(1024 * (2 ** ((p - 3072 * oct_) / 3072) - 1))
        self.step = 2 ** oct_ * (1024 + fns) / 1024
        # level [0x4270]: velocity curve x tone +8 x part volume x channel and
        # master levels (taken as 127) x bank level for setup parts; TL from it
        curve = layer_curve(b, lay["curve"])
        lv = curve[vel]
        for f in ((d[t + 8] >> 1) * 2, part["volume"] * 2, 127 * 2, 127 * 2) + (() if self.resident else (level * 2,)):
            lv = (lv * f) >> 8
            if lv:
                lv += 1
        tl = lv ^ 0xFF
        if tl != 0xFF:
            tl >>= 1
        # pan and direct level: tone +6 / +7, or the part's when bit 7 is set [0x43C0, 0x3C74]
        pan = part["pan"] & 31 if d[t + 6] & 0x80 else d[t + 6] & 31
        if not d[t + 6] & 0x80 and d[lay["off"] + 0x28] & 31:
            stats["voices with a pan pattern (ignored)"] += 1
        disdl = part["raw"][9] & 15 if d[t + 7] & 0x80 else d[t + 7] & 15
        full = tl + SEND_LEVEL[disdl]
        side = full + SEND_LEVEL[~pan & 15]
        self.att = (full, side) if pan & 0x10 else (side, full)
        # envelope: tone +0x0C -> AR / D1R / D2R, +0x0E -> RR / DL / KRS / LPSLNK [0x3B8C]
        a, r = u16(d, t + 0xC), u16(d, t + 0xE)
        krs = (r >> 10) & 15

        def eff(rate):
            e = 2 * rate
            if krs < 15:
                e += (fns >> 9) & 1
                e += max(0, (krs + ((oct_ & 15) ^ 8) - 8) * 2)
            return min(e, 63)
        self.ar = eg_attack(AEG_ATTACK_MS[eff(a & 31)])
        self.d1r = eg_steps(AEG_DSR_MS[eff((a >> 6) & 31)])
        self.d2r = eg_steps(AEG_DSR_MS[eff((a >> 11) & 31)])
        self.rr = eg_steps(AEG_DSR_MS[eff(r & 31)])
        self.dl = ((r >> 5) & 31) << 5
        if r & 0x4000:
            stats["tones with LPSLNK (ignored)"] += 1
        lfo = u16(d, t + 0x30)
        if lfo & 7 or (lfo >> 5) & 7:
            stats["tones with an LFO depth (ignored)"] += 1
        if u16(d, t + 0x10) & 0x7FF:
            stats["tones with the filter envelope (ignored)"] += 1
        dsp = part["raw"][8] if d[t + 0x1E] & 0x80 else d[t + 0x1F]
        if dsp & 0xF0:
            stats["voices sent to the DSP (effects not rendered)"] += 1

    def hits_loop(self):
        """Samples from the start until the play position reaches the loop start."""
        return math.ceil(self.lsa / self.step)

    def envelope(self, n):
        """AEG value (0 .. 0x3FF, attenuation) of the first n samples, and the
        sample where the voice ends (release or decay done) if before n."""
        out = np.empty(n, dtype=np.int64)
        val, state, i = 0x280 << 16, 0, 0
        ko = self.keyoff - self.start if self.keyoff is not None else n
        while i < n:
            if state < 3 and i >= ko:
                state = 3
            lim = n if state == 3 else min(n, ko)
            if state == 0:                         # attack: exponential, per sample
                if self.ar == 0:
                    out[i:lim] = val >> 16
                    i = lim
                    continue
                while i < lim:
                    out[i] = val >> 16
                    val -= ((val << 16) // self.ar) + 1
                    i += 1
                    if val >> 16 <= 0:
                        val, state = 0, 1
                        break
                continue
            rate, top = {1: (self.d1r, self.dl), 2: (self.d2r, 0x3FF), 3: (self.rr, 0x3FF)}[state]
            if rate == 0:
                out[i:lim] = val >> 16
                i = lim
                continue
            k = max(1, -(-((top << 16) - val) // rate))   # steps until the level is reached
            kf = max(1, -(-((FREE_LEVEL + 1 << 16) - val) // rate)) if self.lpctl else n + 1
            m = min(k, kf, lim - i)
            out[i:i + m] = (val + rate * np.arange(m, dtype=np.int64)) >> 16
            val += rate * m
            i += m
            if m == kf:
                return out[:i], i                  # the driver frees the voice [0xA7C -> 0x1BEC]
            if m == k:
                if state == 1:
                    state = 2
                else:
                    return out[:i], i              # Flycast disables the channel at 0x3FF
        return out, None

    def render(self, n):
        env, end = self.envelope(n)
        n = len(env)
        pos = np.arange(n, dtype=np.float64) * self.step
        if self.lpctl and self.lea > self.lsa:
            over = pos >= self.lea
            pos[over] = self.lsa + np.mod(pos[over] - self.lsa, self.lea - self.lsa)
        else:                                      # no loop: the channel stops at the loop end
            n = min(n, int(np.searchsorted(pos, self.lea)))
            pos, env = pos[:n], env[:n]
        i0 = np.minimum(pos.astype(np.int64), len(self.wave) - 1)
        i1 = i0 + 1
        if self.lpctl and self.lea > self.lsa:
            i1[i1 >= self.lea] = self.lsa
        i1 = np.minimum(i1, len(self.wave) - 1)
        fr = pos - np.floor(pos)
        s = self.wave[i0] * (1 - fr) + self.wave[i1] * fr
        ofs = env >> 2
        lut = np.array(TL_LUT, dtype=np.float64)
        outs = []
        for att in self.att:                       # left, right [Flycast ChannelEx::Step]
            idx = ofs + np.minimum(att, 255 - ofs)
            outs.append(s * lut[idx] / 2 ** 19)
        return outs


def layer_curve(b, idx):
    """Velocity curve idx of section 2 (index past the last: curve 0) [0x2FD0]."""
    s2 = b.so[2]
    if idx > u32(b.d, s2):
        idx = 0
    o = s2 + 4 + 128 * idx
    return b.d[o:o + 128]


def synth(banks, bank, key, loops=1, tick=TICK_SAMPLES, tail_s=8.0, start_lag=START_LAG):
    """Play one request through the bank's own waves: (left, right, stats)
    at 44,100 Hz, unscaled (one voice at full level peaks at 1/16 of 16-bit)."""
    stats = Counter()
    evs = render(banks, bank, key, loops, start_lag=start_lag)
    setups, levels, voices = {}, {}, []

    def alive(v, s):
        if v.stop is not None and v.stop <= s:
            return False
        if v.keyoff is None or v.keyoff > s:
            return True
        if v.end is None:                          # key-off is final: find where the release ends
            env, end = v.envelope(int(44100 * 120))
            v.end = v.start + (end if end is not None else len(env))
        return v.end > s

    def key_off(vs, s):
        for v in vs:
            if v.stop is None and (v.keyoff is None or v.keyoff > s):
                v.keyoff = s
                v.end = None

    last = 0
    for t, kind, p in evs:
        s = round(t * tick)
        last = max(last, s)
        if kind == "a0":
            d1, d2, b = p
            if d1 == 0x19:
                setups[b.name] = d2
                st = b.setups[d2] if d2 < len(b.setups) else None
                if st and st["level"] != 0xFF:
                    levels[b.name] = st["level"] & 0x7F
            elif d1 == 0x1C:
                levels[b.name] = d2
            elif d1 == 0 and d2 & 0x0F in (1, 2, 3):
                kind_off = d2 & 0x0F               # 1 all, 2 setup parts, 3 resident parts
                mine = [v for v in voices if d2 < 0x10 or v.bank == b.name]
                key_off([v for v in mine if kind_off == 1 or v.resident == (kind_off == 3)], s)
                if kind_off == 2 and d2 >= 0x10:
                    setups[b.name] = None          # parts reset [0x4774]
                stats[f"A0 00 {d2:02x}"] += 1
            continue
        if kind != "note":
            continue
        st, note, vel, ext, b, form7, e = p
        ch = st & 15
        if st & 0xF0 == 0x80 or not vel:
            key_off([v for v in voices if v.bank == b.name and v.ch == ch and v.note == note], s)
            continue
        res = resolve_note(b, active_parts(b, setups.get(b.name)), ch, note, vel, ext)
        if not res and setups.get(b.name) is None:
            for alt in bank_setups_selected(b):
                res = resolve_note(b, active_parts(b, alt), ch, note, vel, ext)
                if res:
                    stats["note-ons played with a setup a song selects"] += 1
                    break
        if not res:
            stats["note-ons with no part"] += 1
            continue
        for part, prog, tone, why in res:
            if tone is None:
                stats["note-ons with no tone"] += 1
                continue
            if tone["wave"] < 0x100:
                stats["tones on driver waves (skipped)"] += 1
                continue
            lay = next(x for x in prog["layers"] if any(y is tone for y in x["tones"]))
            v = Voice(b, part, prog, lay, tone, note, vel, s, levels.get(b.name, 0x7F), stats)
            # voice limits: 64 channels; resident parts at most header +0x1A [0x1248, 0x1D38]
            live = [x for x in voices if alive(x, s)]
            pool = [x for x in live if x.resident] if v.resident else live
            limit = b.d[0x1A] if v.resident else VOICES
            if len(pool) >= limit or len(live) >= VOICES:
                victim = min(pool if len(pool) >= limit else live, key=lambda x: x.start)
                victim.stop = s
                stats["voices stolen"] += 1
            if v.oneshot:                          # driver checks the play position each tick [0xA74]
                v.keyoff = math.ceil((s + v.hits_loop()) / tick) * tick
                v.keyoff = round(v.keyoff)
            voices.append(v)
            stats["voices"] += 1
    held = [v for v in voices if v.keyoff is None]
    stats["voices still held at the end (keyed off there)"] += len(held)
    for v in held:
        v.keyoff = max(last, v.start)
    total = last + int(44100 * tail_s)
    left, right = np.zeros(total), np.zeros(total)
    for v in voices:
        n = total - v.start
        if v.stop is not None:
            n = min(n, v.stop - v.start)
        if n <= 0:
            continue
        lo, ro = v.render(n)
        left[v.start:v.start + len(lo)] += lo
        right[v.start:v.start + len(ro)] += ro
    nz = np.nonzero(np.abs(left) + np.abs(right) > 0.5)[0]
    end = int(nz[-1]) + 4410 if len(nz) else last
    return left[:end], right[:end], stats


def write_wave(path, left, right, peak=0.891):
    """16-bit stereo WAV, scaled so the loudest sample is at `peak` (-1 dBFS)."""
    m = max(np.max(np.abs(left)), np.max(np.abs(right)), 1e-9)
    g = peak * 32767 / m
    frames = np.stack([left * g, right * g], axis=1)
    with wave.open(path, "wb") as w:
        w.setnchannels(2)
        w.setsampwidth(2)
        w.setframerate(44100)
        w.writeframes(np.round(frames).astype("<i2").tobytes())
    return 20 * math.log10(g)


# -- checks -----------------------------------------------------------------
def bank_paths(src):
    return sorted(glob.glob(os.path.join(src, "bg_*.bin")) + glob.glob(os.path.join(src, "se_*.bin"))
                  + glob.glob(os.path.join(src, "vo_*.bin")))


def active_parts(b, setup):
    """The parts a bank's channels have with `setup` selected (None: only the
    default setup). Selecting a setup frees every part without flag bit 0
    [0x1340 -> 0x19E0] and adds the setup's parts [0x1390]; the default
    setup's parts (flag 0x83) keep bit 0 and stay."""
    parts = list(b.default_setup["parts"]) if b.default_setup else []
    if setup is not None:
        parts = [p for p in parts if p["flags"] & 1]
        st = b.setups[setup] if setup < len(b.setups) else None
        parts += st["parts"] if st else []
    return parts


def resolve_note(b, parts, ch, note, vel, ext):
    """Tones a note-on plays with the given parts [0x3884-0x3A88]:
    [(part, program, tone or None, why)]."""
    out = []
    for part in parts:
        if part["channel"] != ch:
            continue
        if not (part["notes"][0] <= note <= part["notes"][1] and part["vels"][0] <= vel <= part["vels"][1]):
            continue
        prog = b.program_for(part)
        if prog is None:
            out.append((part, None, None, "program missing"))
            continue
        for lay in prog["layers"]:
            typ = prog["type"]
            tone = None
            if typ == 1:
                lo, hi = lay["notes"]
                if lo <= note <= hi:
                    tone = lay["map"][note - lo]
            elif typ == 2:
                ix = note | ((ext & 15) << 7)
                if ix < len(lay["tones"]):
                    tone = lay["tones"][ix]
            else:
                if not lay["vels"][0] <= vel <= lay["vels"][1]:
                    continue
                for t in lay["tones"]:
                    if note <= t["top"]:
                        tone = t
                        break
            if tone is not None and typ in (1, 2) and not (tone["vels"][0] <= vel <= tone["vels"][1]):
                out.append((part, prog, None, "velocity outside the tone's range"))
                continue
            out.append((part, prog, tone, "" if tone else f"no tone for note {note} ext {ext}"))
    return out


def script_messages(b, sc):
    """(tick, (status, d1, d2, ext)) of a script in order, the song list run
    once (each list word visited once)."""
    form7 = bool(sc["header"] & 0x80)
    t = 0
    for kind, arg in sc["items"]:
        if kind == "stream":
            for ev in b.streams[(arg, form7)][0]:
                yield t, ev.message
                t += ev.delta
        elif kind == "message":
            yield t, (arg[0], arg[1], arg[2], 0)


def bank_setups_selected(b):
    """Setups the bank's own scripts select (A0 19 n)."""
    out = set()
    for e in b.entries:
        for sc in e["scripts"]:
            if sc is not None:
                for t, (st, d1, d2, ext) in script_messages(b, sc):
                    if st == 0xA0 and d1 == 0x19:
                        out.add(d2)
    return sorted(out)


def script_notes(b, sc, fallback):
    """Note-ons of a script with the tones they reach: a song's A0 19 n
    changes the parts for the notes after it; other scripts play with the
    default setup, or else with a setup a song of the bank selects
    (`fallback`). Yields (tick, ch, note, vel, ext, setup label, results)."""
    setup = None
    for t, (st, d1, d2, ext) in script_messages(b, sc):
        if st == 0xA0 and d1 == 0x19:
            setup = d2
        elif st & 0xF0 == 0x90 and d2:
            ch = st & 15
            res = resolve_note(b, active_parts(b, setup), ch, d1, d2, ext)
            label = "default" if setup is None else f"setup {setup}"
            if not res and setup is None:
                for s in fallback:
                    res = resolve_note(b, active_parts(b, s), ch, d1, d2, ext)
                    if res:
                        label = f"setup {s} (selected by a song)"
                        break
            yield t, ch, d1, d2, ext, label, res


def check(src, verbose=False):
    banks = [Bank(p) for p in bank_paths(src)]
    print(f"{len(banks)} banks")
    ok = True

    # a. every script parses to its end
    nscripts = nstreams = nevents = 0
    status_use = Counter()
    stops = []
    for b in banks:
        if not b.so[3]:
            stops.append(f"{b.name}: no section 3 (offset 0; the driver would read the header as a table)")
        for e in b.entries:
            for sc in e["scripts"]:
                if sc is None:
                    continue
                nscripts += 1
                status_use["script header %02x" % sc["header"]] += 1
                for pr in sc["problems"]:
                    stops.append(f"{b.name} {e['class']:02x}:{e['id']:02x}: {pr}")
                for kind, arg in sc["items"]:
                    if kind in ("message", "jump", "loop"):
                        status_use[f"song list: {kind}"] += 1
                    elif kind == "end":
                        status_use[f"song list: end word {arg:#04x}"] += 1
        for (off, form7), (events, end, problem) in b.streams.items():
            nstreams += 1
            nevents += len(events)
            if problem:
                stops.append(f"{b.name}: {problem}")
            for ev in events:
                st = ev.status
                hi = st & 0xF0
                if hi == 0xD0 and form7:
                    status_use[f"event Dx n v e (note-on), e = {ev.data[2]}"] += 1
                elif st == 0xA0:
                    status_use[f"event A0 {ev.data[0]:02x}" + (f" {ev.data[1]:02x}" if ev.data[0] == 0 else "")] += 1
                elif st in REQUEST_CLASSES:
                    status_use[f"event {st:02x} (start a sequence)"] += 1
                elif hi == 0x90:
                    status_use["event 9x n v (note-on, v 0 note-off)"] += 1
                else:
                    status_use[f"event {st:02x}"] += 1
                if hi == 0xF0 or (hi == 0xD0 and not form7) or (0xA1 <= st <= 0xA7):
                    stops.append(f"{b.name} @{ev.off:#x}: status {st:02x} is not a sequence command")
                if ev.running:
                    status_use["(events in running status)"] += 1
                if ev.delta > 0x7F:
                    status_use["(two-byte deltas)"] += 1
    print(f"\na. {nscripts} scripts, {nstreams} streams, {nevents} events; all end in 0xFF or the song "
          "list's end / loop unless listed")
    for k, v in sorted(status_use.items()):
        print(f"   {k:42s} {v}")
    print(f"   stops / unknown bytes: {len(stops)}")
    for s in stops:
        print("   ", s)

    # b. byte coverage of sections 1-3 (and 0)
    print("\nb. byte coverage")
    total_gap, zero_gaps = Counter(), Counter()
    for b in banks:
        for s, own in sorted(b.owner.items()):
            run = None
            for i, o in enumerate(own + ["end"]):
                if o is None and run is None:
                    run = i
                elif o is not None and run is not None:
                    start = b.so[s] + run
                    data = b.d[start:b.so[s] + i]
                    total_gap[s] += len(data)
                    prev = own[run - 1] if run else "section start"
                    if any(data) or verbose:
                        print(f"   {b.name} section {s} gap @{start:#x} size {len(data)} after a {prev}: "
                              f"{data[:24].hex(' ')}{' ...' if len(data) > 24 else ''}")
                    else:
                        where = "at the section end" if o == "end" else f"after a {prev}"
                        zero_gaps[(s, len(data), where)] += 1
                    run = None
        for pr in b.problems:
            print(f"   {b.name}: {pr}")
            ok = False
    for (s, n, where), k in sorted(zero_gaps.items()):
        print(f"   section {s}: {k} zero run{'s' if k > 1 else ''} of {n} bytes {where}")
    sizes = Counter()
    for b in banks:
        for s in range(4):
            if b.so[s]:
                sizes[s] += b.end(s) - b.so[s]
    for s in range(4):
        print(f"   section {s}: {sizes[s]} bytes, {sizes[s] - total_gap[s]} parsed, {total_gap[s]} not")

    # c. every sequence, program, tone and wave a sequence names exists
    print("\nc. references")
    req_total = req_missing = 0
    notes = Counter()
    silent = Counter()
    for b in banks:
        fallback = bank_setups_selected(b)
        for e in b.entries:
            for k, sc in enumerate(e["scripts"]):
                if sc is None:
                    continue
                for t, (st, d1, d2, ext) in script_messages(b, sc):
                    if st in REQUEST_CLASSES:
                        req_total += 1
                        if b.find_script(st, d1, d2) is None:
                            where = [x.name for x in banks if x.find_script(st, d1, d2) is not None]
                            req_missing += 1
                            print(f"   {b.name}: start {st:02x}:{d1:02x}:{d2} not in the bank"
                                  f"{'; in ' + ', '.join(where) if where else '; in no bank'}")
                    elif st == 0xA0 and d1 == 0x19:
                        notes["setup selections (A0 19 n)"] += 1
                        if d2 >= len(b.setups) or b.setups[d2] is None:
                            print(f"   {b.name}: A0 19 {d2}: no such setup")
                            ok = False
                for t, ch, note, vel, ext, label, res in script_notes(b, sc, fallback):
                    notes["note-ons"] += 1
                    if not res:
                        silent[(b.name, f"{e['class']:02x}:{e['id']:02x}:{k}", ch, label)] += 1
                        continue
                    notes["note-ons resolved with the default setup" if label == "default"
                          else "note-ons resolved with a selected setup"] += 1
                    for part, prog, tone, why in res:
                        if tone is None:
                            notes["unresolved"] += 1
                            print(f"   {b.name} {e['class']:02x}:{e['id']:02x}:{k}: ch{ch} note {note} "
                                  f"ext {ext}: {why}")
                            continue
                        notes["tones reached"] += 1
                        wv = tone["wave"]
                        if wv < 0x100:
                            notes["  of them driver waves"] += 1
                        elif wv - 0x100 >= b.nwaves:
                            notes["  of them missing waves"] += 1
                            print(f"   {b.name}: tone @{tone['off']:#x} wave {wv - 0x100} past the {b.nwaves} waves")
    for (name, key, ch, label), n in sorted(silent.items()):
        print(f"   {name} {key}: {n} note-ons on ch{ch} have no part "
              f"(with {'the default setup' if label == 'default' else label})")
    print(f"   {req_total} sequence starts, {req_missing} not in their own bank")
    for k, v in sorted(notes.items()):
        print(f"   {k:44s} {v}")
    bad = 0
    for b in banks:
        for t in b.tones.values():
            if t["wave"] >= 0x100 and t["wave"] - 0x100 >= b.nwaves:
                bad += 1
                print(f"   {b.name}: tone @{t['off']:#x} names wave {t['wave'] - 0x100} of {b.nwaves}")
    print(f"   all {sum(len(b.tones) for b in banks)} tones of sections 1 and 4: {bad} name a missing wave; "
          f"wave link word +0x50 set in {sum(1 for b in banks if b.link)} banks")
    if req_missing or bad or notes["unresolved"] or notes["  of them missing waves"]:
        ok = False

    print("\nd. waves tools/dtpk.py found no program for")
    ambience_report(banks)
    parse_errors = [s for s in stops if "no section 3" not in s]
    print(f"\na. parse: {'pass' if not parse_errors else 'FAIL'} ({len(stops)} listed; a bank without "
          "section 3 is data, not a parse error)")
    print(f"b. coverage: {'pass' if not any(b.problems for b in banks) else 'FAIL'} (unparsed bytes listed)")
    print(f"c. references: {'pass' if ok else 'FAIL'} (note-ons with no part are listed as data)")
    return ok and not parse_errors


def wave_users(b):
    """wave -> [(label, program, tone)] for every tone of sections 1 and 4."""
    users = defaultdict(list)
    labels = {o: f"program {bk}:{p}" for (bk, p), o in b.prog_table.items()}
    if b.sec4:
        labels[b.sec4["off"]] = "section 4"
    for o, prog in b.programs.items():
        for lay in prog["layers"]:
            for t in lay["tones"]:
                if t["wave"] >= 0x100:
                    users[t["wave"] - 0x100].append((labels.get(o, f"program @{o:#x}"), prog, t))
    return users


def played_notes(b):
    """tone offset -> Counter of (note, part transpose, part detune, script) the bank's scripts play."""
    out = defaultdict(Counter)
    fallback = bank_setups_selected(b)
    for e in b.entries:
        for k, sc in enumerate(e["scripts"]):
            if sc is None:
                continue
            key = f"{e['class']:02x}:{e['id']:02x}:{k}"
            for t, ch, note, vel, ext, label, res in script_notes(b, sc, fallback):
                for part, prog, tone, why in res:
                    if tone is not None:
                        n = note if prog["type"] == 0 else 60      # types 1, 2 ignore the note [0x4320]
                        out[tone["off"]][(n, part["transpose"], part["detune"], key)] += 1
    return out


def ambience_rates(b):
    """wave -> (rate at note 60 of its section 1 tones, details) for the waves
    tools/dtpk.py finds no section 4 / 5 tone for."""
    old = {w for w, r, who in dtpk.tone_rates(b.d)}
    users = wave_users(b)
    played = played_notes(b)
    out = {}
    for w in range(b.nwaves):
        if w in old:
            continue
        rows = []
        for label, prog, t in users.get(w, []):
            notes = played.get(t["off"], Counter())
            rows.append({"label": label, "type": prog["type"], "tone": t, "base": rate_of(t), "played": notes})
        out[w] = rows
    return out


def ambience_report(banks):
    """For each wave tools/dtpk.py rates from section 1 only: the rate it
    writes (dtpk.section1_rates), the notes the bank's scripts play the
    tone at and the rates those give."""
    kinds = Counter()
    for b in banks:
        written = {}
        for w, r, src in dtpk.section1_rates(b.d):
            written.setdefault(w, (round(r), src))
        for w, rows in ambience_rates(b).items():
            if not rows:
                print(f"   {b.name} wave {w}: no tone in sections 1 or 4 either")
                kinds["no tone"] += 1
                continue
            rate, src = written.get(w, (None, "none"))
            played_rates = Counter()
            for r in rows:
                t = r["tone"]
                print(f"   {b.name} wave {w}: {r['label']} type {r['type']} tone @{t['off']:#x} top note {t['top']} "
                      f"pitch {t['semi']:+d}{t['fine']:+d}/64: {round(r['base'])} Hz at note 60")
                for (n, tr, dt, key), c in r["played"].items():
                    pr = round(rate_of(t, n + tr, part_fine=dt, keyboard=r["type"] == 0))
                    played_rates[pr] += c
                scripts = sorted({key for (n, tr, dt, key) in r["played"]})
                if r["played"]:
                    notes = Counter()
                    for (n, tr, dt, key), c in r["played"].items():
                        notes[n + tr] += c
                    print(f"      played by {', '.join(scripts)}: notes "
                          + " ".join(f"{n}x{c}" for n, c in sorted(notes.items())))
            if not played_rates:
                verdict = "ladder layout only, not played" if src == "ladder" else "not settled, not played"
            elif set(played_rates) == {rate}:
                verdict = "every play at this rate"
            elif played_rates.most_common(1)[0][0] == rate:
                verdict = "most plays at this rate"
            else:
                verdict = ("ladder layout; played transposed" if src == "ladder"
                           else "not settled; played at several rates")
            kinds[f"{src}: {verdict}"] += 1
            pr = ", ".join(f"{r} Hz x{c}" for r, c in sorted(played_rates.items()))
            print(f"      written at {rate} Hz ({src}); {verdict}{'; played at ' + pr if pr else ''}")
    print(f"   {sum(kinds.values())} waves:")
    for k, v in sorted(kinds.items()):
        print(f"     {v:3d} {k}")


def main():
    a = sys.argv[1:]
    if not a:
        print(__doc__)
        sys.exit(1)
    if a[0] == "check":
        sys.exit(0 if check(a[1], "-v" in a) else 1)
    if a[0] == "all":
        os.makedirs(a[2], exist_ok=True)
        n = 0
        for p in bank_paths(a[1]):
            b = Bank(p)
            with open(os.path.join(a[2], b.name + ".txt"), "w") as f:
                b.dump(f)
            n += 1
        print(f"{n} banks dumped to {a[2]}", file=sys.stderr)
        return
    b = Bank(a[0])
    if "--midi" in a or "--wav" in a:
        loops = int(a[a.index("--loops") + 1]) if "--loops" in a else 1
        if "--seq" in a:
            cls, sid, num = (int(x, 16) if i < 2 else int(x) for i, x in enumerate(a[a.index("--seq") + 1].split(":")))
        else:
            e = next(e for e in b.entries if any(e["scripts"]))
            num = next(i for i, s in enumerate(e["scripts"]) if s)
            cls, sid = e["class"], e["id"]
        others = [Bank(p) for p in bank_paths(os.path.dirname(os.path.abspath(a[0]))) if p != a[0]]
        lag = int(a[a.index("--start-lag") + 1]) if "--start-lag" in a else START_LAG
    if "--wav" in a:
        out = a[a.index("--wav") + 1]
        tick = float(a[a.index("--tick") + 1]) if "--tick" in a else TICK_SAMPLES
        left, right, stats = synth([b] + others, b, (cls, sid, num), loops, tick, start_lag=lag)
        if "--mono" in a:                          # the chip's mono bit: (L + R) >> 1 to both
            left = right = (left + right) / 2
        gain = write_wave(out, left, right)
        print(f"{out}: {cls:02x}:{sid:02x}:{num}, {len(left) / 44100:.1f} s, tick {tick:g} samples, "
              f"start lag {lag}{', mono' if '--mono' in a else ''}, scaled {gain:+.1f} dB to -1 dBFS",
              file=sys.stderr)
        for k, v in sorted(stats.items()):
            print(f"  {k}: {v}", file=sys.stderr)
        return
    if "--midi" in a:
        out = a[a.index("--midi") + 1]
        evs = render([b] + others, b, (cls, sid, num), loops, start_lag=lag)
        write_midi(out, evs, f"{b.name} {cls:02x}:{sid:02x}:{num}")
        notes = sum(1 for e in evs if e[1] == "note")
        print(f"{out}: {cls:02x}:{sid:02x}:{num}, {notes} note events, "
              f"{max((e[0] for e in evs), default=0)} ticks "
              f"({max((e[0] for e in evs), default=0) / TICK_HZ:.1f} s)", file=sys.stderr)
        return
    b.dump(sys.stdout)


if __name__ == "__main__":
    main()
