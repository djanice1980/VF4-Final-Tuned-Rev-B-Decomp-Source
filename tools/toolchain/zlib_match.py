"""Assemble zlib 1.1.4 compiled to assembly (one .s per source file in <dir>) with the host's
sh4-elf-as and compare each function, relocations masked, with the game's zlib (addresses from
ghidra/symbols/zlib.tsv). The compiler test: GCC 3.2.3 -O1 -ml -m4-single-only gives 71 of 71
(the tree is built with -g as well since 2026-11-10; it changes no instruction in zlib).
    python3 -I tools/toolchain/zlib_match.py <dir> work/main.bin ghidra/symbols/zlib.tsv
"""
import os, struct, subprocess, sys, glob
outdir = sys.argv[1]; binpath = sys.argv[2]; tsv = sys.argv[3]
B = 0x0C020000; img = open(binpath, "rb").read()
game = {}
for l in open(tsv).read().split("\n")[1:]:
    c = l.split("\t")
    if len(c) > 2 and c[1] == "function":
        name = c[2].split("(")[0].replace("*", " ").split()[-1]
        game[name] = int(c[0], 16)
def elf_funcs(path):
    o = open(path, "rb").read()
    shoff, = struct.unpack_from("<I", o, 0x20); shentsize, shnum, shstrndx = struct.unpack_from("<HHH", o, 0x2E)
    secs = [dict(zip(("nm","typ","flags","addr","off","size","link","info","align","entsize"), struct.unpack_from("<10I", o, shoff + i * shentsize))) for i in range(shnum)]
    ss = secs[shstrndx]
    for s in secs: s["name"] = o[ss["off"] + s["nm"]:o.index(b"\0", ss["off"] + s["nm"])].decode()
    syms = []
    for s in secs:
        if s["typ"] == 2:
            st = secs[s["link"]]
            for k in range(s["size"] // 16):
                n, v, sz, info, oth, shn = struct.unpack_from("<IIIBBH", o, s["off"] + 16 * k)
                syms.append((o[st["off"] + n:o.index(b"\0", st["off"] + n)].decode(), v, sz, info & 15, shn))
    out = {}
    for si, s in enumerate(secs):
        if s["typ"] != 1 or not (s["flags"] & 4) or not s["size"]: continue
        code = o[s["off"]:s["off"] + s["size"]]; mask = bytearray(b"\x01" * len(code))
        for r in secs:
            if r["typ"] in (4, 9) and r["info"] == si:
                ent = 12 if r["typ"] == 4 else 8
                for k in range(r["size"] // ent):
                    off, info = struct.unpack_from("<II", o, r["off"] + ent * k)
                    for j in range(off, min(off + (4 if (info & 0xFF) in (1, 2) else 2), len(code))): mask[j] = 0
        fs = sorted((v, n, sz) for n, v, sz, typ, shn in syms if shn == si and typ == 2)
        for k, (v, n, sz) in enumerate(fs):
            end = fs[k + 1][0] if k + 1 < len(fs) else len(code)
            out[n[1:] if n.startswith("_") else n] = (code[v:end], bytes(mask[v:end]), sz)
    return out
results = {}; total = same = 0; near = []
for s in sorted(glob.glob(os.path.join(outdir, "*.s"))):
    objdir = os.path.join(os.path.abspath(outdir), "obj")
    os.makedirs(objdir, exist_ok=True)
    o = os.path.join(objdir, os.path.basename(s)[:-2] + ".o")
    r = subprocess.run(["sh4-elf-as", "-little", "-o", o, s], capture_output=True, text=True)
    if r.returncode: print("as failed", s, r.stderr[:300]); continue
    for n, (code, mask, sz) in elf_funcs(o).items():
        if n not in game: continue
        total += 1
        a = game[n] - B
        body = sz if sz else len(code)
        diff = sum(1 for j in range(body) if mask[j] and img[a + j] != code[j])
        if diff == 0: same += 1
        else: near.append((diff, n, body))
print(f"{same} of {total} zlib functions byte-identical")
near.sort(); print("closest misses:", [(n, d, b) for d, n, b in near[:8]])
