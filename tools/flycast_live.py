"""Live checks through the GDB server of a Flycast built with
-DENABLE_GDB_SERVER=ON (port 3263):

    VF4LIVE_MODE=hits VF4LIVE_OUT=DIR gdb -q -batch -nx -x tools/flycast_live.py
    VF4LIVE_MODE=snap VF4LIVE_OUT=DIR gdb -q -batch -nx -x tools/flycast_live.py
    VF4LIVE_MODE=clear gdb -q -batch -nx -x tools/flycast_live.py
    VF4LIVE_MODE=cpu VF4LIVE_CPU=4 gdb -q -batch -nx -x tools/flycast_live.py
    VF4LIVE_MODE=trace VF4LIVE_OUT=DIR gdb -q -batch -nx -x tools/flycast_live.py
    VF4LIVE_MODE=phys VF4LIVE_OUT=DIR gdb -q -batch -nx -x tools/flycast_live.py

hits: breakpoints that log and continue (stop() returns False), no halts:
  attack_hit_resolve (0x0C06EE60)    the defender's state words before the
                                     hit result is decided
  attack_recovery_apply (0x0C06EA20) the result word every caller stored,
                                     the attacker's rows and the caller
  action_reaction_record_reset       every requested action word (+0x64):
    (0x0C072E60)                     rob_action_start calls it only when a
                                     request is pending
  Runs until gdb is stopped: gdb in batch mode ignores SIGINT, and SIGTERM
  ends it with the target running, so the breakpoints can stay in (its
  detach fails): always run clear mode after. VF4LIVE_WORDS=0 leaves out
  the request breakpoint (rob_action_start itself, which runs every frame
  for both robs, slowed the interpreter to a few frames a second; this one
  fires only on a request).
snap: one breakpoint at hit_lights_frame (0x0C0460C0, the end of
  robs_frame_step, after both skeletons, face and hands): halts once a
  fighter is mid motion-change blend (+0x3A6 bit 0, weight 0.2..0.8) and
  once one has an eye look-at running (+0x52D & 3), dumps the 32 MB of main
  RAM for each (DIR/blend_N.ram, DIR/eyes_N.ram, readable as a
  flycast_state RAM image), and detaches after VF4LIVE_SNAP_MAX of each
  (VF4LIVE_SNAP_KINDS=blend or eyes for one kind).
clear: after a gdb that was killed with its breakpoints in (Flycast's server
  neither removes them nor resumes when the socket drops), removes this
  script's breakpoints by raw z0 packets and detaches, which restarts the
  emulator.
cpu: writes cpu_on_off (0x0C2926F0, the debug menu's CPU switch: 0 NO TOUCH,
  the mode decides; 1 BOTH OFF; 2 1P ON; 3 2P ON; 4 BOTH ON) and detaches:
  vp_frame / enemy_task_frame then set or clear rob +0x18 bit 2 (set: not
  CPU-driven) on both robs every frame, so with 4 the game's AI fights
  itself and hits mode logs real CPU-vs-CPU exchanges.
trace: breakpoints that log and continue at rob_pose_eval (0x0C092700:
  the time it builds the pose with, fr4 / fr5 as GDB names them, beside
  the rob's frame +0x0A, frame position +0x70, speed +0x74, move, state,
  held frame +0x2F2, +0x318 and the snapshot ring's frame) and at
  rob_move_change (0x0C0656E0: the move started, the rob's frame before).
  VF4LIVE_TRACE_MOVES=a,b,... logs only those moves (and the changes into
  and out of them). Stop it as hits mode (SIGTERM, then clear mode).
phys: one breakpoint at osage_calc_all (0x0C0D3640, the chains' and then
  the cloth's frame, before the wind update): once a fight runs (game_flags
  0x110005 / 0x10005) with neither rob resetting its chains (+0x18 bit
  0x100000), halts on VF4LIVE_PHYS_FRAMES (default 4) frames in a row and
  dumps main RAM on each (DIR/phys_N.ram), then detaches; each frame's dump
  holds the chains and cloth as the previous frame left them and the
  skeleton this frame steps them on (tools/physics_live.py compares).
The other modes read memory only and write nothing but the breakpoints."""
import gdb, json, os, struct, time

MODE = os.environ.get("VF4LIVE_MODE", "hits")
OUT = os.environ.get("VF4LIVE_OUT", "live")
SNAP_MAX = int(os.environ.get("VF4LIVE_SNAP_MAX", "2"))
ROBS, ROB_SIZE = 0x0C37BA30, 0x86C
if MODE not in ("clear", "cpu"):
    os.makedirs(OUT, exist_ok=True)


def mem(a, n):
    return bytes(gdb.selected_inferior().read_memory(a, n))


def reg(name):
    return int(gdb.parse_and_eval("$" + name)) & 0xFFFFFFFF


def rob_fields(b):
    r = mem(b, 0x5A0)
    u8 = lambda o: r[o]
    s16 = lambda o: struct.unpack_from("<h", r, o)[0]
    u16 = lambda o: struct.unpack_from("<H", r, o)[0]
    u32 = lambda o: struct.unpack_from("<I", r, o)[0]
    return {
        "side": u8(0), "chara": u8(1), "move": u16(8), "frame": s16(0xA), "life": s16(0x10),
        "stun": s16(0x12), "flags": u32(0x18), "state": u32(0x20), "f24": u32(0x24),
        "f28": u32(0x28), "f2c": u32(0x2C), "action": u32(0x60), "request": u32(0x64),
        "pending": u32(0x68), "res10c": u32(0x10C), "res115": u8(0x115),
        "pending_damage": s16(0x228), "attack_damage": s16(0x2A0),
        "react_stun": [s16(0x2A8 + 2 * i) for i in range(5)],
        "react_recovery": [s16(0x2B2 + 2 * i) for i in range(5)],
        "result_rows": [[s16(0x2C6 + 6 * i + 2 * k) for k in range(3)] for i in range(4)],
        "active": [s16(0x2E6), s16(0x2E8)], "recovery_end": s16(0x2EA),
        "move_flags": u32(0x324), "attack_word": u32(0x328),
        "motion_flags": u16(0x3A6), "blend_weight": struct.unpack_from("<f", r, 0x3D0)[0],
        "blend_length": struct.unpack_from("<f", r, 0x3D8)[0], "eye_look": u8(0x52D),
        "eye_angles": [s16(0x52E + 2 * i) for i in range(6)],
    }


log = open(os.path.join(OUT, "%s.jsonl" % MODE), "a") if MODE not in ("clear", "cpu") else None
seq = [0]


def emit(kind, **kw):
    seq[0] += 1
    kw.update(kind=kind, seq=seq[0], t=round(time.time(), 3))
    log.write(json.dumps(kw) + "\n")
    log.flush()


class Logger(gdb.Breakpoint):
    def __init__(self, addr, fn):
        super().__init__("*0x%08x" % addr, internal=False)
        self.fn = fn

    def stop(self):
        try:
            self.fn()
        except Exception as e:  # keep the game running whatever a read does
            emit("error", where=self.location, error=repr(e))
        return False


TRACE_MOVES = {int(x, 0) for x in os.environ.get("VF4LIVE_TRACE_MOVES", "").split(",") if x}


def trace_fields(b):
    r = mem(b, 0x5A0)
    cur = struct.unpack_from("<I", r, 0x584)[0]
    return {"side": r[0], "chara": r[1], "move": struct.unpack_from("<H", r, 8)[0],
            "frame": struct.unpack_from("<h", r, 0xA)[0], "frame_pos": struct.unpack_from("<f", r, 0x70)[0],
            "speed": struct.unpack_from("<f", r, 0x74)[0], "state": struct.unpack_from("<I", r, 0x20)[0],
            "hold": struct.unpack_from("<h", r, 0x2F2)[0], "length": struct.unpack_from("<h", r, 0x2E4)[0],
            "side_flags": struct.unpack_from("<H", r, 0x318)[0],
            "motion_flags": struct.unpack_from("<H", r, 0x3A6)[0],
            "ring_frame": struct.unpack("<h", mem(cur + 8, 2))[0] if 0x0C000000 <= cur < 0x0E000000 else None}


def on_pose_eval():
    f = trace_fields(reg("r4"))
    if TRACE_MOVES and f["move"] not in TRACE_MOVES:
        return
    fr = {n: float(gdb.parse_and_eval("$" + n)) for n in ("fr4", "fr5")}
    emit("pose_eval", **fr, **f)


def on_move_change():
    f = trace_fields(reg("r4"))
    new = reg("r5") & 0x7FFF
    if TRACE_MOVES and new not in TRACE_MOVES and f["move"] not in TRACE_MOVES:
        return
    emit("move_change", new_move=new, **f)


def on_hit_resolve():
    a, d = reg("r4"), reg("r5")
    emit("hit_resolve", attacker=rob_fields(a), defender=rob_fields(d),
         hit_attack_level=mem(0x0C4220C4, 1)[0], hit_attack_flags=struct.unpack("<I", mem(0x0C4220C8, 4))[0],
         option_bits=struct.unpack("<I", mem(0x0C3744E8 + 0x14, 4))[0],
         game_flags=struct.unpack("<I", mem(0x0C2925B8, 4))[0])


def on_recovery_apply():
    c = reg("r4")
    result = struct.unpack("<I", mem(c + 0x3C, 4))[0]
    # the coli record's rob: the attacker is whichever rob's +0x3C points here
    side = None
    for s in range(2):
        if struct.unpack("<I", mem(ROBS + s * ROB_SIZE + 0x3C, 4))[0] == c:
            side = s
    att = rob_fields(ROBS + side * ROB_SIZE) if side is not None else None
    emit("recovery_apply", caller=reg("pr"), result=result,
         hit_result_flags=struct.unpack("<I", mem(0x0C4220D0, 4))[0], attacker=att,
         defender_state=(rob_fields(ROBS + (1 - side) * ROB_SIZE)["state"] if side is not None else None))


def on_action_start():
    b = reg("r4")
    f = rob_fields(b)
    emit("action_start", side=f["side"], chara=f["chara"], request=f["request"], action=f["action"],
         move=f["move"], frame=f["frame"], state=f["state"])


KINDS = os.environ.get("VF4LIVE_SNAP_KINDS", "blend,eyes").split(",")
snaps = {k: (0 if k in KINDS else SNAP_MAX) for k in ("blend", "eyes")}
want = [None]
PHYS_FRAMES = int(os.environ.get("VF4LIVE_PHYS_FRAMES", "4"))
phys = {"n": 0}


class PhysSnapper(gdb.Breakpoint):
    def __init__(self):
        super().__init__("*0x%08x" % 0x0C0D3640, internal=False)

    def stop(self):
        if phys["n"]:
            return True  # the frames after the first, whatever they hold
        try:
            if struct.unpack("<I", mem(0x0C2925B8, 4))[0] not in (0x110005, 0x10005):
                return False
            return not any(struct.unpack("<I", mem(ROBS + s * ROB_SIZE + 0x18, 4))[0] & 0x100000
                           for s in range(2))
        except Exception as e:
            emit("error", where="phys", error=repr(e))
        return False


class Snapper(gdb.Breakpoint):
    def __init__(self):
        super().__init__("*0x%08x" % 0x0C0460C0, internal=False)

    def stop(self):
        try:
            for s in range(2):
                b = ROBS + s * ROB_SIZE
                if not mem(b + 0x18, 1)[0] & 1:
                    continue
                mf = struct.unpack("<H", mem(b + 0x3A6, 2))[0]
                w = struct.unpack("<f", mem(b + 0x3D0, 4))[0]
                eye = mem(b + 0x52D, 1)[0]
                if snaps["blend"] < SNAP_MAX and mf & 1 and 0.2 <= w <= 0.8:
                    want[0] = ("blend", s)
                    return True
                if snaps["eyes"] < SNAP_MAX and eye & 3:
                    want[0] = ("eyes", s)
                    return True
        except Exception as e:
            emit("error", where="snap", error=repr(e))
        return False


def dump(kind, side):
    n = snaps[kind]
    path = os.path.join(OUT, "%s_%d.ram" % (kind, n))
    with open(path, "wb") as f:
        for a in range(0x0C000000, 0x0E000000, 0x40000):
            f.write(mem(a, 0x40000))
    f0 = rob_fields(ROBS + side * ROB_SIZE)
    f1 = rob_fields(ROBS + (1 - side) * ROB_SIZE)
    emit("snap", what=kind, side=side, path=path, rob=f0, other=f1)
    snaps[kind] += 1
    print("wrote", path, "side", side, "move", f0["move"], "frame", f0["frame"],
          "weight %.3f eye_look %d" % (f0["blend_weight"], f0["eye_look"]))


gdb.execute("set pagination off")
gdb.execute("set confirm off")
gdb.execute("set architecture sh4")
gdb.execute("set endian little")
gdb.execute("target remote 127.0.0.1:3263")

if MODE == "cpu":
    gdb.execute("set {int}0x0C2926F0 = %d" % int(os.environ.get("VF4LIVE_CPU", "4")))
    print("cpu_on_off =", struct.unpack("<i", mem(0x0C2926F0, 4))[0])
elif MODE == "clear":
    for a in (0x0C0460C0, 0x0C06EE60, 0x0C06EA20, 0x0C072F00, 0x0C072E60, 0x0C092700, 0x0C0656E0,
              0x0C0D3640):
        gdb.execute("maint packet z0,%x,2" % a)  # E01: not set
elif MODE == "trace":
    Logger(0x0C092700, on_pose_eval)
    Logger(0x0C0656E0, on_move_change)
    print("tracing pose builds and move changes to", os.path.join(OUT, "trace.jsonl"))
    try:
        gdb.execute("continue")
    except KeyboardInterrupt:
        pass
    except gdb.error as e:
        print("continue ended:", e)
elif MODE == "hits":
    Logger(0x0C06EE60, on_hit_resolve)
    Logger(0x0C06EA20, on_recovery_apply)
    if os.environ.get("VF4LIVE_WORDS", "1") != "0":
        Logger(0x0C072E60, on_action_start)
    print("logging hits and action words to", os.path.join(OUT, "hits.jsonl"), "- kill -INT this gdb to stop")
    try:
        gdb.execute("continue")
    except KeyboardInterrupt:
        pass
    except gdb.error as e:
        print("continue ended:", e)
elif MODE == "phys":
    PhysSnapper()
    print("waiting for a fight; %d frames of RAM to %s" % (PHYS_FRAMES, OUT))
    try:
        while phys["n"] < PHYS_FRAMES:
            gdb.execute("continue")
            path = os.path.join(OUT, "phys_%d.ram" % phys["n"])
            t0 = time.time()
            with open(path, "wb") as f:
                for a in range(0x0C000000, 0x0E000000, 0x40000):
                    f.write(mem(a, 0x40000))
            rows = [trace_fields(ROBS + s * ROB_SIZE) for s in range(2)]
            emit("phys", n=phys["n"], path=path, robs=rows,
                 game_flags=struct.unpack("<I", mem(0x0C2925B8, 4))[0])
            print("wrote", path, "in %.1f s;" % (time.time() - t0),
                  "frame_pos", [round(r["frame_pos"], 3) for r in rows])
            phys["n"] += 1
    except KeyboardInterrupt:
        pass
    except gdb.error as e:
        print("continue ended:", e)
else:
    Snapper()
    print("waiting for a blend and an eye look-at; snapshots to", OUT)
    try:
        while snaps["blend"] < SNAP_MAX or snaps["eyes"] < SNAP_MAX:
            want[0] = None
            gdb.execute("continue")
            if want[0] is None:
                break  # interrupted
            dump(*want[0])
    except KeyboardInterrupt:
        pass
    except gdb.error as e:
        print("continue ended:", e)

for bp in gdb.breakpoints() or ():
    bp.delete()
gdb.execute("detach")
print("detached")
