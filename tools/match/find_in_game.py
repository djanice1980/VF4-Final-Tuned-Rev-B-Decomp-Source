"""Compile source files with the game's compiler and find each function in the game.

    python3 -I tools/match/find_in_game.py [options] file.c|file.S ...

For identifying library code by what it compiles to: every function in each
object is searched for in work/main.bin (relocated words wildcarded, trailing
alignment nops dropped), and printed as

    file <tab> function <tab> size <tab> addresses (comma separated, or "-")

One address is an identification; several mean the code is too generic to
say (a one-line wrapper; more than eight are only counted). Options:

    -I DIR        an include directory (repeatable; relative to the repo)
    -D NAME[=V]   a define (repeatable); any other -f/-m/-O flag is passed on
    --range LO-HI only search there (hex), e.g. 0c157ea0-0c15e000

C goes through tools/match/match.py's compile (GCC 3.2.3, -O1 -g -ml
-m4-single-only plus these flags); .S files are preprocessed by the same
compiler and assembled with sh4-elf-as. Nothing found here is copied into
the repository; a library's own sources stay outside it (build/).
"""
import os, re, shutil, subprocess, sys, tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import program
import match

ROOT = match.ROOT
B = match.BASE


def assemble(path, flags, incs, out):
    s = os.path.join(out, "out.s")
    rel = os.path.relpath(os.path.abspath(path), ROOT)
    G = match.GCC
    r = match.run_gcc([f"{G}/xgcc", f"-B{G}/", "-ml", "-m4-single-only", *flags,
                       *[f"-I/repo/{i}" for i in incs], "-E", "-P", "-x", "assembler-with-cpp", f"/repo/{rel}",
                       "-o", "/out/out.s"],
                      [(ROOT, "/repo", False), (os.path.abspath(out), "/out", True)])
    if r.returncode:
        raise RuntimeError(r.stderr)
    o = os.path.join(out, "out.o")
    r = subprocess.run(["sh4-elf-as", "-little", "-o", o, s], capture_output=True, text=True)
    if r.returncode:
        raise RuntimeError(r.stderr)
    return o


def compile_c(path, flags, incs, out):
    f = os.path.join(out, os.path.basename(path))
    line = " ".join(flags + [f"-I@/{i}" for i in incs])
    open(f, "w").write(f"/* match-flags: {line} */\n" + open(path, errors="replace").read())
    try:
        return match.compile_c(f, out)
    except SystemExit as e:
        raise RuntimeError(str(e))


def search(img, code, masked, lo, hi):
    first = next((j for j in range(len(code)) if j not in masked), None)
    if first is None:
        return []
    key, hits, k = code[first:first + 6], [], lo - B
    while True:
        k = img.find(key, k, hi - B)
        if k < 0:
            return hits
        base = k - first
        if base % 2 == 0 and all(j in masked or img[base + j] == code[j] for j in range(len(code))):
            hits.append(B + base)
        k += 1


def main():
    args, flags, incs = sys.argv[1:], [], []
    lo, hi, files = B, B + 0x200000, []
    i = 0
    while i < len(args):
        a = args[i]
        if a in ("-I", "-D"):
            (incs if a == "-I" else flags).append(args[i + 1] if a == "-I" else "-D" + args[i + 1]); i += 2; continue
        if a == "--range":
            lo, hi = (int(x, 16) for x in args[i + 1].split("-")); i += 2; continue
        if a.startswith("-I"):
            incs.append(a[2:])
        elif a.startswith(("-D", "-f", "-m", "-O")):
            flags.append(a)
        else:
            files.append(a)
        i += 1
    if not files:
        sys.exit(__doc__)
    img = open(program.IMAGE, "rb").read()
    hi = min(hi, B + len(img))
    for path in files:
        tmp = tempfile.mkdtemp(dir=os.path.join(ROOT, "build"))
        try:
            o = (assemble if path.endswith(".S") else compile_c)(path, flags, incs, tmp)
        except RuntimeError as e:
            first = next((l for l in str(e).split("\n") if "error" in l.lower() or ": " in l), "").strip()
            print(f"{path}\t(compile failed: {first[:120]})")
            shutil.rmtree(tmp)
            continue
        secs, syms, relocs = match.read_elf(o)
        for si, s in enumerate(secs):
            if s["typ"] != 1 or not (s["flags"] & 4) or not s["size"]:
                continue
            fns = sorted((x for x in syms if x["shn"] == si and x["name"] and x["type"] in (0, 2)
                          and not x["name"].startswith((".", "L"))),  # L...: assembly's local labels
                         key=lambda x: x["value"])
            for fs in fns:
                start = fs["value"]
                size = fs["size"] or (next((g["value"] for g in fns if g["value"] > start), s["size"]) - start)
                code = s["data"][start:start + size]
                while len(code) >= 2 and code[-2:] == b"\x09\x00":  # alignment padding (nop)
                    code = code[:-2]
                masked = set()
                for off, typ, sym, add in relocs.get(si, []):
                    if start <= off < start + len(code):
                        masked.update(range(off - start, off - start + (4 if typ in (1, 2) else 2)))
                hits = search(img, code, masked, lo, hi)
                where = (f"{len(hits)} places" if len(hits) > 8 else ",".join(f"0x{h:08x}" for h in hits)) or "-"
                print(f"{path}\t{fs['name'].lstrip('_')}\t{len(code)}\t{where}")
        shutil.rmtree(tmp)


if __name__ == "__main__":
    main()
