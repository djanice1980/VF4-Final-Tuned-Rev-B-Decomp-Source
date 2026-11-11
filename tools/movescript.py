#!/usr/bin/env python3
"""Disassemble the move scripts: the command-input byte code of ctrl_<chr>.bin
and of the mothead_ branch targets.

The interpreter (move_script_exec, 0x0C05E760; names in
ghidra/symbols/script.tsv) runs on a player's input object: +0x80 its input
history (64 frames of 0x28 bytes: u32 direction / button bits, u16 held at
+0x0A, u16 pressed at +0x0C, per-button counters from +0x10, u16 time at
+0x14), +0x84 its fighter (rob; rob +0x34 the opponent), +0x54 a script
register ("state"), +0x5C the fighter's control sets. It loops while the
opcode byte at pc is below 0x50, calling move_script_ops[op] (0x0C29F6CC,
8 bytes each: handler, instruction size). An instruction is { u8 op, s8 b1,
u16 h2 } and, by size, u32 w4, w8, wC. A handler returns 0 to go on or ends
the run: 1 / 2 a move was chosen (rob +0x64 = the move word: motion id in
the low 15 bits, flags above), 3 no match, 4 / 5 wait and resume.

Matching walks back through the input history: an input test that fails on
the current frame steps one frame back while the window (op 13) lasts, and a
failed test with no window left backtracks to the last choice point (op 10 /
11: a stack of 32 { state, pc }), or fails the script when there is none.

Two modes (DAT_0c421b1c): 0 runs a script from a pc, advancing by the
instruction size; from a branch window (move_branch_take) the script is a
list of instruction pointers and each successful step takes the next one
(advance, 0x0C05ED40); the list ends with a null pointer, which stops the run.

Control sets (ctrl_<chr>.bin, relocated at load): one u32 offset per set,
then the end offset, then 0; a fighter with stances has
several sets (LEI and SHN 5, KAG / LAU / SAR / VAN 2), picked by rob +0x550
(timed move command 34). A set is { u32 script[6], u32 table, u32 count }:
script 2 is the neutral command list (0x0C05CE80 runs it each frame), 0 runs
with state 0x2000000 | direction, 4 / 5 with state = direction; op 12 / 77
pick from the table in list mode. Offset 0x10 is a stub (op 0) for unused
slots.

usage: movescript.py <asset dir> <main.bin> <motion json dir> <out dir>
"""
import json
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mothead  # noqa: E402
import motion  # noqa: E402

# op -> (mnemonic, operands, flow); operands: b1 h2 (h4 h6) w4 w8 wC, with a suffix
# for how to print them: m = move word, j = jump target, r = rob field
# offset, f = float, x = hex. flow: next, end, jump (no fall-through),
# branch (fall-through and targets), call (fall-through, target is code
# that runs later)
OPS = {
    0: ("fail", "", "end"),
    1: ("resume_at", "w4j", "end"),
    2: ("move", "w4m", "end"),
    3: ("move_keep", "w4m", "end"),
    4: ("input", "h2n", "next"),
    5: ("allow", "h2n b1", "next"),
    6: ("input_bit", "b1n", "next"),
    7: ("not_input_bit", "b1n", "next"),
    8: ("new_press", "h2n", "next"),
    9: ("new_press_bit", "b1n", "next"),
    10: ("choice", "w4j", "branch"),
    11: ("choice_pc", "w4j", "branch"),
    12: ("continue_with", "w4j", "call"),
    13: ("window", "b1", "next"),
    14: ("wait_here", "", "next"),
    15: ("clear_wait", "", "next"),
    16: ("wait_frame", "b1 h2 w4r", "next"),
    17: ("before_frame", "b1 h2 w4r", "next"),
    18: ("flush_input", "", "next"),
    19: ("jump", "w4j", "jump"),
    20: ("if_state_eq", "w4s", "next"),
    21: ("jump_state_eq", "w4s w8j", "branch"),
    22: ("if_state_ge", "w4x", "next"),
    23: ("if_state_le", "w4x", "next"),
    24: ("switch_held_dir", "w4j w8j wCj", "jump"),
    25: ("history_mark", "b1", "next"),
    26: ("buffer", "h4 h6 w8j", "call"),
    27: ("if_bit", "b1 w4r", "next"),
    28: ("if_not_bit", "b1 w4r", "next"),
    29: ("if_opp_bit", "b1 w4r", "next"),
    30: ("if_opp_not_bit", "b1 w4r", "next"),
    31: ("load", "b1 w4r", "next"),
    32: ("store", "b1 w4r", "next"),
    33: ("load_opp", "b1 w4r", "next"),
    34: ("store_opp", "b1 w4r", "next"),
    35: ("if_distance_le", "w4f", "next"),
    36: ("if_distance_ge", "w4f", "next"),
    37: ("if_rob62", "b1", "next"),
    38: ("cancel_move", "w4m", "next"),
    39: ("if_state_bit", "b1", "next"),
    40: ("if_not_state_bit", "b1", "next"),
    41: ("jump_state_any", "w4x w8j", "branch"),
    42: ("jump_state_none", "w4x w8j", "branch"),
    43: ("if_cond_ge", "b1c h2", "next"),
    44: ("jump_cond_ge", "b1c h2 w4j", "branch"),
    45: ("move_restart", "w4m", "end"),
    46: ("move_masked", "w4m", "end"),
    47: ("set_resume", "w4j", "call"),
    48: ("if_free", "", "next"),
    49: ("set_rob338", "w4x", "end"),
    50: ("if_entry_current", "", "next"),
    51: ("if_rate_ge", "b1", "next"),
    52: ("state_set", "w4x", "next"),
    53: ("state_add", "w4x", "next"),
    54: ("if_wall_dist_le", "b1 w4f", "next"),
    55: ("if_wall_dist_ge", "b1 w4f", "next"),
    56: ("if_opp_wall_dist_le", "b1 w4f", "next"),
    57: ("if_opp_wall_dist_ge", "b1 w4f", "next"),
    58: ("nop", "", "next"),
    59: ("move_or_hold", "b1 h2 w4m w8m wCj", "end"),
    60: ("move_if_side", "w4m", "next"),
    61: ("move_if_other_side", "w4m", "next"),
    62: ("move_if_opponent", "b1 w4m", "next"),
    63: ("escape", "", "next"),
    64: ("hold", "", "next"),
    65: ("jump_cond_le", "b1c h2 w4j", "branch"),
    66: ("if_frame_flag", "b1", "next"),
    67: ("if_not_frame_flag", "b1", "next"),
    68: ("jump_move_eq", "h2 w4j", "branch"),
    69: ("raw_bit", "b1n", "next"),
    70: ("input_toward", "", "next"),
    71: ("clear_all", "", "next"),
    72: ("input_reset", "", "next"),
    73: ("set_rob1e8", "w4x w8j", "end"),
    74: ("clear_branches", "", "next"),
    75: ("if_card_flag", "b1", "next"),
    76: ("if_not_card_flag", "b1", "next"),
    77: ("move_if_card_flag", "b1 w4m w8j", "next"),
    78: ("wait_frame_resume", "b1 h2 w4r", "next"),
    79: ("if_cond_le", "b1c h2", "next"),
}
SIZE = {}  # op -> instruction size, from main.bin

# Input bits (the sampler 0x0C05C6C0 builds them each frame from the pad):
# 1 G, 2 P, 4 K, 8 E (a fourth button) - the order of the button-combination
# counters (0x0C29F678: P, K, G, P+K, G+P, G+K, all) and the commands
# confirm it (Akira's Tetsuzanko is 4 6 6 P+K) -; 0x10 / 0x20 the two raw
# horizontal directions, 0x40 down, 0x80 up; then bits the sampler adds:
# 0x100 forward, 0x200 back (0x10 / 0x20 swapped by the facing, rob +0x18
# bit 5), 0x400 / 0x800 up / down by the fighter's side flag (rob +0x318
# bit 3) and 0x1000 / 0x2000 by the opponent's.
NUMPAD = {0: "5", 0x80: "8", 0x40: "2", 0x100: "6", 0x200: "4", 0x180: "9", 0x280: "7", 0x140: "3", 0x240: "1"}
BUTTONS = ((1, "G"), (2, "P"), (4, "K"), (8, "E"))
EXTRA = ((0x10, "r0"), (0x20, "r1"), (0x400, "8s"), (0x800, "2s"), (0x1000, "8o"), (0x2000, "2o"))
CONFIGURED = {8: 0x100, 9: 0x200, 10: 0x400, 11: 0x800, 12: 0x1000, 13: 0x2000}  # bit numbers 8-13 name these
# the condition table (0x0C29F94C, script_cond / move_branch_cond): a counter
# of the history entry held at least / most N frames
CONDITIONS = ["dir", "P", "K", "G", "P+K", "G+P", "G+K", "G+P+K", "8", "2", "4", "6", "9", "7", "3", "1"]


def notation(m):
    """An input mask in numpad notation: direction (5 = neutral), then buttons."""
    d = m & 0x3C0
    parts = [NUMPAD[d]] if d in NUMPAD else [f"dir{d:#x}"]
    parts += [n for b, n in EXTRA if m & b]
    parts += [n for b, n in BUTTONS if m & b]
    if parts[0] == "5" and len(parts) > 1:
        parts = parts[1:]
    if m & ~0x3FFF:
        parts.append(f"{m & ~0x3FFF:#x}")
    return "+".join(parts)


def bit_name(b):
    return notation(CONFIGURED[b]) if b in CONFIGURED else notation(1 << b) if 0 <= b < 14 else f"bit{b}"
BASE = 0x0C020000
OPTABLE = 0x0C29F6CC


def load_sizes(prog):
    for k in range(0x50):
        SIZE[k] = struct.unpack_from("<I", prog, OPTABLE + 8 * k + 4 - BASE)[0]


def decode(d, pc, names):
    """One instruction at pc: (text, size, flow, targets)."""
    op = d[pc]
    if op >= 0x50:
        return f".byte {op:#04x}  ; not an opcode: the interpreter stops", 1, "end", []
    name, fmt, flow = OPS[op]
    size = SIZE[op]
    b1 = struct.unpack_from("b", d, pc + 1)[0]
    h2 = struct.unpack_from("<H", d, pc + 2)[0]
    h4, h6 = struct.unpack_from("<hH", d, pc + 4) if size >= 8 else (0, 0)
    words = {k: struct.unpack_from("<I", d, pc + off)[0] for k, off in (("w4", 4), ("w8", 8), ("wC", 12)) if off + 4 <= size}
    out, targets = [], []
    for spec in fmt.split():
        k, kind = spec[:2], spec[2:]
        v = {"b1": b1, "h2": h2, "h4": h4, "h6": h6}.get(k, words.get(k))
        if kind == "j":
            targets.append(v)
            out.append(f"L_{v:04x}")
        elif kind == "m":
            mid = v & 0x7FFF
            out.append(f"{mid}" + (f"<{names[mid]}>" if mid in names else "") + (f"|{v >> 16:#x}" if v >> 16 else ""))
        elif kind == "r":
            out.append(f"rob+{v:#x}")
        elif kind == "f":
            out.append(f"{struct.unpack('<f', struct.pack('<I', v))[0]:g}")
        elif kind == "x":
            out.append(f"{v:#x}")
        elif kind == "n":
            out.append((bit_name(v) if k == "b1" else notation(v)) + (f"({v:#x})" if k != "b1" else ""))
        elif kind == "s":
            out.append(f"{v:#x}" + (f"({notation(v & 0xF)})" if 0 < v & 0xF and not v & ~0x400000F else ""))
        elif kind == "c":
            out.append(CONDITIONS[v] if 0 <= v < len(CONDITIONS) else f"cond{v}")
        else:
            out.append(str(v))
    return f"{name} {', '.join(out)}".rstrip(), size, flow, targets


def listing(d, entries, names):
    """Disassemble everything reachable from entries {offset: label}."""
    seen, todo, lines, labels = set(), list(entries), {}, dict(entries)
    while todo:
        pc = todo.pop()
        while pc not in seen and 0 <= pc < len(d):
            seen.add(pc)
            text, size, flow, targets = decode(d, pc, names)
            lines[pc] = (text, size)
            for t in targets:
                if t and t < len(d):
                    labels.setdefault(t, f"L_{t:04x}")
                    todo.append(t)
            if flow in ("end", "jump"):
                break
            pc += size
    out = []
    for pc in sorted(lines):
        if pc in labels:
            out.append(f"{labels[pc]}:" if labels[pc].startswith("L_") else f"{labels[pc]}:  ; L_{pc:04x}")
        out.append(f"  {pc:04x}  {lines[pc][0]}")
    return out


def commands(d, entries, names, limit=20000):
    """Move -> the command inputs that reach it, from every path through the
    scripts at entries: the input tests read backward (the script matches
    the newest input first), neutral dropped, plus the buttons the state
    register held (state = the buttons just pressed). follow-ups are the
    scripts a move leaves for its next input (continue_with): their commands
    are listed as "after"."""
    out, seen, todo, steps = {}, set(), [(pc, None, (), None) for pc in entries], 0
    while todo and steps < limit:
        pc, state, toks, after = todo.pop()
        while 0 <= pc < len(d) and steps < limit:
            steps += 1
            key = (pc, state, toks[-8:], after)
            if key in seen:
                break
            seen.add(key)
            op = d[pc]
            if op >= 0x50:
                break
            size = SIZE[op]
            b1 = struct.unpack_from("b", d, pc + 1)[0]
            h2 = struct.unpack_from("<H", d, pc + 2)[0]
            w = [struct.unpack_from("<I", d, pc + o)[0] for o in (4, 8, 12) if o + 4 <= size]
            name, fmt, flow = OPS[op]
            if op in (4, 8):
                toks = toks + (notation(h2),)
            elif op in (6, 9):
                toks = toks + (bit_name(b1),)
            elif op == 20:
                state = w[0]
            if name.startswith("move") or op == 38:
                mv = w[0] & 0x7FFF if w else None
                seq = [t for t in reversed(toks) if t != "5"]
                if state is not None and state & 0xF and not (seq and any(c in seq[-1] for c in "GPKE")):
                    seq.append(notation(state & 0xF))
                if mv is not None:
                    out.setdefault(mv, set()).add((" ".join(seq) or "-", after or ""))
            if op == 12 or op == 26 or op == 47:  # a follow-up script for the next input
                tgt = w[-1] if op == 26 else w[0]
                if tgt < len(d):
                    todo.append((tgt, None, (), "follow"))
            if op in (10, 11):
                todo.append((w[0], state, toks, after))
            elif op == 21:
                todo.append((w[1], w[0], toks, after))
            elif op in (41, 42):
                todo.append((w[1], state, toks, after))
            elif op in (44, 65):
                todo.append((w[0], state, toks, after))
            elif op == 68:
                todo.append((w[0], state, toks, after))
            elif op == 24:  # the held direction of the history entry: forward, back, neither
                for t in w:  # (the latest input test carries that direction too)
                    todo.append((t, state, toks, after))
                break
            if op == 19:
                pc = w[0]
                continue
            if flow == "end":
                break
            pc += size
    return out


INPUT_PATTERNS = 0x0C174590  # 54 x { value, mask, kind } (input_match)


def input_patterns(prog):
    """The branch input patterns as text: kind 0 none of value's bits in the
    input, 1 the new presses masked equal value, 2 the input masked equal value."""
    out = []
    for k in range(54):
        v, m, kind = struct.unpack_from("<HHH", prog, INPUT_PATTERNS + 6 * k - BASE)
        if k == 0:
            out.append("any")
        elif kind == 0:
            out.append(f"none of {notation(v)}")
        else:
            out.append(("press " if kind == 1 else "input ") + notation(v) + ("" if m in (0xF, 0x3C0) else f" (mask {m:#x})"))
    return out


def control_sets(d):
    u = lambda o: struct.unpack_from("<I", d, o)[0]
    offs = []
    o = 0
    while u(o):
        offs.append(u(o))
        o += 4
    return offs[:-1]  # the last is the end of the sets


def branch_script(d, target, names, limit=64):
    """A branch's script: its instruction pointer list, one instruction each."""
    u = lambda o: struct.unpack_from("<I", d, o)[0]
    out, k = [], 0
    while k < limit:
        pc = u(target + 4 * k)
        if pc == 0:  # a null pc ends the run (move_script_exec)
            out.append("end of list")
            break
        text, size, flow, targets = decode(d, pc, names)
        out.append(f"{pc:04x}  {text}")
        k += 1
        if flow == "end":
            break
    return out


def main():
    src, prog_path, motdir, dest = sys.argv[1:5]
    prog = open(prog_path, "rb").read()
    load_sizes(prog)
    os.makedirs(dest, exist_ok=True)
    names = {}
    for f in os.listdir(motdir):
        if f.startswith("motdat_mask_") and f.endswith(".json"):
            names.update({m["id"]: m["name"] for m in json.load(open(os.path.join(motdir, f)))["motions"]})
    nsets = nins = 0
    allcmds = {}
    for f in sorted(os.listdir(src)):
        if not (f.startswith("ctrl_") and f.endswith(".bin")):
            continue
        d = open(os.path.join(src, f), "rb").read()
        u = lambda o: struct.unpack_from("<I", d, o)[0]
        out = [f"; {f}: move scripts (tools/movescript.py)"]
        for si, s in enumerate(control_sets(d)):
            nsets += 1
            entries = {}
            for k in range(6):
                if u(s + 4 * k) != 0x10:
                    entries.setdefault(u(s + 4 * k), f"set{si}_script{k}")
            table, count = u(s + 0x18), u(s + 0x1C)
            for k in range(count):  # the count can run past the file end (ctrl_AKI: 4, 3 there)
                if table + 4 * k + 4 <= len(d) and u(table + 4 * k):
                    entries.setdefault(u(table + 4 * k), f"set{si}_table{k}")
            out.append(f"\n; ---- set {si} at {s:#x}: scripts {[hex(u(s + 4 * k)) for k in range(6)]}, table {table:#x} x {count}")
            body = listing(d, entries, names)
            nins += sum(1 for line in body if line.startswith("  "))
            out += body
        open(os.path.join(dest, f.replace(".bin", ".txt")), "w").write("\n".join(out) + "\n")
        cmds = {}
        for si, s in enumerate(control_sets(d)):
            got = commands(d, [u(s + 4 * k) for k in range(6) if u(s + 4 * k) != 0x10], names)
            for mv, cs in got.items():
                cmds.setdefault(mv, set()).update((si, c, a) for c, a in cs)
        lines = [f"; {f}: the inputs that reach each move (tools/movescript.py), numpad notation, newest last;",
                 "; set = control set (stance), 'after' = a follow-up input left by the previous move"]
        for mv in sorted(cmds, key=lambda m: names.get(m, "")):
            forms = sorted({(si, c, a) for si, c, a in cmds[mv]})
            text = "; ".join(f"{c}" + (f" [set {si}]" if si else "") + (" (after)" if a else "") for si, c, a in forms[:12])
            lines.append(f"{mv:5} {names.get(mv, '?'):28} {text}" + (f"; +{len(forms) - 12} more" if len(forms) > 12 else ""))
        open(os.path.join(dest, f.replace("ctrl_", "commands_").replace(".bin", ".txt")), "w").write("\n".join(lines) + "\n")
        allcmds[f[5:-4]] = cmds
    nbr = 0
    pats = input_patterns(prog)
    for bank in motion.bank_table(prog):
        if not bank["mothead"]:
            continue
        path = os.path.join(dest, bank["mothead"].replace(".bin", ".json"))
        if not os.path.exists(path):
            continue
        d = open(os.path.join(src, bank["mothead"]), "rb").read()
        data = json.load(open(path))
        for m in data["moves"]:
            for b in m["branches"]:
                b["input_text"] = pats[b["input"]] if 0 <= b["input"] < 54 else None
                b["input2_text"] = pats[b["input2"]] if 0 <= b["input2"] < 54 else None
                b["condition_text"] = (f"{CONDITIONS[b['condition']]} held >= {b['value']} frames"
                                       if 0 < b["condition"] < len(CONDITIONS) else None)
                b["script_code"] = branch_script(d, int(b["script"], 16), names)
                nbr += 1
        json.dump(data, open(path, "w"), indent=1)
        # frame data: every attack of the bank with its command (from the
        # ctrl_ file of the same tag) and the game's own advantage formula
        tag = bank["mothead"][8:-4]
        cmds = allcmds.get(tag, {})
        rows = ["id\tmove\tcommand\tstartup\tactive\trecovery_end\tlevel\tdamage\tguard\thit\tcounter"]
        for m in data["moves"]:
            if "advantage" not in m:
                continue
            fr = next(c for c in m["commands"] if c["name"] == "frames")
            at = next(c for c in m["commands"] if c["name"] == "attack")
            cs = sorted({c for si, c, a in cmds.get(m["id"], ()) if not a and c != "-"}, key=len)
            av = m["advantage"]
            rows.append(f"{m['id']}\t{m['name']}\t{' / '.join(cs[:3])}\t{m['startup']}\t{fr['active_start']}-{fr['active_end']}"
                        f"\t{fr['recovery_end']}\t{at['level_name']}\t{at['damage']}\t{av['guard']:+d}\t{av['hit']:+d}\t{av['counter']:+d}")
        open(os.path.join(dest, f"framedata_{tag}.tsv"), "w").write("\n".join(rows) + "\n")
    print(f"{nsets} control sets, {nins} instructions; {nbr} branch scripts added to {dest}/mothead_*.json", file=sys.stderr)


if __name__ == "__main__":
    main()
