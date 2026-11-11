"""Read a Flycast save state (vf4tuned.state) and find the game's main RAM
in it, so the game's variables can be read at the moment the state was
saved.

    python3 -I tools/flycast_state.py STATE work/main.bin [--dump OUT]

The file is "FLYSAVE1", 8 bytes, a 4-byte field, the PNG thumbnail's size
and the PNG, then the machine state in RetroArch's RZIP container
("#RZIPv\\x01#", the chunk size, the uncompressed size, then zlib chunks
each preceded by its compressed size). The 32 MB of main RAM
(0x0C000000) is found in the state by the game's own code: main.bin loads
at 0x0C020000, so its first bytes sit 0x20000 into RAM. --dump writes the
RAM to OUT."""
import struct, sys, zlib

RAM_BASE = 0x0C000000
RAM_SIZE = 0x2000000
LOAD = 0x0C020000


def unpack(path):
    d = open(path, "rb").read()
    if d[:8] != b"FLYSAVE1":
        raise SystemExit("not a Flycast save state")
    png = struct.unpack_from("<I", d, 0x14)[0]
    o = 0x18 + png
    if d[o:o + 8] != b"#RZIPv\x01#":
        raise SystemExit("no RZIP stream after the thumbnail")
    chunk, total = struct.unpack_from("<IQ", d, o + 8)
    o += 20
    out = bytearray()
    while len(out) < total:
        n = struct.unpack_from("<I", d, o)[0]
        out += zlib.decompress(d[o + 4:o + 4 + n])
        o += 4 + n
    return bytes(out)


def find_ram(state, code):
    """offset of main RAM in the state: where the game's code sits at
    0x20000 into it"""
    probe = code[0x100:0x140]
    k = state.find(probe)
    while k >= 0:
        base = k - 0x100 - (LOAD - RAM_BASE)
        if base >= 0 and state[base + 0x20000:base + 0x20000 + 0x1000] == code[:0x1000]:
            return base
        k = state.find(probe, k + 1)
    return None


def ram(path, image):
    s = unpack(path)
    base = find_ram(s, open(image, "rb").read())
    if base is None:
        raise SystemExit("main RAM not found in the state")
    return s[base:base + RAM_SIZE]


if __name__ == "__main__":
    s = unpack(sys.argv[1])
    code = open(sys.argv[2], "rb").read()
    base = find_ram(s, code)
    print("state %d bytes, main RAM at offset %s" % (len(s), hex(base) if base is not None else "not found"))
    if base is not None:
        r = s[base:base + RAM_SIZE]
        same = sum(1 for i in range(0, len(code) - 3, 4) if r[0x20000 + i:0x20004 + i] == code[i:i + 4])
        print("code words equal to main.bin: %d of %d" % (same, len(code) // 4))
        if "--dump" in sys.argv:
            open(sys.argv[sys.argv.index("--dump") + 1], "wb").write(r)
