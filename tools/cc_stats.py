#!/usr/bin/env python3
"""Measure the calling convention main.bin was compiled with.

Inputs: work/main.dis (sh4-elf-objdump -D of main.bin at 0x0C020000) and
ghidra/functions.tsv (function bounds from ExportFunctions.java).

1. Callee-saved registers. For each register: of the functions that
   write it, how many push it on the stack in the prologue (first 24
   instructions, `...,@-r15`). Callee-saved registers come out near 100%,
   caller-saved near 0%. Pops (`@r15+` into the register) do not count
   as writes.
2. Float argument registers. For each function, the fr4-fr11 registers
   read before being written in the straight-line code from the entry
   (up to the first branch or call). Functions with exactly one such
   register show which register carries the first float argument.

objdump does not know FPSCR.SZ, so a 64-bit pair move prints as its
even single register; that undercounts odd-register writes slightly.

usage: cc_stats.py <main.dis> <functions.tsv>
"""
import collections
import re
import sys

GPR = [f"r{i}" for i in range(16)]
FPR = [f"fr{i}" for i in range(16)]
SPECIAL = ["mach", "macl", "gbr", "fpul", "pr"]
BRANCH = {"bra", "bsr", "bt", "bf", "bt.s", "bf.s", "jmp", "jsr", "rts", "braf", "bsrf", "rte"}


def load_dis(path):
    ins = {}
    for line in open(path):
        f = line.rstrip("\n").split("\t")
        if len(f) < 3 or not f[0].strip().endswith(":"):
            continue
        try:
            a = int(f[0].strip().rstrip(":"), 16)
        except ValueError:
            continue
        mn = f[2].strip()
        ops = f[3].split("!")[0].strip() if len(f) > 3 else ""
        ins[a] = (mn, ops)
    return ins


def regs_in(text):
    return re.findall(r"\b(fr\d+|r\d+|mach|macl|gbr|fpul|pr|fv\d+|xmtrx)\b", text)


def expand(r):
    if r.startswith("fv"):
        n = int(r[2:])
        return [f"fr{n + i}" for i in range(4)]
    return [r]


def effects(mn, ops):
    """(reads, writes) register sets for one instruction."""
    parts = [p.strip() for p in ops.split(",")] if ops else []
    reads, writes = set(), set()
    m = mn.lower()
    if m in ("ftrv",):
        writes |= set(expand(parts[1]))
        reads |= set(expand(parts[1]))
        return reads, writes
    if m == "fipr":
        a, b = expand(parts[0]), expand(parts[1])
        reads |= set(a) | set(b)
        writes.add(b[3])
        return reads, writes
    if m in ("mul.l", "muls.w", "mulu.w", "muls", "mulu"):
        reads |= set(regs_in(ops))
        writes.add("macl")
        return reads, writes
    if m in ("dmuls.l", "dmulu.l"):
        reads |= set(regs_in(ops))
        writes |= {"mach", "macl"}
        return reads, writes
    if m in ("mac.l", "mac.w"):
        for p in parts:
            reads |= set(regs_in(p))
            if p.endswith("+"):
                writes |= set(regs_in(p))
        writes |= {"mach", "macl"}
        reads |= {"mach", "macl"}
        return reads, writes
    if m == "clrmac":
        return reads, {"mach", "macl"}
    if m in BRANCH or m.startswith(("cmp/", "fcmp/")) or m in ("tst", "nop", "fschg", "frchg", "pref", "clrt", "sett"):
        return set(regs_in(ops)), writes
    if not parts:
        return reads, writes
    if len(parts) == 1:
        # one-operand forms (dt, shll, fneg, fabs, fsqrt, fldi0, ...) write it
        rs = regs_in(parts[0])
        if m in ("fldi0", "fldi1"):
            return reads, set(rs)
        return set(rs), set(rs)
    src, dst = ",".join(parts[:-1]), parts[-1]
    for p in parts[:-1]:
        reads |= set(regs_in(p))
        if p.startswith("@") and p.endswith("+"):
            writes |= set(regs_in(p))
    if dst.startswith("@"):
        reads |= set(regs_in(dst))                 # address register
        if dst.startswith("@-"):
            writes |= set(regs_in(dst))
    else:
        d = regs_in(dst)
        writes |= set(d)
        two_input = m in ("add", "addc", "addv", "sub", "subc", "subv", "and", "or", "xor",
                          "fadd", "fsub", "fmul", "fdiv", "fmac", "div1", "shad", "shld",
                          "and.b", "or.b", "xor.b", "rotcl", "rotcr", "negc")
        if two_input:
            reads |= set(d)
    return reads, writes


def main():
    ins = load_dis(sys.argv[1])
    funcs = []
    for line in list(open(sys.argv[2]))[1:]:
        entry, name, ranges = line.rstrip("\n").split("\t")
        addrs = []
        for r in ranges.split(","):
            lo, hi = (int(x, 16) for x in r.split("-"))
            addrs += [a for a in range(lo, hi + 1, 2) if a in ins]
        funcs.append((int(entry, 16), sorted(addrs)))

    all_regs = GPR + FPR + SPECIAL
    wrote = collections.Counter()
    saved = collections.Counter()
    for entry, addrs in funcs:
        w, s = set(), set()
        for k, a in enumerate(addrs):
            mn, ops = ins[a]
            r, wr = effects(mn, ops)
            parts = [p.strip() for p in ops.split(",")] if ops else []
            if parts and parts[0] == "@r15+":
                wr -= set(regs_in(parts[-1]))      # restore pop, not a write
            w |= wr
            if k < 24 and parts and parts[-1] == "@-r15":
                s |= set(regs_in(parts[0]))
        for reg in all_regs:
            if reg in w:
                wrote[reg] += 1
                if reg in s:
                    saved[reg] += 1

    print("1. save-before-write: functions that write REG / of those, push it in the prologue")
    for reg in all_regs:
        if wrote[reg]:
            print(f"   {reg:5s} written in {wrote[reg]:5d}   saved {saved[reg]:5d}   "
                  f"{100 * saved[reg] / wrote[reg]:5.1f}%")

    hist = collections.Counter()
    pairs = collections.Counter()
    for entry, addrs in funcs:
        seen_w, live = set(), []
        for a in addrs:
            if a < entry:
                continue
            mn, ops = ins[a]
            r, wr = effects(mn, ops)
            for reg in sorted(r):
                if reg in FPR[4:12] and reg not in seen_w and reg not in live:
                    live.append(reg)
            seen_w |= wr
            if mn in BRANCH:
                break
        if len(live) == 1:
            hist[live[0]] += 1
        elif len(live) == 2:
            pairs[tuple(sorted(live, key=lambda x: int(x[2:])))] += 1
    print("\n2. live-in float registers (fr4-fr11), functions with exactly one:")
    for reg in FPR[4:12]:
        print(f"   {reg:5s} {hist[reg]:5d}")
    print("   ... with exactly two (most common pairs):", pairs.most_common(5))


if __name__ == "__main__":
    main()
