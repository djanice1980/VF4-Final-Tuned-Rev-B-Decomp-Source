#!/usr/bin/env python3
"""Decode the move records (mothead_<bank>.bin) to JSON.

Read from the rob code (names in ghidra/symbols/move.tsv). A bank's file
starts with one u32 offset per motion of the bank (motion id - first, 0 = no
record); offsets are relative to the file (pol_uti-style relocation at load).
A record is 20 bytes (move_start, 0x0C0659E0):
  +0x00  u32 flags (rob +0x324; bit 8 / 22 preset +0x2F6 / +0x2F8, bit 11
         keeps the held-face flag, bit 14 / 15 / 26 more state)
  +0x04  u8 (rob +0x284), +0x05 u8 (rob +0x285; bit 6 is a side flag)
  +0x08  command list { u32 argument offset, u32 command } ending in 0, run
         once at the move start through move_commands (52 handlers)
  +0x0C  timed list { u32 entry offset, u32 command } ending in 0; an entry
         is { s16 frame, arguments }; move_timed_run calls
         move_timed_commands[command] (46 handlers) once the move reaches
         the frame
  +0x10  branch list { u32 branch offset, 0 } ending in 0: a branch is
         { u8 flags, s8 input, s8 input2, s8 condition, u16 value, s16 start,
         s16 end, u16, u32 script offset } (move_branch_try): live from frame
         start (-1: from the beginning) to end, it needs input patterns
         input and input2 (input_match, 0 = any) and the condition
         (move_branch_cond), then its script picks the next move
         (move_script_exec byte code: tools/movescript.py decodes it into
         the JSON as script_code)
Each command's fields below are named after what its handler and the code
that reads the rob field do with them (the rob offset when that is all the
evidence says); see docs/asset-formats.md, Move records. Arguments are
written as raw hex as well, so nothing is lost. Attacks (record flag bit 14
with frames and attack commands) also get "startup" and "advantage": guard,
hit and counter-hit frame advantage by the game's own formula
(frame_advantage, 0x0C0F0680).

usage: mothead.py <asset dir> <main.bin> <motion json dir> <out dir>
"""
import json
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import motion  # noqa: E402

# command -> (name, [(field, type, offset)], argument size); types: b B h H i I f x (hex u32)
S3 = [("value", "h", 0), ("frame", "h", 2), ("extra", "h", 4)]
COMMANDS = {
    0: ("follow", [("move", "i", 0), ("rob_2de", "h", 4), ("rob_2e0", "h", 6), ("rob_2e2", "h", 8)], 12),
    1: ("frames", [("active_start", "h", 0), ("active_end", "h", 2), ("recovery_end", "h", 4)], 6),
    2: ("cmd02", [("rob_32", "B", 0), ("rob_2ee", "h", 2), ("rob_2f0", "h", 4), ("rob_2f2", "h", 6),
                  ("rob_34c", "x", 0x0C), ("rob_354_add", "f", 0x10), ("rob_350", "x", 0x14)], 24),
    3: ("attack", [("attack_flags", "B", 0), ("limb", "B", 1), ("level", "B", 2), ("rob_289", "B", 3),
                   ("rob_28a", "B", 4), ("rob_28b", "B", 5), ("rob_28c", "B", 6), ("damage", "h", 8),
                   ("rob_2a4", "h", 0x0A), ("rob_2a6", "h", 0x0C), ("rob_328", "f", 0x10), ("rob_32c", "f", 0x14)], 24),
    4: ("frame_table", [("guard_stun", "h", 0), ("hit_stun", "h", 2), ("counter_stun", "h", 4),
                        ("rob_364", "f", 8), ("rob_368", "f", 0x0C),
                        ("guard_recovery", "h", 0x10), ("hit_recovery", "h", 0x12), ("counter_recovery", "h", 0x14),
                        ("state_recovery", "h", 0x16), ("state_stun", "h", 0x18),
                        ("guard_wait", "h", 0x1A), ("hit_wait", "h", 0x1C), ("counter_wait", "h", 0x1E),
                        ("state_wait", "h", 0x20), ("row5_stun", "h", 0x22)], 36),
    5: ("frame_rows", [(f"{r}_{c}", "h", 2 * (3 * i + j)) for i, r in enumerate(("r4000", "r8000", "r10000", "r800"))
                       for j, c in enumerate(("stun", "recovery", "wait"))], 24),
    **{k: (f"frame_row_{r}", [("stun", "h", 0), ("recovery", "h", 2), ("wait", "h", 4)], 6) for k, r in
       ((6, "hit"), (7, "guard"), (8, "counter"), (9, "r4000"), (10, "r8000"), (11, "r10000"), (12, "r800"), (13, "row5"))},
    14: ("nop", [], 0),
    15: ("throw_damage", [("damage", "h", 2), ("rob_2f6", "h", 4), ("rob_294", "h", 6), ("rob_296", "h", 0x0A),
                          ("rob_298", "h", 0x0C)], 14),
    16: ("cmd16", [("rob_2f2", "h", 0), ("rob_2f6", "h", 2), ("rob_294", "h", 4), ("rob_296", "h", 6)], 8),
    17: ("cmd17", [("rob_2fa", "h", 0), ("rob_358", "x", 4), ("rob_35c", "x", 8), ("rob_200", "f", 0x0C)], 16),
    18: ("nop", [], 0),
    19: ("list_374", [], None),
    20: ("damage_by_distance", [("near", "B", 0), ("middle", "B", 2), ("near_distance", "f", 4),
                                ("middle_distance", "f", 8), ("far_distance", "f", 0x0C)], 16),
    21: ("cmd21", [("rob_300", "h", 0), ("rob_302", "h", 2), ("rob_304", "h", 4), ("rob_306", "h", 6),
                   ("rob_28f", "B", 8), ("rob_308", "h", 0x0A)], 12),
    22: ("cmd22", [("rob_330", "x", 0), ("rob_338", "x", 4)], 8),
    23: ("cmd23", [("rob_334", "x", 0)], 4),
    24: ("cmd24", [("rob_2f6", "h", 0)], 2),
    25: ("cmd25", [("rob_1bd", "B", 0), ("rob_29c", "h", 2), ("rob_290", "B", 4), ("rob_29e", "h", 6)], 8),
    26: ("cmd26", [("rob_33c", "x", 0), ("rob_30a", "h", 4)], 8),
    27: ("list_378", [], None),
    28: ("list_37c", [], None),
    29: ("list_380", [], None),
    30: ("cmd30", [("rob_348", "x", 0), ("rob_29a", "h", 4)], 8),
    31: ("cmd31", [("rob_2ec", "h", 0)], 2),
    32: ("cmd32", [("rob_2f8", "h", 0)], 2),
    33: ("flags24", [("flags", "x", 0)], 4),
    34: ("flags28", [("flags", "x", 0)], 4),
    35: ("flags2c", [("flags", "x", 0)], 4),
    36: ("face_motion", [("motion", "h", 0)], 2),
    37: ("cmd37", [("rob_30e", "h", 0), ("rob_310", "h", 2)], 4),
    38: ("hands_flag", [], 0),
    39: ("flag28_bit15", [], 0),
    40: ("velocity_scale", [("scale", "f", 0), ("rob_20c", "f", 4)], 8),
    41: ("cmd41", [("bit10", "B", 0)], 1),
    42: ("flags3a6", [("flags", "H", 0)], 2),
    43: ("cmd43", [("rob_36c", "f", 0)], 4),
    44: ("list_384", [], None),
    45: ("cmd45", [("rob_312", "h", 0), ("rob_314", "h", 2)], 4),
    46: ("nop", [], 0),
    47: ("cmd47", [("rob_3aa", "h", 0)], 2),
    48: ("cmd48", [("rob_178", "B", 0)], 1),
    49: ("nop", [], 0),
    50: ("flag344", [("bit", "B", 0)], 1),
    51: ("nop", [], 0),
}
BIT = [("bit", "h", 2)]
TIMED = {
    0: ("random_follow", [("every", "B", 2), ("chance", "B", 3), ("move", "i", 4)], 8),
    1: ("sound", [("sound", "h", 2)], 4),
    2: ("voice", [("voice", "h", 2)], 4),
    3: ("tcmd03", [("rob_2f6_add", "h", 2)], 4),
    4: ("set_flag20", BIT, 4), 5: ("clear_flag20", BIT, 4),
    6: ("toggle_side", [], 2),
    7: ("tcmd07", [], 2),
    8: ("launch", [("heading", "h", 2), ("speed", "f", 4), ("rise", "f", 8)], 12),
    9: ("sound_flag", [("sound", "h", 2)], 4),
    10: ("state33", [("op", "h", 2)], 4),
    11: ("nop", [], 2),
    12: ("tcmd12", [("rob_12", "h", 2)], 4),
    13: ("hand_shape", [("hands", "H", 2), ("value", "f", 4), ("until", "h", 8)], 12),
    14: ("rigid_face", [("face", "h", 2)], 4),
    15: ("damage_tick", [("damage", "h", 2)], 4),
    16: ("tcmd16", [("rob_577", "B", 2)], 4),
    17: ("camera", [("camera", "h", 2), ("kind", "B", 4), ("length", "h", 6)], 8),
    18: ("tcmd18", [("op", "B", 2), ("value", "B", 3), ("distance", "f", 4)], 8),
    19: ("adjust_177", [("delta", "b", 2)], 4),
    20: ("adjust_177_if", [("when", "B", 2), ("delta", "b", 3)], 4),
    21: ("adjust_177_opponent", [("when", "B", 2), ("delta", "b", 3)], 4),
    22: ("approach", [("until", "h", 2), ("flags", "H", 4), ("min", "f", 8), ("max", "f", 0x0C),
                      ("target_x", "f", 0x10), ("target_z", "f", 0x14), ("offset", "f", 0x18), ("rise", "f", 0x1C)], 32),
    23: ("tcmd23", [("kind", "h", 2), ("reaction", "H", 4), ("mask", "x", 8), ("distance", "f", 0x0C)], 16),
    24: ("tcmd24", [("value", "h", 2), ("compare", "h", 4), ("rob_64", "x", 8)], 12),
    25: ("set_flag24", BIT, 4), 26: ("clear_flag24", BIT, 4),
    27: ("set_flag28", BIT, 4), 28: ("clear_flag28", BIT, 4),
    29: ("set_flag2c", BIT, 4), 30: ("clear_flag2c", BIT, 4),
    31: ("opponent_108", [("value", "h", 2), ("mask", "x", 4)], 8),
    32: ("opponent_108_add", [("value", "h", 2), ("mask", "x", 4)], 8),
    33: ("slot_effect", [("mode", "h", 2), ("slot", "h", 4), ("length", "h", 6)], 8),
    34: ("tcmd34", [("rob_550", "h", 2)], 4),
    35: ("tcmd35", [("op", "h", 2)], 4),
    36: ("set_flag318", BIT, 4), 37: ("clear_flag318", BIT, 4),
    38: ("tcmd38", [("until", "h", 2)], 4),
    39: ("limb_target", [("until", "h", 2), ("release", "B", 4), ("limb", "B", 5), ("kind", "B", 6),
                         ("x", "f", 8), ("y", "f", 0x0C), ("z", "f", 0x10)], 20),
    40: ("stop", [], 2),
    41: ("eye_look", [("look", "B", 2)], 4),
    42: ("tcmd42", [], 2),
    43: ("nop", [], 2),
    44: ("tcmd44", [("frames", "h", 2), ("rob_340", "x", 4), ("rob_370", "x", 8)], 12),
    45: ("tcmd45", [], 2),
}


# attack levels (rob +0x287): the code's three tests 0x0C09C0E0 / 0C09C140 /
# 0C09C1A0 group them high {1, 7}, mid {2, 4}, low {3, 5, 6, 13}; 8-10 are
# throws (the guard code skips them), 11 an air grab, 12 / 14 down attacks
LEVELS = {0: "none", 1: "high", 2: "mid", 3: "low", 4: "mid (special)", 5: "low (5)", 6: "low (special)",
          7: "high (7)", 8: "throw", 9: "throw (9)", 10: "throw (10)", 11: "air grab", 12: "down attack",
          13: "low (13)", 14: "down attack (14)"}


def _cdiv(a, b):
    """C integer division (toward zero), as the SH-4 code computes it."""
    q = abs(a) // abs(b)
    return q if (a >= 0) == (b > 0) else -q


def guard_stun(stun, damage):
    """0x0C071A80: the guard row's stun, or from the damage (10 + 2/3 of it,
    6 + under 15 damage), clamped to 10..26."""
    if stun:
        return stun
    v = (6 if damage < 15 else 10) + _cdiv(damage * 2, 3)
    return max(10, min(26, v))


def hit_stun(stun, damage, start, end, flags):
    """0x0C071900 with the frame at the first active one: the row's stun, or
    from the damage (8 + 4/5 of it) kept within the frames left - 2 .. + 6
    (attack flags +0x0A bit 0 drops the lower bound; bit 1 forces the
    damage formula and the move's whole length as the floor)."""
    if stun and not flags & 2:
        return stun
    v = _cdiv(damage * 4, 5) + 8
    left, total = end - start, end - start
    if flags & 2:
        return max(total, v)
    v = min(v, left + 6)
    if not flags & 1:
        v = max(v, left - 2)
    return v


def advantage(frames, attack, table):
    """Frame advantage on guard, hit and counter hit as the game's training
    display computes it (0x0C0F0680): stun - (recovery end - first active
    frame), the recovery end being the row's or the move's; a counter hit
    adds half the damage. Assumes neither fighter in a special state and the
    default game options."""
    start, end = frames["active_start"], frames["recovery_end"]
    t = table or {}
    dmg, fl = attack["damage"], attack["rob_2a4"]
    rec = lambda k: t.get(k, -1) if t.get(k, -1) != -1 else end
    g = guard_stun(t.get("guard_stun", 0), dmg) - (rec("guard_recovery") - start)
    h = hit_stun(t.get("hit_stun", 0), dmg, start, end, fl) - (rec("hit_recovery") - start)
    c = hit_stun(t.get("counter_stun", 0), dmg + (dmg >> 1), start, end, fl) - (rec("counter_recovery") - start)
    return {"guard": g, "hit": h, "counter": c}


def _fields(d, a, spec):
    out = {}
    for name, t, off in spec:
        if t == "x":
            out[name] = f"{struct.unpack_from('<I', d, a + off)[0]:#010x}"
        else:
            v = struct.unpack_from("<" + t, d, a + off)[0]
            out[name] = round(v, 6) if t == "f" else v
    return out


def records(d, first, end):
    """motion id -> decoded move record."""
    u = lambda o: struct.unpack_from("<I", d, o)[0]
    refs = sorted({u(4 * i) for i in range(end - first) if u(4 * i)})
    out = {}
    for i in range(end - first):
        r = u(4 * i)
        if not r:
            continue
        m = {"flags": f"{u(r):#010x}", "byte4": d[r + 4], "byte5": d[r + 5], "commands": [], "timed": [], "branches": []}
        c = u(r + 8)
        while c and u(c):
            a, k = u(c), u(c + 4)
            name, spec, size = COMMANDS.get(k, (f"cmd{k:02d}", [], None))
            e = {"cmd": k, "name": name, "arg": f"{a:#x}"}
            e.update(_fields(d, a, spec))
            if k == 3:
                e["level_name"] = LEVELS.get(e["level"], "?")
            if size:
                e["raw"] = d[a:a + size].hex()
            m["commands"].append(e)
            c += 8
        c = u(r + 12)
        while c and u(c):
            a, k = u(c), u(c + 4)
            name, spec, size = TIMED.get(k, (f"tcmd{k:02d}", [], None))
            e = {"frame": struct.unpack_from("<h", d, a)[0], "cmd": k, "name": name}
            e.update(_fields(d, a, spec))
            if size and size > 2:
                e["raw"] = d[a + 2:a + size].hex()
            m["timed"].append(e)
            c += 8
        c = u(r + 16)
        while c and u(c):
            a = u(c)
            fl, i1, i2, cond = struct.unpack_from("<Bbbb", d, a)
            val, st, en, x = struct.unpack_from("<Hhhh", d, a + 4)
            m["branches"].append({"flags": fl, "input": i1, "input2": i2, "condition": cond, "value": val,
                                  "start": st, "end": en, "unknown": x, "script": f"{u(a + 12):#x}"})
            c += 8
        fr = next((c for c in m["commands"] if c["name"] == "frames"), None)
        at = next((c for c in m["commands"] if c["name"] == "attack"), None)
        if fr and at and u(r) & 0x4000:  # record flag bit 14: an attack
            tb = next((c for c in m["commands"] if c["name"] == "frame_table"), None)
            m["startup"] = fr["active_start"]
            m["advantage"] = advantage(fr, at, tb)
        out[first + i] = m
    return out, refs


def main():
    src, prog_path, motdir, dest = sys.argv[1:5]
    prog = open(prog_path, "rb").read()
    os.makedirs(dest, exist_ok=True)
    total = cmds = timed = branches = 0
    for bank in motion.bank_table(prog):
        if not bank["mothead"]:
            continue
        d = open(os.path.join(src, bank["mothead"]), "rb").read()
        recs, _ = records(d, bank["first"], bank["end"])
        names = {}
        mj = os.path.join(motdir, bank["motdat"].replace(".bin", ".json"))
        if os.path.exists(mj):
            names = {m["id"]: m["name"] for m in json.load(open(mj))["motions"]}
        moves = [{"id": k, "name": names.get(k), **v} for k, v in sorted(recs.items())]
        total += len(moves)
        cmds += sum(len(m["commands"]) for m in moves)
        timed += sum(len(m["timed"]) for m in moves)
        branches += sum(len(m["branches"]) for m in moves)
        with open(os.path.join(dest, bank["mothead"].replace(".bin", ".json")), "w") as f:
            json.dump({"bank": bank, "moves": moves}, f, indent=1)
    print(f"{total} moves, {cmds} commands, {timed} timed commands, {branches} branches -> {dest}", file=sys.stderr)


if __name__ == "__main__":
    main()
