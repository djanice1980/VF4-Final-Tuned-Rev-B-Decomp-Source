"""Compile reconstructed C with the game's compiler and compare each function with the game.

    python3 -I tools/match/match.py [file.c | file.s ...] [-v]

VF4FT_INC=<dir> compiles against a copy of src/include instead (a long run,
such as permute.py, then survives an export regenerating src/).
VF4FT_GCC=<version> compiles with another GCC built in build/toolchain/work.
VF4FT_EXTRA_FLAGS="-mspace ..." adds flags to every compile (for trying one
on a whole run).

With no files it checks every file in source/, the reconstructed C (and the
hand-written assembly, source/asm/*.s, which is only assembled): each
function there must match (run it before committing).

The compiler is GCC 3.2.3 with the game's flags (-O1 -g -ml -m4-single-only, plus any a
file names in a "match-flags:" comment near its top), built by
tools/toolchain/build-gcc.sh into build/toolchain/work. It is a 32-bit Linux
program: where this machine can run it (32-bit glibc installed: lib32-glibc on Arch,
libc6:i386 on Debian/Ubuntu) it runs directly, otherwise in the vf4ft-gcc-build image
(VF4FT_DOCKER=1 forces the image). Both give the same output; starting a container
per compile costs about 0.7 s against 0.1 s for the compile itself. The output is assembled with the host's sh4-elf-as. Each function in the object
is found in work/main.bin by its name (the function headers of the exported src/, or the address in a
FUN_0cxxxxxx name) and compared byte for byte. Every relocated word is checked too,
not masked: a call or a global must hold that symbol's game address (src/,
src/include/vf4ft_globals.h, or the address in FUN_ / DAT_ / PTR_..._ names), and a
reference to the object's own read-only data (a string literal) must point at the same
bytes in the game. Exit status 0 when every function matches.

On a DIFF, a function whose delay slots disagree with the file's flags is
pointed out (the game fills them unless built with -fno-delayed-branch;
tools/match/delay_slots.py --groups maps which code is built which way).

Results per function:
    match      every byte, every relocation verified
    match*     every byte; some relocations could not be checked (listed with -v)
    DIFF       first differing offset, with both sides disassembled
"""
import os, re, struct, subprocess, sys, tempfile
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import program

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
BASE = 0x0C020000
# -g since 2026-11-10 (the maintainer's ruling: the whole tree is built with -g). It
# changes no instruction in 6,630 of the 6,631 functions; sprite.c's
# vram_used_total_update matches as plain C only with it. Every tool that
# compiles C takes its flags from this list (try.py, rtl_dump.py,
# find_in_game.py, local_match.py, permute.py through compile_c).
FLAGS = ["-ml", "-m4-single-only", "-O1", "-g"]
# VF4FT_GCC=3.2.2 (or another version built by tools/toolchain/build-gcc.sh
# into build/toolchain/work) compiles with that GCC instead, for comparing
GCC = f"/work/obj-{os.environ.get('VF4FT_GCC', '3.2.3')}/gcc"
TOOLCHAIN = os.path.join(ROOT, "build", "toolchain", "work")
_direct = None


def direct():
    """True when the compiler runs on this machine without the container."""
    global _direct
    if _direct is None:
        _direct = False
        if os.environ.get("VF4FT_DOCKER") != "1":
            try:
                r = subprocess.run([TOOLCHAIN + GCC[len("/work"):] + "/xgcc", "--version"], capture_output=True)
                _direct = r.returncode == 0
            except OSError:  # not built here, or no 32-bit loader
                pass
    return _direct


def run_gcc(argv, binds):
    """Run the compiler. argv names paths as the container sees them;
    binds: [(host dir, container dir, writable)] (build/toolchain/work is
    always /work). Run directly, each container path becomes its host path."""
    binds = [(TOOLCHAIN, "/work", False)] + binds
    if direct():
        by_len = sorted(binds, key=lambda b: -len(b[1]))

        def host(a):
            for pre in ("-I", "-B", ""):
                for h, t, _ in by_len:
                    if a == pre + t or a.startswith(pre + t + "/"):
                        return pre + h + a[len(pre + t):]
            return a
        return subprocess.run([host(a) for a in argv], capture_output=True, text=True)
    # --mount, not -v: -v creates a missing directory (as root), which once
    # recreated src/include under a running export step
    mounts = [x for h, t, w in binds
              for x in ("--mount", f"type=bind,source={h},target={t}" + ("" if w else ",readonly"))]
    return subprocess.run(["docker", "run", "--rm", "--user", f"{os.getuid()}:{os.getgid()}", *mounts,
                           "vf4ft-gcc-build", *argv], capture_output=True, text=True)


def load_symbols():
    """Current names: every function header in the exported src/ (address, then its
    signature line), .s files' too (test.bin's libgcc helpers sit in
    libgcc/*.s, so their callers' relocations could not be checked), and
    every global in src/include/vf4ft_globals.h."""
    syms = {}
    # a comment ends at its own */ (not a later one), and the signature may
    # put its return type on a line of its own (undefined4\nFUN_0c15d220(...));
    # a header whose name was matched past the next header gave that next
    # function's name this address (isalpha was checked at FUN_0c15d220)
    head = re.compile(r"^/\* 0x(0c[0-9a-f]{6})  size (?:(?!\*/).)*\*/\n(?:/\*(?:(?!\*/).)*\*/\n)*\n?"
                      r"([^(;{}/]*?)\b([A-Za-z_]\w*)\s*\(", re.M | re.S)
    for dirpath, _, files in os.walk(program.SRC):
        for f in files:
            if f.endswith((".c", ".s")):
                for m in head.finditer(open(os.path.join(dirpath, f), encoding="utf-8", errors="replace").read()):
                    syms.setdefault(m.group(3), int(m.group(1), 16))
    g = os.path.join(program.SRC, "include", "vf4ft_globals.h")
    if os.path.exists(g):
        for m in re.finditer(r"^extern [^;]*?\b([A-Za-z_]\w*)\s*(?:\[[^\]]*\])*\s*;\s*/\*\s*0x([0-9a-f]+)\s*\*/", open(g).read(), re.M):
            syms.setdefault(m.group(1), int(m.group(2), 16))
    return syms


def game_slots():
    """{function address: (branches, filled delay slots)} in the game, from
    work/delay_slots.tsv (the export step); empty when it is missing."""
    p = program.SLOTS
    out = {}
    if os.path.exists(p):
        for line in open(p).read().split("\n")[1:]:
            if line:
                _, f, _, slot = line.split("\t")
                n, k = out.get(int(f, 16), (0, 0))
                out[int(f, 16)] = (n + 1, k + (slot != "nop"))
    return out


def c_name(name):
    """The C name of an object-file symbol: the toolchain prefixes one "_"
    (only one: newlib's __ieee754_sqrtf is ___ieee754_sqrtf in the object)."""
    return name[1:] if name.startswith("_") else name


def address_of(name, syms):
    n = c_name(name)
    if n in syms:
        return syms[n]
    m = re.search(r"(?:^|_)([0-9a-f]{8})$", n)  # main RAM, its mirrors, hardware registers
    return int(m.group(1), 16) if m else None


def file_flags(path):
    """Extra flags a file asks for, in a comment line "match-flags: ..." near its top
    (the game's own code and the libraries were not all built the same way)."""
    for line in open(path, encoding="utf-8", errors="replace").read().split("\n")[:20]:
        m = re.search(r"match-flags:\s*(.*?)\s*(\*/)?$", line)
        if m:
            return m.group(1).split()
    return []


def compile_c(path, outdir):
    """An "-I@/dir" in match-flags is dir in this repository (mounted at /repo),
    searched before the stub headers: a C library's own headers must win.
    A .s file (hand-written assembly, tools/match/to_asm.py) is only assembled."""
    src = os.path.abspath(path)
    outdir = os.path.abspath(outdir)
    if src.endswith(".s"):
        o = os.path.join(outdir, "out.o")
        r = subprocess.run(["sh4-elf-as", "-little", "-o", o, src], capture_output=True, text=True)
        if r.returncode:
            sys.exit(f"assemble failed: {path}\n{r.stderr}")
        return o
    extra = file_flags(path)
    incs = [f.replace("-I@/", "-I/repo/", 1) for f in extra if f.startswith("-I@/")]
    extra = [f for f in extra if not f.startswith("-I@/")]
    s = os.path.join(outdir, "out.s")
    r = run_gcc([f"{GCC}/xgcc", f"-B{GCC}/", "-nostdinc", *incs, "-I/stub", "-I/inc",
                 "-isystem", f"{GCC}/include", *FLAGS, *extra, *os.environ.get("VF4FT_EXTRA_FLAGS", "").split(), "-S", f"/src/{os.path.basename(src)}", "-o", "/out/out.s"],
                [(os.path.dirname(src), "/src", False),
                 (os.path.join(ROOT, "tools", "toolchain", "stub"), "/stub", False),
                 (os.environ.get("VF4FT_INC") or os.path.join(program.SRC, "include"), "/inc", False),
                 (outdir, "/out", True), (ROOT, "/repo", False)])
    if r.returncode:
        sys.exit(f"compile failed: {path}\n{r.stderr}")
    o = os.path.join(outdir, "out.o")
    r = subprocess.run(["sh4-elf-as", "-little", "-o", o, s], capture_output=True, text=True)
    if r.returncode:
        sys.exit(f"assemble failed: {path}\n{r.stderr}")
    return o


def read_elf(path):
    o = open(path, "rb").read()
    shoff, = struct.unpack_from("<I", o, 0x20)
    shentsize, shnum, shstrndx = struct.unpack_from("<HHH", o, 0x2E)
    keys = ("nm", "typ", "flags", "addr", "off", "size", "link", "info", "align", "entsize")
    secs = [dict(zip(keys, struct.unpack_from("<10I", o, shoff + i * shentsize))) for i in range(shnum)]
    ss = secs[shstrndx]
    cstr = lambda off: o[off:o.index(b"\0", off)].decode()
    for s in secs:
        s["name"] = cstr(ss["off"] + s["nm"])
        s["data"] = o[s["off"]:s["off"] + s["size"]] if s["typ"] != 8 else b""
    syms = []
    for s in secs:
        if s["typ"] == 2:
            st = secs[s["link"]]
            for k in range(s["size"] // 16):
                n, v, sz, info, oth, shn = struct.unpack_from("<IIIBBH", o, s["off"] + 16 * k)
                syms.append(dict(name=cstr(st["off"] + n), value=v, size=sz, type=info & 15, shn=shn))
    relocs = {}
    for s in secs:
        if s["typ"] == 4:  # RELA
            symtab = s["link"]
            lst = relocs.setdefault(s["info"], [])
            for k in range(s["size"] // 12):
                off, info, addend = struct.unpack_from("<IIi", o, s["off"] + 12 * k)
                lst.append((off, info & 0xFF, syms[info >> 8], addend))
    return secs, syms, relocs


def disasm(blob, at, n=24):
    with tempfile.NamedTemporaryFile(suffix=".bin") as f:
        f.write(blob); f.flush()
        r = subprocess.run(["sh4-elf-objdump", "-D", "-b", "binary", "-m", "sh4", "-EL",
                            f"--adjust-vma={at:#x}", f.name], capture_output=True, text=True)
    lines = [l for l in r.stdout.split("\n") if re.match(r"\s+[0-9a-f]+:", l)]
    return lines[:n]


DATA_AT = set()  # addresses of the data objects (not functions) run_checks has compared


def run_checks(paths, img, symtab, slots, verbose=False, say=print):
    """Compile each file and compare its functions with the game. Returns
    (functions, matching, [(name, address, size, "match" | "match*" | "DIFF"
    | "?")]); a compile error raises SystemExit with the compiler's message."""
    total = good = 0
    results = []
    for path in paths:
        with tempfile.TemporaryDirectory() as tmp:
            secs, syms, relocs = read_elf(compile_c(path, tmp))
            for si, s in enumerate(secs):
                if s["typ"] != 1 or not (s["flags"] & 4) or not s["size"]:
                    continue
                # functions, and in a .s file the data its authors kept in .text
                # (a table, a constant, a code template: .type name,@object);
                # data is checked the same way but is not counted as a function
                kinds = (2, 1) if path.endswith(".s") else (2,)
                for fs in sorted((x for x in syms if x["shn"] == si and x["type"] in kinds and x["size"]), key=lambda x: x["value"]):
                    data = fs["type"] == 1
                    total += not data
                    name = c_name(fs["name"])
                    at = address_of(fs["name"], symtab)
                    lo, size = fs["value"], fs["size"]
                    if data and at is not None:
                        DATA_AT.add(at)
                    if at is None:
                        results.append((name, None, 0, "?")); say(f"  ?      {name}: no game address for this name"); continue
                    code = s["data"][lo:lo + size]
                    game = img[at - BASE:at - BASE + size]
                    masked = set(); unchecked = []; bad = []
                    for off, typ, sym, add in relocs.get(si, []):
                        if not lo <= off < lo + size:
                            continue
                        width = 4 if typ in (1, 2) else 2
                        masked.update(range(off - lo, off - lo + width))
                        word = struct.unpack_from("<I", game, off - lo)[0] if width == 4 else None
                        target = sym["name"] if sym["name"] else secs[sym["shn"]]["name"]
                        if typ != 1:
                            unchecked.append(f"+{off - lo:#x} type {typ} -> {target}"); continue
                        # sh-elf gas keeps the addend in the relocated word itself (the RELA
                        # field is 0 for these), so read it from the object
                        add += struct.unpack_from("<I", code, off - lo)[0]
                        if sym["type"] == 3 or not sym["name"]:  # section-relative: our own data
                            sec = secs[sym["shn"]]
                            want = sec["data"][add:add + 64]
                            if sec["typ"] == 8:
                                unchecked.append(f"+{off - lo:#x} -> {sec['name']}+{add:#x} (bss)"); continue
                            want = want.split(b"\0")[0] + b"\0" if b"\0" in want else want[:16]
                            have = img[word - BASE:word - BASE + len(want)] if BASE <= word < BASE + len(img) else b""
                            if have != want:
                                bad.append(f"+{off - lo:#x}: {sec['name']}+{add:#x} {want[:24]!r} but the game's word {word:#010x} holds {have[:24]!r}")
                            continue
                        dest = address_of(sym["name"], symtab)
                        if dest is None:
                            unchecked.append(f"+{off - lo:#x} -> {sym['name']} (no game address)"); continue
                        # 32 bits: a negative addend (&DAT_X - 8) wraps
                        if word != (dest + add) & 0xFFFFFFFF:
                            bad.append(f"+{off - lo:#x}: {c_name(sym['name'])}{add:+#x} = {(dest + add) & 0xFFFFFFFF:#010x}, game has {word:#010x}")
                    diff = next((j for j in range(size) if j not in masked and code[j] != game[j]), None)
                    if diff is None and not bad:
                        good += not data
                        results.append((name, at, size, "match" if not unchecked else "match*"))
                        say(f"  {'match ' if not unchecked else 'match*'} {name} ({size} bytes at {at:#010x}{', data' if data else ''})")
                        if verbose:
                            for u in unchecked: say(f"           unchecked {u}")
                        continue
                    results.append((name, at, size, "DIFF"))
                    if data:
                        total += 1  # a wrong data object fails the run like a wrong function
                    say(f"  DIFF   {name} ({size} bytes at {at:#010x})" + (f": first byte differs at +{diff:#x}" if diff is not None else ""))
                    n, k = slots.get(at, (0, 0))
                    nodelay = "-fno-delayed-branch" in file_flags(path)
                    if n and (k == 0) != nodelay and (k > 0 or n >= 2):
                        # a hint, not a verdict: a library function can leave every slot empty
                        say(f"           the game fills {k} of its {n} delay slots; this file is built "
                              + ("with" if nodelay else "without") + " -fno-delayed-branch")
                    for b in bad: say(f"           relocation {b}")
                    if diff is not None:
                        start = max(0, (diff & ~1) - 8)
                        ours = disasm(code[start:start + 24], at + start)
                        theirs = disasm(game[start:start + 24], at + start)
                        for x, y in zip(ours, theirs):
                            say(f"           ours {x.strip():44s} game {y.strip()}")
    return total, good, results


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("-")]
    verbose = "-v" in sys.argv
    whole = not args and "VF4FT_EXTRA_FLAGS" not in os.environ and "VF4FT_GCC" not in os.environ
    if not args:
        args = sorted(os.path.join(d, f) for d, _, fs in os.walk(program.SOURCE)
                      for f in fs if f.endswith((".c", ".s")))
    if not args:
        sys.exit(__doc__)
    img = open(program.IMAGE, "rb").read()
    symtab = load_symbols()
    slots = game_slots()
    total, good, results = run_checks(args, img, symtab, slots, verbose)
    if whole:
        # what the source reproduces, byte range by byte range (a function's
        # code with its literal pool and tables): ghidra/TextLayout.java and
        # tools/progress.py measure the image against it
        out = os.path.join(program.ROOT, "ghidra", "matched_extents.tsv" if program.NAME == "main" else "matched_extents_test.tsv")
        with open(out, "w") as w:
            w.write("address\tbytes\tname\tkind\n")
            for name, at, size, how in sorted(results, key=lambda r: r[1]):
                if how != "DIFF":
                    w.write(f"{at:08x}\t{size}\t{name}\t{'data' if at in DATA_AT else 'function'}\n")
    print(f"{good} of {total} functions match")
    sys.exit(0 if good == total else 1)


if __name__ == "__main__":
    main()
