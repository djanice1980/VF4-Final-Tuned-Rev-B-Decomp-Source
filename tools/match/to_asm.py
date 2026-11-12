"""Write a function the game's compiler did not produce (hand-written
assembly) as an assembler source that assembles to its exact bytes.

    python3 -I tools/match/to_asm.py ADDRESS ... [--out DIR] [--force]
    python3 -I tools/match/to_asm.py --assembly [--out DIR]

ADDRESS is a function in src/ (0c10c180); --assembly takes every unmatched
function tools/match/delay_slots.py places in hand-written assembly.
Each region becomes DIR/<address>.s (default source/asm/): the function's
span from its src/ header, with any other function that starts inside it
(an entry part way into the routine) as a global label of its own.

The listing is objdump's, made into source gas reassembles: branch targets
become labels (.L<address>), pc-relative loads name their pool word, and a
pool word holding a game address names it (a function or global by its
src/ name, else FUN_/DAT_<address>), so match.py checks it as a relocation.
Code is what control flow reaches from the entries; the rest of the span
(jump tables, padding, words only reached through a computed jump) is
written as .word / .long. A line gas does not take, or assembles to other
bytes, is written as the .word it is. The result is checked against the
game before it is written; then

    python3 -I tools/match/match.py source/asm/<address>.s

checks it as every source file is checked.
"""
import bisect, os, re, struct, subprocess, sys, tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import program
import match
import auto_match
import delay_slots

BASE = match.BASE
INSN = re.compile(r"^\s*([0-9a-f]+):\s+(?:[0-9a-f]{2} )+\s*\t([\w./]+)\t?([^!]*?)\s*(?:!.*)?$")
DELAYED = {"bra", "bsr", "braf", "bsrf", "jmp", "jsr", "rts", "rte", "bt.s", "bf.s"}
END = {"bra", "braf", "jmp", "rts", "rte"}  # control does not fall past the slot
PCREL = {"mov.l": 4, "mov.w": 2, "mova": 4}


def objdump(blob, at):
    with tempfile.NamedTemporaryFile(suffix=".bin") as f:
        f.write(blob)
        f.flush()
        r = subprocess.run(["sh4-elf-objdump", "-D", "-b", "binary", "-m", "sh4", "-EL",
                            f"--adjust-vma={at:#x}", f.name], capture_output=True, text=True)
    out = {}
    for m in map(INSN.match, r.stdout.split("\n")):
        if m:
            out[int(m.group(1), 16)] = (m.group(2), m.group(3).strip())
    return out


def target(ops):
    m = re.match(r"0x([0-9a-f]+)", ops)
    return int(m.group(1), 16) if m else None


def loaded_word(lst, pc, reg, word_at):
    """The pool word a mov.l a few instructions before pc put in reg, or None."""
    for back in range(2, 14, 2):
        mn, ops = lst.get(pc - back, (None, ""))
        if mn is None or mn in DELAYED:
            return None
        if mn == "mov.l" and ops.endswith("," + reg) and target(ops) is not None:
            return word_at(target(ops))
        if ops.endswith("," + reg):
            return None  # reg set some other way
    return None


def trace(lst, lo, hi, entries, external=lambda a: False, word_at=None):
    """(code addresses, {pool address: width}) reached from entries.

    A walk may start at an address already counted as a delay slot (a branch
    into the slot runs on past it); external(a) is true where another file's
    routine is, and flow there leaves this routine (a tail call or a branch
    into it) instead of being followed; with word_at, a jmp or jsr through a
    register loaded from the pool with an address inside lo..hi is followed."""
    code, pool, walked, work = set(), {}, set(), list(entries)
    while work:
        pc = work.pop()
        while lo <= pc < hi and pc not in walked and pc not in pool and pc in lst and not external(pc):
            walked.add(pc)
            code.add(pc)
            mn, ops = lst[pc]
            t = target(ops)
            if mn in PCREL and t is not None:
                pool[t] = PCREL[mn]
            elif mn in ("bra", "bsr", "bt", "bf", "bt.s", "bf.s") and t is not None:
                work.append(t)
            elif mn in ("jmp", "jsr") and word_at is not None:
                w = loaded_word(lst, pc, ops.strip().lstrip("@"), word_at)
                if w is not None and lo <= w < hi:
                    work.append(w)
            if mn in DELAYED:
                code.add(pc + 2)
                if pc + 2 in lst and lst[pc + 2][0] in PCREL and target(lst[pc + 2][1]) is not None:
                    pool[target(lst[pc + 2][1])] = PCREL[lst[pc + 2][0]]
                if mn in END:
                    break
                pc += 4
            else:
                pc += 2
    return code, pool


def name_table():
    """game address -> name: src/ functions and globals (a name over a
    FUN_/DAT_ placeholder)."""
    out = {}
    for n, a in match.load_symbols().items():
        if a not in out or re.match(r"(FUN|DAT|PTR)_", out[a]):
            out[a] = n
    return out


def word_expr(w, names, img):
    if 0x0C000000 <= w < 0x0D000000:  # main RAM: code, data and the bss past the image
        if w in names:
            return "_" + names[w]
        return f"_DAT_{w:08x}"
    return f"0x{w:08x}"


def render(img, lo, hi, entries, names, raw=()):
    """[(offset, size, text)]: one line per halfword or word; raw holds
    offsets to write as plain data."""
    lst = objdump(img[lo - BASE:hi - BASE], lo)
    code, pool = trace(lst, lo, hi, [a for a, _ in entries])
    labels = set()
    for pc in code:
        mn, ops = lst[pc]
        t = target(ops)
        if t is not None and lo <= t < hi and (mn in PCREL or mn in ("bra", "bsr", "bt", "bf", "bt.s", "bf.s")):
            labels.add(t)
    starts = dict(entries)
    out, pc = [], lo
    while pc < hi:
        pre = []
        if pc in starts:
            pre += ["", f"\t.global\t_{starts[pc]}", f"\t.type\t_{starts[pc]},@function", f"_{starts[pc]}:"]
        if pc in labels:
            pre.append(f".L{pc:08x}:")
        h = struct.unpack_from("<H", img, pc - BASE)[0]
        if pool.get(pc) == 4 and pc % 4 == 0 and pc + 4 <= hi and pc not in raw:
            w = struct.unpack_from("<I", img, pc - BASE)[0]
            out.append((pc, 4, pre, f"\t.long\t{word_expr(w, names, img)}"))
            pc += 4
            continue
        if pc in code:
            mn, ops = lst[pc]
            t = target(ops)
            if t is not None:
                if lo <= t < hi:
                    sym = f".L{t:08x}"
                elif t in names:
                    sym = "_" + names[t]
                else:
                    sym = f"_FUN_{t:08x}"
                ops = re.sub(r"^0x[0-9a-f]+", sym, ops)
            text = f"\t{mn}\t{ops}".rstrip()
            if pc in raw:  # gas does not take it as written (a branch out of the file)
                text = f"\t.word\t0x{h:04x}\t! {mn} {ops}".rstrip()
        elif h == 0x0009:
            text = "\tnop"
        else:
            text = f"\t.word\t0x{h:04x}"
        out.append((pc, 2, pre, text))
        pc += 2
    return out


def source(lines, lo, hi, entries, img, note):
    pad = lo % 4
    s = [f"! {note}", "! Generated by tools/match/to_asm.py from the game's bytes: hand-written",
         "! assembly (not the game's compiler's output); not Sega's original text.", "",
         "\t.text", "\t.balign\t4"]
    if pad:  # keep the game's address modulo 4: pc-relative loads depend on it
        s.append(f"\t.word\t0x{struct.unpack_from('<H', img, lo - pad - BASE)[0]:04x}\t! before the routine")
    where = {}
    for off, size, pre, text in lines:
        s.extend(pre)
        where[len(s)] = (off, size)
        s.append(text)
    for a, n in entries:
        end = next((b for b, _ in sorted(entries) if b > a), hi)
        s.append(f"\t.size\t_{n},0x{end - a:x}")
    return "\n".join(s) + "\n", where


def assemble(text, tmp):
    p = os.path.join(tmp, "a.s")
    open(p, "w").write(text)
    o = os.path.join(tmp, "a.o")
    r = subprocess.run(["sh4-elf-as", "-little", "-o", o, p], capture_output=True, text=True)
    return o, r


def compare(o, img, lo, hi, names_rev):
    """Offsets (game addresses) whose bytes differ, relocations applied."""
    secs, syms, relocs = match.read_elf(o)
    ti = next(i for i, s in enumerate(secs) if s["name"] == ".text")
    data = bytearray(secs[ti]["data"])
    pad = lo % 4
    skip = set()
    for off, typ, sym, add in relocs.get(ti, []):
        if typ == 1:
            add += struct.unpack_from("<I", data, off)[0]
            if sym["type"] == 3 or not sym["name"]:
                v = lo - pad + add
            else:
                v = match.address_of(sym["name"], names_rev)
            if v is None:
                skip.update(range(off, off + 4)); continue
            struct.pack_into("<I", data, off, (v) & 0xFFFFFFFF)
        else:
            skip.update(range(off, off + 2))
    bad = set()
    for i in range(hi - lo):
        if i + pad in skip:
            continue
        if i + pad >= len(data) or data[i + pad] != img[lo - BASE + i]:
            bad.add(lo + (i & ~1))
    return bad


def build(img, lo, hi, entries, names, note):
    names_rev = match.load_symbols()
    raw = set()
    with tempfile.TemporaryDirectory() as tmp:
        for _ in range(50):
            lines = render(img, lo, hi, entries, names, raw)
            text, where = source(lines, lo, hi, entries, img, note)
            o, r = assemble(text, tmp)
            if r.returncode:
                errs = {int(m.group(1)) for m in re.finditer(r"a\.s:(\d+): Error", r.stderr)}
                hit = {where[n - 1] for n in errs if n - 1 in where}
                if not hit or {o for o, _ in hit} <= raw:
                    raise SystemExit(f"{lo:08x}: gas: {r.stderr}")
                raw |= {o + k for o, n in hit for k in range(0, n, 2)}
                continue
            bad = compare(o, img, lo, hi, names_rev)
            if not bad:
                return text, len(raw)
            fix = {(o, n) for o, n, *_ in lines if any(o <= b < o + n for b in bad)}
            if {o for o, _ in fix} <= raw:
                raise SystemExit(f"{lo:08x}: still differs at {[hex(b) for b in sorted(bad)[:4]]}")
            raw |= {o + k for o, n in fix for k in range(0, n, 2)}
    raise SystemExit(f"{lo:08x}: did not settle")


def region(img, a, cands, matched):
    """(end, [(address, name)]) of the routine at a: its span, grown over
    every function that starts inside it (an entry part way in) and over
    the pool words and branch targets it reaches past its end (hand-written
    routines share a pool placed after the last of them)."""
    hi = a + cands[a][0]
    starts = sorted(set(matched) | set(cands))

    def external(t):
        """t is in another file's routine: the nearest function start at or
        before it is a matched one past this routine's start."""
        i = bisect.bisect_right(starts, t) - 1
        return i >= 0 and starts[i] > a and starts[i] in matched

    def word_at(p):
        return struct.unpack_from("<I", img, p - BASE)[0] if BASE <= p < BASE + len(img) - 3 else None

    for _ in range(100):
        inside = [b for b in sorted(cands) if a <= b < hi]
        hi = max([hi] + [b + cands[b][0] for b in inside])
        lst = objdump(img[a - BASE:hi + 0x400 - BASE], a)
        code, pool = trace(lst, a, hi + 0x400, inside, external, word_at)
        far = [p + w for p, w in pool.items() if p + w > hi and not external(p)]  # another file's pool stays there
        # a branch target at or past the end: the instruction there is in the routine
        far += [t + 2 for pc in code if pc < hi and lst[pc][0] in ("bra", "bt", "bf", "bt.s", "bf.s")
                for t in [target(lst[pc][1])] if t is not None and hi <= t < hi + 0x400 and not external(t)]
        code_past = [pc for pc in code if pc >= hi]
        new = max(far + [pc + 2 for pc in code_past] + [hi])
        if new == hi:
            return hi, [(b, cands[b][1]) for b in inside]
        clash = [b for b in matched if hi <= b < new]
        if clash:
            raise SystemExit(f"{a:08x}: reaches {new:08x}, past matched function {min(clash):08x}")
        hi = new
    raise SystemExit(f"{a:08x}: region did not settle")


def main():
    args = sys.argv[1:]
    out = os.path.join(program.SOURCE, "asm")
    if "--out" in args:
        i = args.index("--out")
        out = args[i + 1]
        del args[i:i + 2]
    force = "--force" in args
    img = open(program.IMAGE, "rb").read()
    cands = {a: (n, name) for a, n, _, name, _ in auto_match.candidates()}
    if "--assembly" in args:
        kind = auto_match.kind_of(delay_slots.groups())
        want = sorted(a for a in cands if kind(a) == "assembly")
    else:
        want = sorted(int(a, 16) for a in args if not a.startswith("-"))
    names = name_table()
    matched = set(match.load_symbols().values()) - set(cands)
    os.makedirs(out, exist_ok=True)
    done = set()
    for a in want:
        if a in done:
            continue
        if a not in cands:
            print(f"{a:08x}: not an unmatched function in src/"); continue
        try:
            hi, entries = region(img, a, cands, matched)
        except SystemExit as e:
            print(e); continue
        done.update(b for b, _ in entries)
        path = os.path.join(out, f"{a:08x}.s")
        if os.path.exists(path) and not force:
            print(f"{a:08x}: {path} exists (--force rewrites it)"); continue
        try:
            text, nraw = build(img, a, hi, entries, names, f"0x{a:08x}  size 0x{hi - a:x}  " + ", ".join(e[1] for e in entries))
        except SystemExit as e:
            print(e); continue
        open(path, "w").write(text)
        print(f"{a:08x}: {os.path.relpath(path, match.ROOT)} ({hi - a} bytes, {len(entries)} entr{'y' if len(entries) == 1 else 'ies'}, {nraw} halfwords as data)")


if __name__ == "__main__":
    main()
