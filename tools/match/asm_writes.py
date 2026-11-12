"""Which scratch registers each hand-written routine writes, read from its
listing in source/asm/, against what ghidra/call_effects.tsv measured over
the body Ghidra has (and ghidra/call_effects_overrides.tsv).

    python3 -I tools/match/asm_writes.py [-v]

From each entry (.global label) the code is walked as it flows: on past
conditional branches and into delay slots, to branch targets, through calls
(a callee adds its own writes: a hand-written one's from call_effects.tsv,
a compiled one all of them), and ending at rts, rte or a jmp. A tail branch
or a jmp to a known routine adds that routine's writes. Scratch registers
are r1-r7 and fr1-fr11 (r0 and fr0 carry results and are left out, as in
call_effects.tsv).

The FPU state is followed along each path: under fschg (pair moves) an
fmov's even frN is the pair drN and an odd one the other bank's pair; after
frchg the text's registers are the caller's back bank, not scratch. A
register pushed before the routine writes it counts as saved.

Prints every routine whose listing writes a scratch register the table says
it keeps (exit status 1 when there is one): the table then tells callers
that a value survives the call when it does not; the fix is a row in
ghidra/call_effects_overrides.tsv. Routines whose override row is narrower
than the walk on purpose (a register stepped and stepped back, a hook's
known targets) are listed apart.

    python3 -I tools/match/asm_writes.py --show NAME...   the walk's list for each
"""
import os, re, sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
SCRATCH = ["r%d" % i for i in range(1, 8)] + ["fr%d" % i for i in range(1, 12)]
NO_WRITE = {"cmp/eq", "cmp/hs", "cmp/ge", "cmp/hi", "cmp/gt", "cmp/pz", "cmp/pl", "cmp/str",
            "tst", "tst.b", "fcmp/eq", "fcmp/gt", "div0s", "pref", "ocbi", "ocbp", "ocbwb",
            "jmp", "jsr", "bra", "bsr", "bt", "bf", "bt/s", "bf/s", "bt.s", "bf.s", "braf", "bsrf",
            "rts", "rte", "nop", "clrmac", "clrt", "sett", "clrs", "sets", "sleep", "frchg",
            "fschg", "trapa", "tas.b", "ldtlb", "movca.l"}
COND = {"bt", "bf", "bt/s", "bf/s", "bt.s", "bf.s"}
DELAYED = {"bra", "bsr", "jmp", "jsr", "rts", "rte", "braf", "bsrf", "bt/s", "bf/s", "bt.s", "bf.s"}


def regs_of(op):
    """Registers an operand names (fr / dr / fv expanded to single registers)."""
    out = []
    m = re.fullmatch(r"(r\d+|fr\d+)", op)
    if m:
        return [op]
    m = re.fullmatch(r"dr(\d+)", op)
    if m:
        n = int(m.group(1))
        return ["fr%d" % n, "fr%d" % (n + 1)]
    m = re.fullmatch(r"fv(\d+)", op)
    if m:
        n = int(m.group(1))
        return ["fr%d" % (n + k) for k in range(4)]
    return out


def dest(op, ops):
    """Registers the instruction writes through its last operand."""
    if op in NO_WRITE or op.startswith("ld") or not ops:
        return []
    if op == "fipr":  # fipr fvM,fvN writes only the last register of fvN
        n = int(re.sub(r"\D", "", ops[-1]))
        return ["fr%d" % (n + 3)]
    return regs_of(ops[-1])


def split_ops(s):
    ops, depth, cur = [], 0, ""
    for ch in s:
        if ch == "(":
            depth += 1
        elif ch == ")":
            depth -= 1
        if ch == "," and depth == 0:
            ops.append(cur.strip())
            cur = ""
        else:
            cur += ch
    if cur.strip():
        ops.append(cur.strip())
    return ops


def parse(path):
    """Instructions in order and label -> index; pool words name their symbol."""
    ins, labels, pool = [], {}, {}
    last_label = None
    for raw in open(path):
        line = raw.split("!")[0].rstrip()
        if not line.strip():
            continue
        m = re.match(r"^([.\w$]+):\s*(.*)$", line)
        if m:
            labels[m.group(1)] = len(ins)
            last_label = m.group(1)
            line = m.group(2)
            if not line.strip():
                continue
        t = line.strip()
        if t.startswith("."):
            m = re.match(r"\.long\s+([_A-Za-z][\w.$]*)", t)
            if m and last_label:
                pool[last_label] = m.group(1).lstrip("_")
            if re.match(r"\.(long|word|short|byte|2byte|4byte)\b", t):
                ins.append(("<data>", []))
            continue
        parts = t.split(None, 1)
        ins.append((parts[0].lower(), split_ops(parts[1]) if len(parts) > 1 else []))
    return ins, labels, pool


def load_table():
    table = {}
    for name in ("call_effects.tsv",):
        for line in open(os.path.join(ROOT, "ghidra", name)):
            f = line.rstrip("\n").split("\t")
            if f[0] == "function":
                continue
            table[f[1]] = (f[0], set() if f[3] in ("-", "") else set(f[3].split(",")))
    over = {}
    for line in open(os.path.join(ROOT, "ghidra", "call_effects_overrides.tsv")):
        f = line.rstrip("\n").split("\t")
        if f[0] == "function":
            continue
        over[f[1]] = set(SCRATCH) if f[2] == "all" else (set() if f[2] == "-" else set(f[2].split(",")))
    return table, over


def main():
    verbose = "-v" in sys.argv
    table, over = load_table()
    files = sorted(f for f in os.listdir(os.path.join(ROOT, "source", "asm")) if f.endswith(".s"))
    parsed, entries = {}, {}
    for f in files:
        p = os.path.join(ROOT, "source", "asm", f)
        ins, labels, pool = parse(p)
        parsed[f] = (ins, labels, pool)
        for line in open(p):
            m = re.match(r"\s*\.global\s+_?([\w$]+)", line)
            if m:
                entries[m.group(1)] = f

    def known(name):
        """What the table (with overrides) says a routine writes."""
        if name in over:
            return over[name]
        if name in table:
            return table[name][1]
        return set(SCRATCH)  # compiled code: the default convention writes all of them

    entry_at = {}
    for e, f in entries.items():
        labels = parsed[f][1]
        idx = labels.get("_" + e, labels.get(e))
        if idx is not None:
            entry_at[(f, idx)] = e
    memo = {}

    def callee_writes(callee, stack):
        """A hand-written callee is walked itself; anything else is taken from the table."""
        if callee in entries and callee not in stack:
            return walk(callee, stack)[0]
        return known(callee)

    def walk(name, stack=()):
        if name in memo:
            return memo[name]
        stack = stack + (name,)
        f = entries[name]
        ins, labels, pool = parsed[f]
        start = labels.get("_" + name, labels.get(name))
        writes, saved, seen = set(), set(), set()

        def regs_in(op, o, sz, fr):
            """The caller-visible registers an operand names, given the FPU state
            on this path: sz = pair moves (fschg), fr = banks swapped (frchg)."""
            m = re.fullmatch(r"(?:fr|dr|xd|fv)(\d+)", o)
            if not m:
                return regs_of(o)
            n = int(m.group(1))
            kind = o[:2]
            if kind == "fr" and sz and op == "fmov":
                kind, n = ("dr", n) if n % 2 == 0 else ("xd", n - 1)
            if kind == "xd":  # the other bank's pair
                return [] if not fr else ["fr%d" % n, "fr%d" % (n + 1)]
            if fr:  # the text's fr / dr / fv name the caller's back bank
                return []
            return regs_of("%s%d" % (kind, n))

        def effect(op, ops, sz, fr):
            if len(ops) == 2 and ops[1] == "@-r15" and op in ("mov.l", "fmov", "fmov.s"):
                for x in regs_in(op, ops[0], sz, fr):
                    if x not in writes:
                        saved.add(x)  # pushed before the routine changed it: restored later
                return
            if len(ops) == 2 and ops[0] == "@r15+":
                for x in regs_in(op, ops[1], sz, fr):
                    if x not in saved:
                        writes.add(x)
                return
            for o in ops:
                m = re.fullmatch(r"@-(r\d+)|@(r\d+)\+", o)
                if m and (m.group(1) or m.group(2)) not in ("r15",) + tuple(saved):
                    writes.add(m.group(1) or m.group(2))
            if op in NO_WRITE or op.startswith("ld") or not ops:
                return
            if op == "fipr":
                got = regs_in(op, ops[-1], sz, fr)
                got = got[3:4]
            else:
                got = regs_in(op, ops[-1], sz, fr)
            for x in got:
                if x not in saved:
                    writes.add(x)

        def resolve(op, ops, i):
            """The routine a call or jump goes to, by name, or None."""
            if op in ("bsr", "bra"):
                t = ops[0] if ops else ""
                idx = labels.get(t)
                if idx is not None:
                    return entry_at.get((f, idx)), idx
                return t.lstrip("_"), None
            reg = ops[0].lstrip("@") if ops else ""
            for j in range(i - 1, max(i - 12, -1), -1):
                pj, oj = ins[j]
                if pj == "mov.l" and len(oj) == 2 and oj[1] == reg and oj[0] in pool:
                    return pool[oj[0]], None
            return None, None

        todo = [(start, 0, 0)]
        while todo:
            i, sz, fr = todo.pop()
            while i is not None and i < len(ins) and (i, sz, fr) not in seen:
                seen.add((i, sz, fr))
                op, ops = ins[i]
                if op == "<data>":
                    break
                effect(op, ops, sz, fr)
                if op == "fschg":
                    sz ^= 1
                if op == "frchg":
                    fr ^= 1
                nxt = i + 1

                def slot():
                    if i + 1 < len(ins) and ins[i + 1][0] != "<data>":
                        effect(ins[i + 1][0], ins[i + 1][1], sz, fr)

                if op in COND and ops and ops[0] in labels:
                    todo.append((labels[ops[0]], sz, fr))
                elif op == "braf":
                    stop = min([v for (ff, v) in entry_at if ff == f and v > i] or [len(ins)])
                    todo.extend((v, sz, fr) for v in labels.values() if i < v < stop)
                    slot()
                    nxt = None
                elif op in ("bsr", "jsr", "bsrf"):
                    callee, idx = resolve(op, ops, i) if op != "bsrf" else (None, None)
                    if callee and callee != name:
                        writes.update(callee_writes(callee, stack) - saved)
                    elif idx is not None:
                        todo.append((idx, sz, fr))  # a local subroutine: walked as part of this one
                    elif callee != name:
                        writes.update(set(SCRATCH) - saved)
                elif op in ("bra", "jmp"):
                    callee, idx = resolve(op, ops, i)
                    if callee and callee != name and (op == "jmp" or idx is None or entry_at.get((f, idx))):
                        writes.update(callee_writes(callee, stack) - saved)  # a tail call
                    elif idx is not None:
                        todo.append((idx, sz, fr))
                    elif op == "jmp":
                        writes.update(set(SCRATCH) - saved)
                    slot()
                    nxt = None
                elif op in ("rts", "rte"):
                    slot()
                    nxt = None
                i = nxt
        memo[name] = ({w for w in writes if w in SCRATCH}, False)
        return memo[name]

    if "--show" in sys.argv:
        for name in sys.argv[sys.argv.index("--show") + 1:]:
            got = walk(name)[0]
            print("%s\t%s" % (name, ",".join(sorted(got, key=SCRATCH.index)) or "-"))
        return

    short, judged = [], []
    for name in sorted(entries, key=lambda n: table.get(n, ("~",))[0]):
        if name not in table:
            continue
        missing = sorted(walk(name)[0] - known(name), key=SCRATCH.index)
        if not missing:
            continue
        # an override row is a reading by hand (a register stepped and stepped
        # back, a hook's known targets); _start never returns
        (judged if name in over or name == "_start" else short).append((table[name][0], name, missing))
    print("routines whose listing writes registers the table says they keep (%d):" % len(short))
    for a, n, m in short:
        print("  %s %-36s also writes %s" % (a, n, ",".join(m)))
    print("narrower by a reading by hand (call_effects_overrides.tsv) or never returning (%d):" % len(judged))
    for a, n, m in judged:
        print("  %s %-36s the walk also finds %s" % (a, n, ",".join(m)))
    sys.exit(1 if short else 0)


if __name__ == "__main__":
    main()
