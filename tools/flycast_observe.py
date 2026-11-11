"""Watch the game's functions being called, through the GDB server of the
Flycast debug build (port 3263): who calls each one, in which game mode,
with which arguments, and what it returns. The runbook is
docs/live_observation.md; tools/observe_summary.py turns the logs into a
digest per function.

    gdb -q -batch -nx -x tools/flycast_observe.py                   observe
    VF4OBS_MODE=clear gdb -q -batch -nx -x tools/flycast_observe.py  recovery
    python3 tools/flycast_observe.py check        offline: what would be set

observe: a breakpoint on the entry of every function of
  ghidra/source_map.tsv (VF4OBS_FUNCS narrows it). Each hit is one JSON line
  in work/observe/<run>.jsonl: time, function, the return address in PR and
  the function and offset it lies in, r4-r7, fr4-fr7, r0-r3, r15, four stack
  words, the bytes the pointer arguments point at, main_mode_no /
  sub_mode_no, game_frame_count and vblank_count. For the first hits of a
  function the return is captured too: the entry breakpoint moves to the
  return address and comes back when control arrives there with r15 where
  it was at the entry (a "ret" line: r0, r1, fr0). A function is finished
  after VF4OBS_HITS hits in VF4OBS_VARIETY different contexts (call site x
  mode); one that has its hits in the current mode but not the variety is
  parked, and set again when the game enters a mode it has not been seen
  in. So the breakpoints thin out and the game speeds up as coverage grows.
  work/observe/state.json keeps the counts: a later session sets only what
  is still open.
clear: after a session that was killed. Removes every breakpoint the
  journals (work/observe/*.bps) and a scan of the code find, puts back from
  work/main.bin any trapa word Flycast no longer knows (a save state made
  with breakpoints in), and detaches, which restarts the emulator.
check: no connection; prints what the static checks decide.

What Flycast's server does, and what follows from it here (read from
core/debug/gdb_server.cpp and debug_agent.h of v2.7):
  - A breakpoint is the word 0xC308 (trapa #0x20) written over the
    instruction; the old word is kept in a table. Z0 on an address already
    in the table answers OK without writing, z0 writes the kept word back.
    So the script reads the word before every removal and never removes
    over code the game has changed, and checks after setting that the
    words really are there.
  - A trapa in a delay slot loses the branch (the stop backs the PC up one
    instruction). No breakpoint goes on an address that code reaches as a
    delay slot; a return address is used only when a call instruction sits
    two instructions before it.
  - Every halt and every resume restarts the emulator thread and its audio,
    tens of milliseconds each, and a single step is a resume and a halt.
    So a hit is not stepped over: at an entry the breakpoint is taken out
    and the game continues, and it goes back in at the return address stop
    or at the next stop of any kind. A call that happens in between is not
    seen, which is fine: the aim is variety, not volume. (One instruction
    is stepped only where a breakpoint has to stay under the PC: a return
    address a deeper call came back to, the guard below, a mode handler
    called outside its mode.)
  - The server neither removes breakpoints nor resumes when the client
    goes away. This script removes its breakpoints on a normal end, on
    SIGTERM / SIGHUP, on Ctrl-C, on work/observe/STOP and on an error; if
    its own connection is no longer usable it starts a second gdb in clear
    mode. After kill -9, run clear mode by hand.
  - The game itself rewrites one function entry at run time
    (code_jump_patch, for ta_input_finish). The entry of code_jump_patch
    stays set as a guard: when the game is about to patch an entry, the
    breakpoint there is taken out first and the function is left alone.
  - The Naomi test menu is another program (work/test.bin) that the BIOS
    loads over the game, at the same address, and leaving it loads the
    game again. Flycast's table knows nothing of that. When the script
    finds that the code in RAM is not the game's (it looks after
    VF4OBS_IDLE seconds without a stop) it touches nothing, waits until
    the image is back and the PC is inside it, and sets everything again.
    The entries of the game's mode handlers stay set while the game is in
    another mode: the call of one is how a mode change is noticed.
Only one GDB client at a time: do not run tools/flycast_live.py while this
is connected (the server sends its stops to the newest client).

Settings (environment):
  VF4OBS_MODE        observe (default), clear, check
  VF4OBS_RUN         run name, default obs-<date>-<time>; the log is
                     work/observe/<run>.jsonl (appended to if it exists)
  VF4OBS_DIR         log directory, default work/observe
  VF4OBS_FUNCS       file of functions to watch, one address or name a line
                     (# starts a comment); default: all of source_map.tsv
  VF4OBS_SKIP        file of functions to leave out, same format
  VF4OBS_HITS        hits a function needs (3)
  VF4OBS_VARIETY     different contexts (call site x mode) they must come
                     from (2)
  VF4OBS_MODE_QUOTA  hits taken in one mode before the function is parked
                     until the mode changes (default: VF4OBS_HITS)
  VF4OBS_MAX_HITS    hits after which a function is finished whatever the
                     variety (8)
  VF4OBS_RET         how many hits of a function also capture the return
                     (2); "all" for every hit, 0 for none
  VF4OBS_RET_FRAMES  game frames to wait for a return before giving up (4)
  VF4OBS_PEEK        bytes read at each pointer argument and pointer
                     result (16; 0: none)
  VF4OBS_IDLE        seconds without a stop before the script halts the
                     game for a moment to check its breakpoints (15)
  VF4OBS_TIME        stop after this many seconds (0: run until stopped)
  VF4OBS_CPU         write this to cpu_on_off when attaching (4: the AI
                     plays both sides; see tools/flycast_live.py cpu mode)
  VF4OBS_BOOT        1: attach while the machine is still loading the game
                     and set the breakpoints once the image is in RAM and
                     the PC is inside it (to see the boot code)
  VF4OBS_MAX_STOPS   stop after this many stops (0: no limit)
  VF4OBS_STATE       state file, default <dir>/state.json
  VF4OBS_FRESH       1: ignore the state file's counts
  VF4OBS_HOST, VF4OBS_PORT   default 127.0.0.1, 3263
  VF4OBS_IMAGE       default work/main.bin
While it runs, from another terminal:
  touch work/observe/STOP        ends the session cleanly
  echo 4 > work/observe/CPU      writes cpu_on_off (0 puts it back)
"""
import bisect
import collections
import csv
import glob
import json
import math
import os
import random
import select
import signal
import struct
import subprocess
import sys
import threading
import time
import traceback

try:
    import gdb
except ImportError:  # check mode and the tests run without gdb
    gdb = None

ENV = os.environ.get
HERE = os.path.dirname(os.path.abspath(globals().get("__file__", "tools/flycast_observe.py")))
ROOT = os.path.dirname(HERE)
MODE = ENV("VF4OBS_MODE", "observe")
OUT = ENV("VF4OBS_DIR", os.path.join(ROOT, "work", "observe"))
RUN = ENV("VF4OBS_RUN", time.strftime("obs-%Y%m%d-%H%M%S"))
HOST, PORT = ENV("VF4OBS_HOST", "127.0.0.1"), int(ENV("VF4OBS_PORT", "3263"))
IMAGE = ENV("VF4OBS_IMAGE", os.path.join(ROOT, "work", "main.bin"))
HITS = int(ENV("VF4OBS_HITS", "3"))
VARIETY = int(ENV("VF4OBS_VARIETY", "2"))
QUOTA = int(ENV("VF4OBS_MODE_QUOTA", str(HITS)))
MAX_HITS = int(ENV("VF4OBS_MAX_HITS", "8"))
RET = ENV("VF4OBS_RET", "2")
RET_N = 1 << 30 if RET == "all" else int(RET)
RET_FRAMES = int(ENV("VF4OBS_RET_FRAMES", "4"))
RET_SECS = 900.0
PEEK = int(ENV("VF4OBS_PEEK", "16"))
IDLE = float(ENV("VF4OBS_IDLE", "15"))
TIME_LIMIT = float(ENV("VF4OBS_TIME", "0"))
MAX_STOPS = int(ENV("VF4OBS_MAX_STOPS", "0"))
STATE = ENV("VF4OBS_STATE", os.path.join(OUT, "state.json"))

LOAD = 0x0C020000            # work/main.bin loads here
RAM_LO, RAM_HI = 0x0C000000, 0x0E000000
TRAP = 0xC308                # trapa #0x20, Flycast's breakpoint word
MODE_WORDS = 0x0C24313C      # main_mode_no; sub_mode_no is 12 bytes on (0x0C243148)
FRAME_WORDS = 0x0C292540     # game_frame_count, vblank_count (+4)
MODE_DATA, MODE_STRIDE = 0x0C162B00, 0xCC   # as tools/mode_table.py reads it
CPU_ON_OFF = 0x0C2926F0      # the debug menu's CPU switch (tools/flycast_live.py)
GUARDS = ("code_jump_patch",)  # routines that rewrite a function entry: (routine, target)
PATCH_LEN = 12               # bytes code_jump_patch copies over the entry
R_PC, R_PR, R_FR0 = 16, 17, 25   # places in Flycast's 'g' reply: r0-r15, pc, pr, ..., fr0 at 25


if gdb is not None:
    _TTY = os.fdopen(os.dup(1), "w")

    def print(*a, **kw):  # noqa: A001  gdb's own output is switched off (see connect); ours goes to the terminal
        _TTY.write(" ".join(str(x) for x in a) + "\n")
        _TTY.flush()


def norm(a):
    """the address in the 0x0C region for a P1 / P2 mirror address"""
    return a & 0x1FFFFFFF if (a >> 29) in (4, 5) else a


def in_ram(a):
    return (a >> 29) in (0, 4, 5) and RAM_LO <= (a & 0x1FFFFFFF) < RAM_HI


# ---------------------------------------------------------------- the image

class Image:
    """work/main.bin, the function map and what can be decided from them
    without the emulator"""

    def __init__(self, path=IMAGE, map_path=None):
        self.data = open(path, "rb").read()
        rows = list(csv.DictReader(open(map_path or os.path.join(ROOT, "ghidra", "source_map.tsv")),
                                   delimiter="\t"))
        self.rows = sorted((int(r["address"], 16), r["name"], int(r["size"]), r["file"]) for r in rows)
        self.starts = [r[0] for r in self.rows]
        self.name = {r[0]: r[1] for r in self.rows}
        self.size = {r[0]: r[2] for r in self.rows}
        self.by_name = {r[1]: r[0] for r in self.rows}
        self.code_lo = self.starts[0]
        self.code_hi = (self.rows[-1][0] + self.rows[-1][2] + 0x1F) & ~0x1F
        self.slots = set()
        self.exc = set()
        self.analyze()

    def word(self, a):
        o = a - LOAD
        if o < 0 or o + 2 > len(self.data):
            return None
        return self.data[o] | self.data[o + 1] << 8

    def u32(self, a):
        return struct.unpack_from("<I", self.data, a - LOAD)[0]

    def cstr(self, a):
        o = a - LOAD
        e = self.data.find(b"\0", o, o + 64)
        return self.data[o:e].decode("ascii", "replace")

    @staticmethod
    def branch(op, a):
        """(kind, target) of a branch instruction: kind 'j' ends the flow,
        'c' is a call, 'b' a conditional branch; delayed says a slot follows"""
        if op in (0x000B, 0x002B):
            return "j", None, True                       # rts, rte
        m = op & 0xF0FF
        if m in (0x402B, 0x0023):
            return "j", None, True                       # jmp @Rn, braf Rn
        if m in (0x400B, 0x0003):
            return "c", None, True                       # jsr @Rn, bsrf Rn
        top = op & 0xF000
        if top in (0xA000, 0xB000):
            d = op & 0xFFF
            d -= 0x1000 if d & 0x800 else 0
            return ("j" if top == 0xA000 else "c"), a + 4 + 2 * d, True   # bra, bsr
        hi = op & 0xFF00
        if hi in (0x8900, 0x8B00, 0x8D00, 0x8F00):
            d = op & 0xFF
            d -= 0x100 if d & 0x80 else 0
            return "b", a + 4 + 2 * d, hi in (0x8D00, 0x8F00)             # bt, bf, bt/s, bf/s
        return None

    @staticmethod
    def is_call(op):
        return op is not None and (op & 0xF0FF in (0x400B, 0x0003) or op & 0xF000 == 0xB000)

    def analyze(self):
        """slots: every address the code reaches as the delay slot of a
        branch, following the flow from every function entry (literal pools
        are never walked: they lie behind unconditional branches).
        exc: functions whose own flow saves SSR / SPC or ends in rte:
        entered by an exception, so PR is not their return address."""
        seen = set()
        work = list(self.starts)
        while work:
            a = work.pop()
            while self.code_lo <= a < self.code_hi and a not in seen:
                seen.add(a)
                br = self.branch(self.word(a), a)
                if br is None:
                    a += 2
                    continue
                kind, target, delayed = br
                if delayed:
                    self.slots.add(a + 2)
                    seen.add(a + 2)
                if target is not None:
                    work.append(target)
                if kind == "j":
                    break
                a += 4 if delayed else 2
        tails = collections.defaultdict(set)   # entry -> addresses it jumps to through a pool word
        for start in self.starts:
            seen2, work2, steps = set(), [start], 0
            while work2 and steps < 600 and start not in self.exc:
                a = work2.pop()
                while self.code_lo <= a < self.code_hi and a not in seen2 and steps < 600:
                    seen2.add(a)
                    steps += 1
                    op = self.word(a)
                    if op == 0x002B or op & 0xF0FF in (0x4033, 0x4043, 0x0032, 0x0042):
                        self.exc.add(start)   # rte; stc(.l) ssr / spc
                        break
                    br = self.branch(op, a)
                    if br is None:
                        a += 2
                        continue
                    kind, target, delayed = br
                    if delayed:
                        seen2.add(a + 2)
                        slot = self.word(a + 2)
                        if slot is not None and slot & 0xF0FF in (0x4033, 0x4043, 0x0032, 0x0042):
                            self.exc.add(start)
                    if op & 0xF0FF == 0x402B:             # jmp @Rn: the mov.l pool,Rn before it
                        for x in range(a - 2, max(a - 18, self.code_lo), -2):
                            lo = self.word(x)
                            if lo & 0xFF00 == 0xD000 | (op & 0x0F00):
                                lit = (x & ~3) + 4 + 4 * (lo & 0xFF)
                                if self.code_lo <= lit < self.code_hi - 4:
                                    tails[start].add(norm(self.u32(lit)))
                                break
                    if kind in ("b", "j") and target is not None:
                        work2.append(target)
                    if kind == "j":
                        break
                    a += 4 if delayed else 2

        grew = True
        while grew:                                   # a jump into an exception routine is one
            grew = False
            for start, to in tails.items():
                if start not in self.exc and to & self.exc:
                    self.exc.add(start)
                    grew = True

    def containing(self, a):
        """(entry, name, offset) of the function an address lies in, or
        None outside the code"""
        a = norm(a)
        if not self.code_lo <= a < self.code_hi:
            return None
        k = bisect.bisect_right(self.starts, a) - 1
        if k < 0:
            return None
        s = self.starts[k]
        return s, self.name[s], a - s

    def entry_reason(self, a):
        """why an entry cannot take a breakpoint, or None"""
        w = self.word(a)
        if w is None or not self.code_lo <= a < self.code_hi or a & 1:
            return "not an address in the image's code"
        if a in self.slots:
            return "the entry is the delay slot of the branch at %08x" % (a - 2)
        if w == TRAP:
            return "the first instruction is a trapa #0x20 itself"
        return None

    def site_reason(self, pr):
        """why a return address cannot take a breakpoint, or None"""
        r = norm(pr)
        if r & 1 or not self.code_lo <= r - 4 or not r < self.code_hi:
            return "PR is outside the image's code"
        if r in self.slots:
            return "PR is a delay slot"
        if not self.is_call(self.word(r - 4)):
            return "no call instruction before PR"
        return None

    def modes(self):
        """the game's mode table: names and handler entries"""
        names, sub, main = {}, collections.defaultdict(set), collections.defaultdict(set)
        try:
            for m in range(8):
                base = MODE_DATA + m * MODE_STRIDE
                mname = self.cstr(self.u32(base + 4))
                main[self.u32(base + 8)].add(m)
                names[str(m)] = mname
                for k in range(16):
                    e = base + 12 + k * 12
                    if not self.u32(e + 4):
                        break
                    names["%d.%d" % (m, k)] = "%s.%s" % (mname, self.cstr(self.u32(e + 4)))
                    sub[self.u32(e + 8)].add("%d.%d" % (m, k))
        except (struct.error, IndexError):
            return {}, {}, {}
        return names, dict(sub), dict(main)

    def pick(self, path):
        """the entries a file of addresses or names lists"""
        out = []
        for line in open(path):
            t = line.split("#")[0].split()
            if not t:
                continue
            t = t[0]
            if t in self.by_name:
                out.append(self.by_name[t])
                continue
            try:
                a = int(t, 16)
            except ValueError:
                print("observe: %s: no function named %s" % (path, t))
                continue
            if a in self.name:
                out.append(a)
            else:
                print("observe: %s: %08x is not a function entry of source_map.tsv" % (path, a))
        return out


class Fn:
    __slots__ = ("addr", "name", "hits", "ctx", "modes", "done", "skip", "noret", "exc", "rets")

    def __init__(self, addr, name):
        self.addr, self.name = addr, name
        self.hits, self.ctx, self.modes = 0, set(), {}
        self.done, self.skip, self.noret, self.exc, self.rets = False, None, False, False, 0

    def wants(self, mode):
        return not self.skip and not self.done and self.modes.get(mode, 0) < QUOTA

    def count(self, pr, mode):
        self.hits += 1
        self.modes[mode] = self.modes.get(mode, 0) + 1
        self.ctx.add("%08x:%s" % (norm(pr), mode))
        self.done = (self.hits >= HITS and len(self.ctx) >= VARIETY) or self.hits >= MAX_HITS


def targets(img):
    """the functions to watch, with the ones that cannot be"""
    want = img.pick(ENV("VF4OBS_FUNCS")) if ENV("VF4OBS_FUNCS") else list(img.starts)
    drop = set(img.pick(ENV("VF4OBS_SKIP"))) if ENV("VF4OBS_SKIP") else set()
    funcs = {}
    for a in want:
        if a in funcs:
            continue
        f = funcs[a] = Fn(a, img.name[a])
        f.exc = a in img.exc
        f.skip = "left out by VF4OBS_SKIP" if a in drop else img.entry_reason(a)
    return funcs


# ---------------------------------------------------------------- the link

class Link:
    """raw packets through gdb's connection to Flycast's server"""

    def __init__(self):
        self.conn = gdb.selected_inferior().connection
        self.packets = 0

    def send(self, p):
        self.packets += 1
        r = self.conn.send_packet(p)
        return r.decode("latin1") if r else ""

    def regs(self):
        r = self.send("g")
        if len(r) < 8 * (R_FR0 + 16):
            raise RuntimeError("register reply %r" % r[:40])
        return [int.from_bytes(bytes.fromhex(r[8 * i:8 * i + 8]), "little") for i in range(len(r) // 8)]

    def mem(self, a, n):
        out = b""
        while n:
            k = min(n, 0x700)
            r = self.send("m%x,%x" % (a, k))
            if len(r) != 2 * k:
                raise RuntimeError("memory reply at %08x: %r" % (a, r[:40]))
            out += bytes.fromhex(r)
            a, n = a + k, n - k
        return out

    def word(self, a):
        return struct.unpack("<H", self.mem(a, 2))[0]

    def write(self, a, data):
        return self.send("M%x,%x:%s" % (a, len(data), data.hex()))

    def z(self, a):
        return self.send("z0,%x,2" % a)

    def Z(self, a):
        return self.send("Z0,%x,2" % a)


def connect():
    for cmd in ("set pagination off", "set confirm off", "set architecture sh4", "set endian little"):
        gdb.execute(cmd, to_string=True)
    try:
        # gdb 17 and later ask qExecAndArgs when connecting; Flycast acknowledges a query it
        # does not know and never answers it, so gdb would sit out three timeouts first
        gdb.execute("set remote fetch-exec-and-args-packet off", to_string=True)
    except gdb.error:
        pass
    # gdb prints two lines for every stop (it does not know our breakpoints, so each is
    # "Program received signal SIGTRAP"): send its output away, ours goes to the terminal
    for cmd in ("set logging file /dev/null", "set logging redirect on", "set logging enabled on",
                "set tcp auto-retry off"):      # no server: say so at once
        gdb.execute(cmd, to_string=True)
    try:
        gdb.execute("target remote %s:%d" % (HOST, PORT), to_string=True)
    except gdb.error as e:
        print("observe: no GDB server at %s:%d (%s)." % (HOST, PORT, e))
        print("observe: is the debug Flycast running with the game started, and Debug.GDBEnabled = yes?")
        return None
    gdb.execute("set remotetimeout 20", to_string=True)   # a halt or a step restarts the emulator thread
    return Link()


def journal_open(directory):
    """addresses the journals say were set and not taken out"""
    live = set()
    for path in glob.glob(os.path.join(directory, "*.bps")):
        mine = set()
        try:
            for line in open(path):
                line = line.strip()
                if len(line) == 9 and line[0] in "+-":
                    (mine.add if line[0] == "+" else mine.discard)(int(line[1:], 16))
        except (OSError, ValueError):
            pass
        live |= mine
    return live


def sweep(link, img, extra, say=print):
    """Take out what is left in the code: z0 for the given addresses and for
    every trapa word the image does not have there, then put the image's
    word back where the trapa is still there (Flycast no longer knows it).
    Returns the live code and the counts."""
    span = img.code_hi - img.code_lo

    def scan():
        live = link.mem(img.code_lo, span)
        stray = [img.code_lo + i for i in range(0, span, 2)
                 if live[i] == 0x08 and live[i + 1] == 0xC3 and img.word(img.code_lo + i) != TRAP]
        return live, stray

    live, stray = scan()

    def lw(a):
        o = a - img.code_lo
        return live[o] | live[o + 1] << 8

    if sum(1 for a in img.starts if lw(a) in (img.word(a), TRAP)) < 0.9 * len(img.starts):
        # another program is in RAM (the test menu loads over the game) or the machine is
        # still loading: a 0xC308 there is not ours, and z0 would write the game's word into it
        say("observe: the game's code is not in RAM: nothing removed")
        return live, {"stray": 0, "removed": 0, "restored": 0, "absent": True}
    todo = set(stray)
    for a in extra:
        if img.code_lo <= a < img.code_hi and lw(a) in (TRAP, img.word(a)):
            todo.add(a)
    removed = sum(1 for a in sorted(todo) if link.z(a) == "OK")
    restored = 0
    if stray:
        live, left = scan()
        for a in left:
            link.write(a, struct.pack("<H", img.word(a)))
            restored += 1
            say("observe: %08x (%s) held a trapa Flycast did not know; the image's word %04x is back"
                % (a, "%s+%#x" % img.containing(a)[1:], img.word(a)))
        if left:
            live, left = scan()
            if left:
                say("observe: %d trapa words could not be removed: %s" % (
                    len(left), " ".join("%08x" % a for a in left[:8])))
    return live, {"stray": len(stray), "removed": removed, "restored": restored}


def clear_mode(img):
    link = connect()
    if link is None:
        return 1
    extra = journal_open(OUT) | set(img.starts)
    _, n = sweep(link, img, extra)
    if n.get("absent"):
        print("clear: the game's code is not in RAM (test menu, or the machine is loading): nothing was "
              "touched. Run clear mode again once the game itself runs.")
    else:
        print("clear: %d trapa words found in the code, %d breakpoints removed from Flycast's table, "
              "%d words put back from the image" % (n["stray"], n["removed"], n["restored"]))
        for path in glob.glob(os.path.join(OUT, "*.bps")):
            os.remove(path)
    gdb.execute("detach")
    print("clear: detached, the emulator runs")
    return 0


# ---------------------------------------------------------------- a session

class Pending:
    __slots__ = ("fn", "sp", "id", "t", "fc", "site")

    def __init__(self, fn, sp, hid, fc, site):
        self.fn, self.sp, self.id, self.t, self.fc, self.site = fn, sp, hid, time.time(), fc, site


class Session:
    def __init__(self, img, link, funcs):
        self.img, self.link, self.funcs = img, link, funcs
        self.inserted = set()      # addresses holding our trapa
        self.armed = set()         # entries that should hold one
        self.pend = {}             # return address -> [Pending]
        self.busy = set()          # entries waiting for their return
        self.defer = set()         # entries to set again at the next stop
        self.parked = set()        # entries with their quota in this mode
        self.nopatch = set()       # addresses the game rewrites
        self.mode = None
        self.mode_names, self.sub_handlers, self.main_handlers = img.modes()
        self.guards = {img.by_name[n] for n in GUARDS if n in img.by_name}
        self.false_sentinel = collections.Counter()
        self.safe_cache = {}
        self.bulk = False          # setting many at once: checked together afterwards
        self.live0 = b""
        self.stops = self.hits = self.rets = self.lost = self.seq = 0
        self.t0 = self.last_stop = self.last_status = self.last_save = time.time()
        self.last_sig = None
        self.in_continue = False
        self.gen = 0               # counts the resumes; an interrupt belongs to one
        self.int_posted = False    # the watcher asked for a halt of this resume
        self.int_sent = False      # and it was sent
        self.stop_req = None
        self.cmd_cpu = None
        self.dead = False
        self.unexpected = collections.Counter()
        self.log = None
        self.journal = None
        self.rng = random.Random(1)

    # ---- output ----
    def emit(self, k, **kw):
        kw["k"] = k
        kw["t"] = round(time.time(), 3)
        self.log.write(json.dumps(kw, separators=(",", ":")) + "\n")
        self.log.flush()

    def say(self, *a):
        print(time.strftime("%H:%M:%S"), *a)

    def mode_name(self, mode):
        return self.mode_names.get(mode, mode)

    def status(self):
        seen = sum(1 for f in self.funcs.values() if f.hits)
        done = sum(1 for f in self.funcs.values() if f.done)
        el = time.time() - self.t0
        self.say("stops %d (%.0f/s) hits %d returns %d | functions seen %d of %d, finished %d, parked %d, "
                 "set %d, waiting for a return %d | %s" % (
                     self.stops, self.stops / max(el, 1e-3), self.hits, self.rets, seen, len(self.funcs), done,
                     len(self.parked), len(self.inserted), sum(len(v) for v in self.pend.values()),
                     self.mode_name(self.mode)))
        self.last_status = time.time()

    # ---- state file ----
    def load_state(self):
        if ENV("VF4OBS_FRESH") == "1" or not os.path.exists(STATE):
            return 0
        try:
            st = json.load(open(STATE))
        except (OSError, ValueError) as e:
            self.say("state file %s unreadable (%s): starting from nothing" % (STATE, e))
            return 0
        n = 0
        for key, v in st.get("funcs", {}).items():
            f = self.funcs.get(int(key, 16))
            if not f:
                continue
            f.hits, f.ctx, f.modes = v.get("h", 0), set(v.get("c", [])), dict(v.get("m", {}))
            f.noret, f.rets = bool(v.get("n")), v.get("r", 0)
            f.done = (f.hits >= HITS and len(f.ctx) >= VARIETY) or f.hits >= MAX_HITS
            n += 1
        return n

    def save_state(self):
        try:
            old = json.load(open(STATE)).get("funcs", {}) if os.path.exists(STATE) else {}
        except (OSError, ValueError):
            old = {}
        if ENV("VF4OBS_FRESH") == "1":
            old = {k: v for k, v in old.items() if int(k, 16) not in self.funcs}
        for f in self.funcs.values():
            if f.hits:
                old["%08x" % f.addr] = {"name": f.name, "h": f.hits, "c": sorted(f.ctx), "m": f.modes,
                                        "d": int(f.done), "n": int(f.noret), "r": f.rets}
        tmp = STATE + ".tmp"
        with open(tmp, "w") as fh:
            json.dump({"version": 1, "updated": time.strftime("%Y-%m-%d %H:%M:%S"),
                       "rule": {"hits": HITS, "variety": VARIETY, "max_hits": MAX_HITS, "mode_quota": QUOTA},
                       "funcs": old}, fh, indent=0, sort_keys=True)
        os.replace(tmp, STATE)
        self.last_save = time.time()

    # ---- breakpoints ----
    def live_word(self, a):
        o = a - self.img.code_lo
        return self.live0[o] | self.live0[o + 1] << 8

    def can_insert(self, a):
        if a in self.nopatch:
            return "the game rewrites this code"
        if not self.img.code_lo <= a < self.img.code_hi or a in self.img.slots:
            return "not a safe address"
        if self.live_word(a) != self.img.word(a):
            return "the code in RAM differs from the image here"
        return None

    def insert(self, a):
        if a in self.inserted:
            return True
        if self.can_insert(a):
            return False
        self.journal.write("+%08x\n" % a)
        self.journal.flush()
        if self.link.Z(a) != "OK":
            return False
        if not self.bulk and self.link.word(a) != TRAP:
            # Flycast's table listed the address already and wrote nothing: unlist, set again
            self.link.z(a)
            if self.link.Z(a) != "OK" or self.link.word(a) != TRAP:
                self.link.z(a)
                return False
        self.inserted.add(a)
        return True

    def remove(self, a, note=True):
        """take our trapa out; never write over code the game has changed"""
        self.inserted.discard(a)
        w = self.link.word(a)
        if w != TRAP and w != self.img.word(a):
            self.lost += 1
            self.nopatch.add(a)
            self.emit("note", what="code changed under a breakpoint, left alone", addr="%08x" % a, word=w)
        else:
            r = self.link.z(a)
            if w == TRAP and r != "OK":
                self.link.write(a, struct.pack("<H", self.img.word(a)))
                self.emit("note", what="trapa Flycast did not know, image word written back", addr="%08x" % a)
        if note:
            self.journal.write("-%08x\n" % a)
            self.journal.flush()

    def refresh(self, a):
        want = a in self.armed or bool(self.pend.get(a))
        if want and a not in self.inserted:
            if not self.insert(a):
                self.armed.discard(a)
        elif not want and a in self.inserted:
            self.remove(a)

    def sentinel(self, a):
        """a mode handler of a mode the game is not in: its call is how a
        mode change is noticed"""
        s, m = self.sub_handlers.get(a), self.main_handlers.get(a)
        if s is None and m is None or self.mode is None:
            return False
        if s and self.mode in s:
            return False
        if m and int(self.mode.split(".")[0]) in m:
            return False
        return True

    def safe(self, a):
        """an entry that may take a breakpoint: passes the static checks and
        holds the image's word"""
        v = self.safe_cache.get(a)
        if v is None:
            f = self.funcs.get(a)
            v = self.safe_cache[a] = not (f.skip if f else self.img.entry_reason(a)) and self.can_insert(a) is None
        return v and a not in self.nopatch

    def entry_desired(self, a):
        if not self.safe(a):
            return False
        if a in self.guards or self.sentinel(a):
            return True
        f = self.funcs.get(a)
        return bool(f and a not in self.busy and a not in self.defer and f.wants(self.mode))

    def update_entry(self, a):
        if self.entry_desired(a):
            self.armed.add(a)
            self.parked.discard(a)
        else:
            self.armed.discard(a)
            f = self.funcs.get(a)
            if f and not f.skip and not f.done and a not in self.busy and a not in self.defer:
                self.parked.add(a)
        self.refresh(a)

    def step_over(self, a):
        """run the one instruction under a breakpoint that stays"""
        if self.link.word(a) != TRAP:
            return
        self.link.z(a)
        r = self.link.send("s")
        self.link.Z(a)
        if not r.startswith(("S", "T")):
            raise RuntimeError("step reply %r" % r[:20])

    # ---- start ----
    def read_mode(self):
        m = struct.unpack("<4i", self.link.mem(MODE_WORDS, 16))
        f = struct.unpack("<2i", self.link.mem(FRAME_WORDS, 8))
        return "%d.%d" % (m[0], m[3]), f[0], f[1]

    def loaded(self):
        """share of the watched entries whose word in RAM is the image's"""
        ok = sum(1 for a in self.img.starts if self.live_word(a) == self.img.word(a))
        return ok / len(self.img.starts)

    def prepare(self):
        """leftovers out, the code checked against the image, every wanted
        entry set and checked"""
        img = self.img
        os.makedirs(OUT, exist_ok=True)
        self.live0, n = sweep(self.link, img, journal_open(OUT), self.say)
        if n["stray"] or n["removed"]:
            self.say("leftover breakpoints of an earlier session: %d trapa words in the code, %d removed, "
                     "%d put back from the image" % (n["stray"], n["removed"], n["restored"]))
        if not n.get("absent"):
            for path in glob.glob(os.path.join(OUT, "*.bps")):   # dealt with, or of a Flycast that is gone
                os.remove(path)
        share = self.loaded()
        if share < 0.98:
            return "only %.1f%% of the function entries in RAM match %s: the game is not loaded yet" % (
                100 * share, os.path.relpath(IMAGE, ROOT))
        self.journal = open(os.path.join(OUT, RUN + ".bps"), "a")
        self.log = open(os.path.join(OUT, RUN + ".jsonl"), "a")
        for f in self.funcs.values():
            if not f.skip and self.live_word(f.addr) != img.word(f.addr):
                f.skip = "the code in RAM differs from the image (patched at run time)"
        resumed = self.load_state()
        self.mode, fc, vb = self.read_mode()
        skipped = [f for f in self.funcs.values() if f.skip]
        self.emit("start", run=RUN, functions=len(self.funcs), skipped=len(skipped), resumed=resumed,
                  rule={"hits": HITS, "variety": VARIETY, "max_hits": MAX_HITS, "mode_quota": QUOTA,
                        "ret": RET, "ret_frames": RET_FRAMES, "peek": PEEK},
                  mode=self.mode, fc=fc, vb=vb, mode_names=self.mode_names,
                  image=os.path.relpath(IMAGE, ROOT), image_bytes=len(img.data))
        for f in skipped:
            self.emit("skip", fn="%08x" % f.addr, name=f.name, why=f.skip)
        if ENV("VF4OBS_CPU") is not None:
            self.write_cpu(int(ENV("VF4OBS_CPU"), 0))
        self.arm_all()
        return None

    def arm_all(self):
        """set every entry that is wanted in the current mode, and check"""
        self.mode, fc, vb = self.read_mode()
        skipped = sum(1 for f in self.funcs.values() if f.skip)
        want = [a for a in sorted(set(self.funcs) | set(self.sub_handlers) | set(self.main_handlers) | self.guards)
                if self.entry_desired(a)]
        self.say("%d functions: %d cannot take a breakpoint, %d already finished, setting %d breakpoints"
                 % (len(self.funcs), skipped, sum(1 for f in self.funcs.values() if f.done), len(want)))
        chunk = int(ENV("VF4OBS_CHUNK", "1000"))
        t = time.time()
        self.bulk = True
        for i, a in enumerate(want):
            self.armed.add(a)
            self.refresh(a)
            if (i + 1) % chunk == 0:
                self.say("  set %d of %d (%.1f s)" % (i + 1, len(want), time.time() - t))
        self.bulk = False
        for f in self.funcs.values():
            if not f.skip and not f.done and f.addr not in self.armed:
                self.parked.add(f.addr)
        bad = self.verify_all()
        self.say("set %d breakpoints in %.1f s%s; mode %s, frame %d" % (
            len(self.inserted), time.time() - t, " (%d needed a second try)" % bad if bad else "",
            self.mode_name(self.mode), fc))

    def program_share(self):
        """share of a spread of function entries that hold the image's word
        or our trapa: low when another program is in RAM (the test menu is
        one, loaded over the game) or the machine is loading"""
        probe = self.img.starts[::61]
        ok = sum(1 for a in probe if self.link.word(a) in (self.img.word(a), TRAP))
        return ok / len(probe)

    def away(self):
        """The game's code is not in RAM any more. Touch nothing until it is
        back (Flycast's table still lists our addresses, and removing one
        now would write the game's word into the other program), then set
        everything again."""
        self.say("the game's code is no longer in RAM (test menu or reset): waiting for the game to come back")
        self.emit("note", what="game code replaced in RAM, waiting")
        for lst in self.pend.values():
            for p in lst:
                self.emit("noret", id=p.id, fn="%08x" % p.fn.addr, name=p.fn.name, why="the game was reloaded")
        self.pend.clear()
        self.busy.clear()
        self.defer.clear()
        if not self.wait_for_image():
            self.inserted.clear()        # nothing of ours is in RAM; the final scan confirms it
            return
        # z0 writes the word Flycast kept (the image's, which is what RAM holds again) and
        # takes the address off its table, so that the Z0 that follows really writes
        for a in sorted(self.inserted):
            self.link.z(a)
            self.journal.write("-%08x\n" % a)
        self.inserted.clear()
        self.armed.clear()
        self.parked.clear()
        self.say("the game is back")
        self.emit("note", what="game code back in RAM, breakpoints set again")
        self.arm_all()

    def verify_all(self):
        """every address we think is set must hold the trapa: Z0 does not
        write when Flycast's table already lists the address (after a reset
        reloaded the code, or after an earlier client)"""
        img = self.img
        live = self.link.mem(img.code_lo, img.code_hi - img.code_lo)
        bad = 0
        for a in sorted(self.inserted):
            o = a - img.code_lo
            w = live[o] | live[o + 1] << 8
            if w == TRAP:
                continue
            bad += 1
            if w != img.word(a):
                self.inserted.discard(a)
                self.armed.discard(a)
                self.nopatch.add(a)
                f = self.funcs.get(a)
                if f:
                    f.skip = "the code in RAM changed (patched at run time)"
                    self.emit("skip", fn="%08x" % a, name=f.name, why=f.skip)
                continue
            self.link.z(a)
            self.link.Z(a)
            if self.link.word(a) != TRAP:
                self.link.z(a)
                self.inserted.discard(a)
                self.armed.discard(a)
                self.emit("note", what="breakpoint would not go in", addr="%08x" % a)
        return bad

    def health(self):
        """a few of our words read back; all of them if one is missing"""
        if self.program_share() < 0.9:
            self.away()
            return
        pool = list(self.inserted)
        for a in self.rng.sample(pool, min(24, len(pool))):
            if self.link.word(a) != TRAP:
                bad = self.verify_all()
                self.say("%d breakpoints were gone from RAM (the game reloaded or rewrote its code): set again"
                         % bad)
                self.emit("note", what="breakpoints missing from RAM, set again", count=bad)
                return

    def write_cpu(self, v):
        self.link.write(CPU_ON_OFF, struct.pack("<i", v))
        got = struct.unpack("<i", self.link.mem(CPU_ON_OFF, 4))[0]
        self.say("cpu_on_off = %d" % got)
        self.emit("note", what="cpu_on_off written", value=got)

    # ---- one stop ----
    def peek(self, v):
        if not PEEK or not in_ram(v):
            return None
        a = norm(v)
        try:
            return self.link.mem(a, min(PEEK, RAM_HI - a)).hex()
        except RuntimeError:
            return None

    def change_mode(self, mode, fc):
        self.emit("mode", **{"from": self.mode, "to": mode, "name": self.mode_name(mode), "fc": fc})
        self.mode = mode
        for a in list(self.armed | self.parked) + list(self.sub_handlers) + list(self.main_handlers):
            self.update_entry(a)

    def finish(self, p):
        lst = self.pend.get(p.site, [])
        if p in lst:
            lst.remove(p)
        if not lst:
            self.pend.pop(p.site, None)
        self.busy.discard(p.fn.addr)
        self.update_entry(p.fn.addr)
        self.refresh(p.site)

    def returns(self, pc, regs, mode, fc):
        lst = self.pend.get(pc)
        if not lst:
            return False
        sp = regs[15]
        below = [p for p in lst if p.sp <= sp]
        if not below:
            return False     # a deeper call through the same site came back
        top = max(p.sp for p in below)
        for p in below:
            if p.sp == top:
                fb = regs[R_FR0]
                fv = struct.unpack("<f", struct.pack("<I", fb))[0]
                self.rets += 1
                self.emit("ret", id=p.id, fn="%08x" % p.fn.addr, name=p.fn.name, r0=regs[0], r1=regs[1],
                          fr0b=fb, fr0=fv if math.isfinite(fv) else None, sp=sp, dsp=sp - p.sp,
                          mode=mode, fc=fc, dfc=fc - p.fc, pk=self.peek(regs[0]))
            else:
                self.emit("noret", id=p.id, fn="%08x" % p.fn.addr, name=p.fn.name,
                          why="its frame was left without the return being seen")
            self.finish(p)
        return True

    def expire(self, fc):
        now = time.time()
        for site in list(self.pend):
            for p in list(self.pend.get(site, ())):
                late = fc - p.fc
                if late >= RET_FRAMES or late < 0 or now - p.t > RET_SECS:
                    p.fn.noret = True
                    self.emit("noret", id=p.id, fn="%08x" % p.fn.addr, name=p.fn.name,
                              why="no return within %d frames" % RET_FRAMES if late >= 0 else "frame count reset")
                    self.finish(p)

    def guard(self, regs):
        """the game is about to copy a jump over a function's entry"""
        target = norm(regs[4])
        rng = range(target & ~1, (target & ~1) + PATCH_LEN, 2)
        self.nopatch.update(rng)
        for a in rng:
            for p in list(self.pend.get(a, ())):
                self.emit("noret", id=p.id, fn="%08x" % p.fn.addr, name=p.fn.name, why="return site rewritten")
                self.finish(p)
            self.armed.discard(a)
            if a in self.inserted:
                self.remove(a)
            f = self.funcs.get(a)
            if f and not f.skip:
                f.skip = "the game rewrites this entry at run time (%s)" % self.img.name.get(norm(regs[R_PC]), "?")
                self.parked.discard(a)
                self.emit("skip", fn="%08x" % a, name=f.name, why=f.skip)

    def entry(self, pc, regs, mode, fc, vb, changed):
        if pc in self.guards:
            self.guard(regs)
        f = self.funcs.get(pc)
        if not (f and pc not in self.busy and f.wants(mode)):
            if not changed and pc not in self.guards and self.sentinel(pc):
                # called although the game is in another mode: not a usable sentinel
                self.false_sentinel[pc] += 1
                if self.false_sentinel[pc] >= 3:
                    self.sub_handlers.pop(pc, None)
                    self.main_handlers.pop(pc, None)
            self.update_entry(pc)
            return
        pr = regs[R_PR]
        f.count(pr, mode)
        self.hits += 1
        self.seq += 1
        hid = "%s:%d" % (RUN, self.seq)
        capture = None
        if f.exc:
            capture = "entered by an exception: PR is not its return address"
        elif f.noret:
            capture = "it did not return earlier"
        elif f.rets >= RET_N:
            capture = "return already captured %d times" % f.rets
        else:
            capture = self.img.site_reason(pr) or self.can_insert(norm(pr))
        site = norm(pr)
        who = self.img.containing(pr)
        frb = regs[R_FR0 + 4:R_FR0 + 8]
        fr = [struct.unpack("<f", struct.pack("<I", b))[0] for b in frb]
        rec = dict(id=hid, h=f.hits, fn="%08x" % pc, name=f.name, pr="%08x" % pr,
                   caller=who[1] if who else None, coff=who[2] if who else None,
                   r=regs[4:8], frb=frb, fr=[v if math.isfinite(v) else None for v in fr],
                   r03=regs[0:4], sp=regs[15], mode=mode, fc=fc, vb=vb,
                   ret=1 if capture is None else capture)
        if regs[R_PC] != pc:
            rec["pc"] = "%08x" % regs[R_PC]
        if f.exc:
            rec["exc"] = 1
        try:
            if in_ram(regs[15]):
                rec["stk"] = list(struct.unpack("<4I", self.link.mem(norm(regs[15]), 16)))
        except RuntimeError:
            pass
        pk = {}
        for i in range(4):
            b = self.peek(regs[4 + i])
            if b:
                pk[str(4 + i)] = b
        if pk:
            rec["pk"] = pk
        self.emit("hit", **rec)
        if capture is None:
            f.rets += 1
            self.busy.add(pc)
            self.pend.setdefault(site, []).append(Pending(f, regs[15], hid, fc, site))
            self.update_entry(pc)
            self.refresh(site)
            if site not in self.inserted:      # could not be set after all
                p = self.pend[site][-1]
                self.emit("noret", id=hid, fn="%08x" % pc, name=f.name, why="return breakpoint refused")
                self.finish(p)
        else:
            if f.wants(mode):
                self.defer.add(pc)
            self.update_entry(pc)

    def foreign(self, pc, rawpc):
        """a stop that is not one of ours"""
        sig = self.last_sig
        self.unexpected[pc] += 1
        inside = self.img.code_lo <= pc < self.img.code_hi
        w = self.link.word(pc) if in_ram(rawpc) else None
        where = self.img.containing(pc)
        where = "%s+%#x" % where[1:] if where else "outside the image"
        if sig == "SIGTRAP" and w == TRAP and inside and self.img.word(pc) != TRAP:
            r = self.link.z(pc)
            if self.link.word(pc) == TRAP:
                self.link.write(pc, struct.pack("<H", self.img.word(pc)))
                r = "image word written back"
            self.say("a breakpoint that is not ours at %08x (%s): removed (%s)" % (pc, where, r))
            self.emit("note", what="foreign breakpoint removed", addr="%08x" % pc, where=where, how=r)
            return
        self.emit("note", what="unexpected stop", signal=sig, pc="%08x" % rawpc, where=where, word=w)
        if sig == "SIGTRAP" and w is not None and w & 0xFF00 == 0xC300:
            self.stop_req = ("the game ran its own trapa #%#x at %08x (%s): with a debugger attached Flycast "
                             "halts there instead of raising the game's exception" % (w & 0xFF, rawpc, where))
        elif sig not in ("SIGTRAP", "SIGINT") or self.unexpected[pc] > 20:
            self.stop_req = "the emulated CPU stopped with %s at %08x (%s)" % (sig, rawpc, where)

    def on_stop(self):
        regs = self.link.regs()
        rawpc = regs[R_PC]
        pc = norm(rawpc)
        mode, fc, vb = self.read_mode()
        self.stops += 1
        mine = pc in self.inserted and self.last_sig == "SIGTRAP"   # a halt we asked for is not a hit
        todo, self.defer = self.defer, set()
        changed = mode != self.mode
        if changed:
            self.change_mode(mode, fc)
        if mine:
            self.returns(pc, regs, mode, fc)
            if pc in self.armed:
                self.entry(pc, regs, mode, fc, vb, changed)
        elif self.last_sig != "SIGINT":
            self.foreign(pc, rawpc)
        elif not self.int_sent and not self.stop_req:
            self.stop_req = "interrupted (Ctrl-C)"
        for a in todo:
            self.update_entry(a)
        self.expire(fc)
        if pc in self.inserted:
            self.step_over(pc)

    def housekeeping(self, idle):
        if self.cmd_cpu is not None:
            v, self.cmd_cpu = self.cmd_cpu, None
            self.write_cpu(v)
        if idle:
            self.health()
        now = time.time()
        if now - self.last_status > 5 or idle:
            self.status()
        if now - self.last_save > 30 or idle:
            self.save_state()

    # ---- the loop and the watcher ----
    def watch(self, rfd):
        """a thread: turns a signal, the STOP / CPU files, the time limit
        and a long silence into a halt of the game the main loop can act on"""
        stop_file, cpu_file = os.path.join(OUT, "STOP"), os.path.join(OUT, "CPU")
        while not self.dead:
            try:
                if select.select([rfd], [], [], 0.25)[0]:
                    os.read(rfd, 64)
                    self.stop_req = self.stop_req or "signal"
                if os.path.exists(stop_file):
                    os.remove(stop_file)
                    self.stop_req = self.stop_req or "work/observe/STOP"
                if os.path.exists(cpu_file):
                    try:
                        self.cmd_cpu = int(open(cpu_file).read().split()[0], 0)
                    except (ValueError, IndexError):
                        pass
                    os.remove(cpu_file)
                now = time.time()
                if TIME_LIMIT and now - self.t0 > TIME_LIMIT:
                    self.stop_req = self.stop_req or "time limit"
                quiet = now - self.last_stop
                need = (self.stop_req or self.cmd_cpu is not None) and quiet > 0.5 or quiet > IDLE
                if need and self.in_continue and not self.int_posted:
                    self.int_posted = True
                    gen = self.gen
                    gdb.post_event(lambda: self.interrupt(gen))
            except Exception:
                print(traceback.format_exc())

    def interrupt(self, gen):
        """on gdb's own thread: halt the game, if it still runs the resume
        the watcher saw"""
        if self.in_continue and gen == self.gen:
            self.int_sent = True
            try:
                gdb.execute("interrupt")
            except gdb.error as e:
                print("observe: interrupt:", e, flush=True)

    def resume(self):
        self.last_sig = None
        self.gen += 1
        self.int_posted = self.int_sent = False
        self.in_continue = True
        try:
            gdb.execute("continue", to_string=True)
        finally:
            self.in_continue = False

    def on_gdb_stop(self, ev):
        # gdb reports a SIGTRAP stop as a plain stop event, every other signal by name
        self.last_sig = getattr(ev, "stop_signal", None) or "SIGTRAP"

    def loop(self):
        while not self.stop_req:
            self.resume()
            idle = self.int_sent and self.last_sig == "SIGINT"
            self.last_stop = time.time()
            self.on_stop()
            self.housekeeping(idle)
            if MAX_STOPS and self.stops >= MAX_STOPS:
                self.stop_req = "stop limit"

    def cleanup(self):
        """every breakpoint out, checked against the code; then resume"""
        for a in sorted(self.inserted):
            self.remove(a)
        self.armed.clear()
        self.pend.clear()
        _, n = sweep(self.link, self.img, (), self.say)
        if n["stray"]:
            self.say("%d trapa words were still in the code after the removal: %d removed, %d put back from the image"
                     % (n["stray"], n["removed"], n["restored"]))
        return n

    def wait_for_image(self):
        """VF4OBS_BOOT: let the machine run in short stretches until the
        image is in RAM and the PC is inside it"""
        probe = self.img.starts[::61]
        last = time.time()
        while not self.stop_req:
            regs = self.link.regs()
            ok = sum(1 for a in probe if self.link.word(a) == self.img.word(a)) >= 0.97 * len(probe)
            if ok and self.img.code_lo <= norm(regs[R_PC]) < self.img.code_hi:
                return True
            if time.time() - last > 10:
                self.say("  still waiting (PC %08x, image %s)" % (regs[R_PC], "in RAM" if ok else "not in RAM"))
                last = time.time()
            self.last_stop = time.time() - IDLE + 0.2      # ask the watcher for a halt in 0.2 s
            self.resume()
        return False


def emergency_clear():
    """our connection cannot be used: drop it and let a new gdb remove the
    breakpoints (a new client halts the machine, clear mode restarts it)"""
    print("observe: removing the breakpoints through a second connection", flush=True)
    try:
        gdb.execute("disconnect")
    except gdb.error:
        pass
    env = dict(os.environ, VF4OBS_MODE="clear")
    try:
        subprocess.run(["gdb", "-q", "-batch", "-nx", "-x", os.path.join(HERE, "flycast_observe.py")],
                       env=env, timeout=120)
    except (OSError, subprocess.SubprocessError) as e:
        print("observe: could not run clear mode (%s). Run it by hand:" % e)
        print("    VF4OBS_MODE=clear gdb -q -batch -nx -x tools/flycast_observe.py")


def observe(img):
    funcs = targets(img)
    link = connect()
    if link is None:
        return 1
    s = Session(img, link, funcs)
    gdb.events.stop.connect(s.on_gdb_stop)
    rfd, wfd = os.pipe()
    os.set_blocking(wfd, False)
    try:
        signal.set_wakeup_fd(wfd, warn_on_full_buffer=False)
        for sig in (signal.SIGTERM, signal.SIGHUP):
            signal.signal(sig, lambda *a: None)
    except (ValueError, OSError) as e:
        print("observe: signals not hooked (%s); stop with: touch %s" % (e, os.path.join(OUT, "STOP")))
    watcher = threading.Thread(target=s.watch, args=(rfd,), daemon=True)
    watcher.start()
    clean = detached = False
    try:
        try:
            if ENV("VF4OBS_BOOT") == "1":
                os.makedirs(OUT, exist_ok=True)
                s.live0 = link.mem(img.code_lo, img.code_hi - img.code_lo)
                if s.loaded() < 0.98:
                    s.say("waiting for the game image to be loaded")
                    if not s.wait_for_image():
                        gdb.execute("detach")
                        return 1
            why = s.prepare()
            if why:
                print("observe:", why)
                print("observe: nothing was set. Wait for the attract mode and start again "
                      "(or use VF4OBS_BOOT=1 to wait here).")
                gdb.execute("detach")
                return 1
            s.say("observing; stop with Ctrl-C, or from another terminal: touch %s"
                  % os.path.relpath(os.path.join(OUT, "STOP"), ROOT))
            s.loop()
            s.say("stopping: %s" % s.stop_req)
        except KeyboardInterrupt:
            s.stop_req = "Ctrl-C"
            s.say("stopping: Ctrl-C")
        except gdb.error as e:
            s.stop_req = "gdb: %s" % e
            s.say("stopping:", s.stop_req)
            if "closed" in str(e).lower() or "not being run" in str(e).lower():
                s.dead = True
        except Exception:
            s.stop_req = "error"
            print(traceback.format_exc())
        if s.dead:
            print("observe: the connection to Flycast is gone. If Flycast is still open, run clear mode.")
        else:
            n = s.cleanup()
            clean = True
            if s.log:
                s.emit("end", why=str(s.stop_req), stops=s.stops, hits=s.hits, returns=s.rets,
                       seen=sum(1 for f in s.funcs.values() if f.hits),
                       finished=sum(1 for f in s.funcs.values() if f.done),
                       seconds=round(time.time() - s.t0, 1), packets=link.packets, left=n["stray"])
            gdb.execute("detach")
            detached = True
            if n.get("absent"):
                clean = False       # keep the journal: the next session or clear mode unlists them
                s.say("the game's code was not in RAM at the end, so nothing of ours is in memory; Flycast "
                      "still lists the addresses, which the next session (or clear mode) takes off. "
                      "Detached, the emulator runs")
            else:
                s.say("all breakpoints removed; detached, the emulator runs")
    except BaseException:
        print(traceback.format_exc())
    finally:
        s.dead = True
        if s.log:
            try:
                s.save_state()
                s.status()
                s.say("log: %s" % os.path.relpath(s.log.name, ROOT))
            except Exception:
                print(traceback.format_exc())
        if clean and s.journal:
            s.journal.close()
            os.remove(s.journal.name)
        elif not clean and s.journal and not detached:
            emergency_clear()
    return 0 if clean or detached else 1


def check(img):
    funcs = targets(img)
    names, sub, main = img.modes()
    why = collections.Counter(f.skip for f in funcs.values() if f.skip)
    print("%d functions in the map, %d to watch, code %08x-%08x" % (
        len(img.starts), len(funcs), img.code_lo, img.code_hi))
    print("%d addresses are reached as delay slots; %d entries cannot take a breakpoint:" % (
        len(img.slots), sum(why.values())))
    for f in sorted(funcs.values(), key=lambda f: f.addr):
        if f.skip:
            print("  %08x %-32s %s" % (f.addr, f.name, f.skip))
    exc = sorted(f.addr for f in funcs.values() if f.exc)
    print("%d are entered by an exception (hits logged, no return capture):" % len(exc))
    print("  " + " ".join(img.name[a] for a in exc))
    print("mode table: %d modes, %d sub-mode handlers, %d main handlers; guards: %s" % (
        len(names), len(sub), len(main), ", ".join(n for n in GUARDS if n in img.by_name) or "none"))
    return 0


def main():
    if not os.path.exists(IMAGE):
        print("observe: %s is missing (the game's code image the checks need)" % IMAGE)
        return 1
    img = Image()
    if MODE == "check" or gdb is None:
        return check(img)
    if MODE == "clear":
        return clear_mode(img)
    if MODE == "observe":
        return observe(img)
    print("observe: VF4OBS_MODE must be observe, clear or check")
    return 1


if __name__ == "__main__":
    code = main()
    if gdb is not None:
        sys.stdout.flush()
        gdb.execute("quit %d" % code)
    else:
        sys.exit(code)
