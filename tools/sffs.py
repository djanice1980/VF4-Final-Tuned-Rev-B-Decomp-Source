#!/usr/bin/env python3
"""List and extract the SimpleFlashFS volume in the decrypted DIMM image.

Format (measured on VF41_S.BIN; VF41_S2.BIN is a byte-identical copy):

  volume header, 4 KiB block at 0x800000
    +0x00  "SimpleFlashFS"      +0x20  "1999.5.5"
    +0x30  u32 4                +0x34  u32 0x00800000 (volume start)
    +0x38  u32 0x1000 (block)   +0x3C  u32 0x20020820 (meaning unknown)
  files, back to back from the next block: a 0x100-byte header, then data
    +0x00  bytes from this header to the end of the volume (0x0EFFF000)
    +0x04  space taken: header + data rounded up to the block size;
           the next header starts here
    +0x08  live flag: -1 live, 0 deleted (the game's loader drops entries
           with 0 here; flash deletes by clearing bits)
    +0x0C  data size
    +0x10  u32, meaning unknown (not a 16-bit sum, CRC-32 or Adler-32)
    +0x14  modification time, Unix seconds (2003-10-17 .. 2004-10-26)
    +0x18  st_mode (0100644 / 0100664)
    +0x1C  u32, meaning unknown
    +0x20  -1 filler to +0x7C; +0x7C is -1 on disk, and the game's RAM copy
           of the directory holds the header's image offset there
           (dimm_fopen: data start = +0x7C + 0x100)
    +0x80  file name, NUL-terminated (lookup compares up to 0x80 bytes)
  +0x04 == -1 marks free space: the walk skips +0x00 bytes (to the volume
  end); it stops at a header whose +0x00 is -1 (erased flash). This is the
  walk of the game's own directory loader (0x0C0F2E80), which keeps at most
  1200 entries.

usage: sffs.py list    <image> [manifest.tsv]
       sffs.py extract <image> <out dir>       (stored files, plus unpacked/
                                                 copies of the .gz ones)
"""
import datetime
import gzip
import hashlib
import os
import struct
import sys
import zlib

VOLUME = 0x800000


def volume(img):
    if img[VOLUME:VOLUME + 13] != b"SimpleFlashFS":
        sys.exit("no SimpleFlashFS header at 0x800000")
    start, block = struct.unpack_from("<II", img, VOLUME + 0x34)
    return start, block


def files(img):
    """Walk the volume like the game's directory loader (0x0C0F2E80)."""
    start, block = volume(img)
    off = start + block
    while off + 0x100 <= len(img):
        w = struct.unpack_from("<8I", img, off)
        if w[0] == 0xFFFFFFFF:
            return  # erased flash: end of volume
        if w[1] == 0xFFFFFFFF:
            off += w[0]  # free space: skip to where it ends
            continue
        if (w[6] & 0xF000) != 0x8000:
            sys.exit(f"unexpected header at {off:#x}")
        name = img[off + 0x80:off + 0x100].split(b"\0", 1)[0].decode("ascii")
        yield {"offset": off, "name": name, "remaining": w[0], "taken": w[1], "size": w[3],
               "deleted": w[2] == 0,
               "u10": w[4], "mtime": w[5], "mode": w[6], "u1c": w[7],
               "data": img[off + 0x100:off + 0x100 + w[3]]}
        off += w[1]


def gz_info(data):
    """(original name, unpacked size) for a gzip member, else (None, None)."""
    if data[:2] != b"\x1f\x8b":
        return None, None
    flags = data[3]
    name = None
    if flags & 0x08:
        p = 10 + (2 + struct.unpack_from("<H", data, 10)[0] if flags & 0x04 else 0)
        name = data[p:data.index(b"\0", p)].decode("latin-1")
    try:
        size = len(gzip.decompress(data))
    except (OSError, EOFError, zlib.error):
        size = None
    return name, size


def main():
    if len(sys.argv) < 3 or sys.argv[1] not in ("list", "extract"):
        sys.exit(__doc__)
    img = open(sys.argv[2], "rb").read()
    entries = list(files(img))
    if sys.argv[1] == "list":
        out = open(sys.argv[3], "w") if len(sys.argv) > 3 else sys.stdout
        out.write("offset\tname\tdeleted\tsize\ttaken\tmode\tmtime\tu10\tu1c\tsha1\tgz_name\tgz_size\n")
        for e in entries:
            gname, gsize = gz_info(e["data"])
            out.write(f"{e['offset']:08x}\t{e['name']}\t{int(e['deleted'])}\t{e['size']}\t{e['taken']}\t{e['mode']:o}\t"
                      f"{datetime.datetime.fromtimestamp(e['mtime'], datetime.timezone.utc):%Y-%m-%d %H:%M:%S}\t"
                      f"{e['u10']:08x}\t{e['u1c']:08x}\t{hashlib.sha1(e['data']).hexdigest()}\t"
                      f"{gname or ''}\t{gsize if gsize is not None else ''}\n")
        print(f"{len(entries)} files ({sum(e['deleted'] for e in entries)} deleted), "
              f"{sum(e['size'] for e in entries):,} bytes", file=sys.stderr)
        return
    dest = sys.argv[3]
    unpacked = os.path.join(dest, "unpacked")
    os.makedirs(unpacked, exist_ok=True)
    bad = empty = 0
    for e in entries:
        # deleted entries are kept, under deleted/, since their data is still there
        path = os.path.join(dest, "deleted" if e["deleted"] else "", e["name"])
        os.makedirs(os.path.dirname(path), exist_ok=True)
        with open(path, "wb") as f:
            f.write(e["data"])
        os.utime(path, (e["mtime"], e["mtime"]))
        if not e["data"]:
            empty += 1  # e.g. the three *_scr_test.bin.gz placeholders
            continue
        if e["data"][:2] == b"\x1f\x8b":
            try:
                raw = gzip.decompress(e["data"])
            except (OSError, EOFError, zlib.error):
                bad += 1
                continue
            up = os.path.join(unpacked, e["name"][:-3] if e["name"].endswith(".gz") else e["name"])
            with open(up, "wb") as f:
                f.write(raw)
            os.utime(up, (e["mtime"], e["mtime"]))
    print(f"extracted {len(entries)} files to {dest} ({empty} empty, {bad} gzip errors)", file=sys.stderr)


if __name__ == "__main__":
    main()
