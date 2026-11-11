"""flycast_hitcheck.py HITS.JSONL (from tools/flycast_live.py hits mode): each
logged attack_hit_resolve against the result bits the matched C
(coli_attack.c) gives for the logged defender: counter
(move_flags 0x4000 with state bit 0, or flags2c 0x20), the defender-state
row 0x800 (state bit 2), the damage rows 0x4000 / 0x8000 / 0x10000 (the
defender's own attack_damage against 15 / 25, option bit 0x10000), the
airborne 4 (state 0x40) and the stagger 0x200 (hit_attack_flags). Bits the
log cannot predict (0x10 lethal, 0x2000 relation, 0x20 / 0x40 / 0x80 from
the guard paths) are masked out of both. Prints a table per result."""
import collections
import json
import os
import struct
import sys

prog = open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "work", "main.bin"), "rb").read()
ROWS = [struct.unpack_from("<bb2xIIIf", prog, 0x0C17497C - 0x0C020000 + 0x14 * i) for i in range(3)]
LOW, HIGH = 15, 25  # rob_coli_atk_task_start
MASK = ~(0x10 | 0x2000 | 0x20 | 0x40 | 0x80 | 0x100) & 0xFFFFFFFF


def predict(e):
    d = e["defender"]
    flags = 0x1001
    st, mf, f2c = d["state"], d["move_flags"], d["f2c"]
    why = []
    if (mf & 0x4000 and st & 1) or f2c & 0x20:
        if not st & 4 and not f2c & 0x20:
            flags = (flags & ~0x1000) | 2
            why.append("counter")
        if st & 4 or f2c & 0x20:
            flags = (flags | 0x800) & ~0x1000
            why.append("0x800 (state 4)")
        else:
            dmg = d["attack_damage"]
            if d["f28"] & 0x800000:
                why.append("pending-attack damage (unpredicted)")
                return None, why
            row = 1
            if e["option_bits"] & 0x10000 and HIGH <= dmg:
                row = 2
            if LOW >= dmg:
                row = 0
            flags |= ROWS[row][2]
            why.append("row %d (own damage %d)" % (row, dmg))
        if st & 0x40:
            flags |= 4
    elif st & 0x40:
        flags |= 4
        why.append("airborne")
    if not flags & 4:
        haf = e["hit_attack_flags"]
        if st & 0x20000000:
            if (haf & 0x8000000 and not mf & 0x2000) or (haf & 0x10000000 and mf & 0x2000):
                flags |= 0x200
        elif (haf & 0x2000000 and not mf & 0x2000) or (haf & 0x4000000 and mf & 0x2000):
            flags |= 0x200
    return flags, why


ev = [json.loads(x) for x in open(sys.argv[1])]
tab = collections.Counter()
bad = []
for i, e in enumerate(ev):
    if e["kind"] != "hit_resolve":
        continue
    nxt = next((x for x in ev[i + 1:] if x["kind"] == "recovery_apply"), None)
    if nxt is None:
        continue
    want, why = predict(e)
    got = nxt["result"]
    if want is None:
        tab[("unpredicted", "%#x" % got)] += 1
        continue
    ok = (want & MASK) == (got & MASK)
    tab[(" ".join(why) or "plain", "%#x" % (got & MASK), ok)] += 1
    if not ok:
        bad.append((e, want, got))
for k, n in sorted(tab.items(), key=str):
    print("%4d  %s" % (n, k))
print("%d mismatches" % len(bad))
for e, want, got in bad[:8]:
    d = e["defender"]
    print("  want %#x got %#x: def move %d frame %d state %#x mflags %#x f2c %#x dmg %d" % (
        want, got, d["move"], d["frame"], d["state"], d["move_flags"], d["f2c"], d["attack_damage"]))
