#!/usr/bin/env python3
"""Read the ISO9660 filesystem in the high-density area of a GD-ROM dump.

The HD area is track 3 of a chdman-extracted .gdi (2352-byte raw MODE1
sectors starting at LBA 45000). Directory records hold absolute LBAs, so
sector N lives at file offset (N - track_start) * 2352 + 16.

usage: gdrom.py <track03.bin> list
       gdrom.py <track03.bin> extract <ISO path> <out file>
       gdrom.py <track03.bin> extract-all <out dir>
"""
import os
import struct
import sys

SECTOR_RAW = 2352
SECTOR_DATA = 2048
HEADER = 16
TRACK_START = 45000


class Track:
    def __init__(self, path, start=TRACK_START):
        self.f = open(path, "rb")
        self.start = start

    def read(self, lba, length):
        out = bytearray()
        while len(out) < length:
            self.f.seek((lba - self.start) * SECTOR_RAW + HEADER)
            out += self.f.read(SECTOR_DATA)
            lba += 1
        return bytes(out[:length])


def parse_dir(track, lba, size, prefix=""):
    data = track.read(lba, size)
    pos = 0
    while pos < len(data):
        rec_len = data[pos]
        if rec_len == 0:
            # records never straddle a sector; skip to the next one
            pos = (pos // SECTOR_DATA + 1) * SECTOR_DATA
            continue
        rec = data[pos:pos + rec_len]
        ext_lba = struct.unpack_from("<I", rec, 2)[0]
        ext_size = struct.unpack_from("<I", rec, 10)[0]
        flags = rec[25]
        name_len = rec[32]
        name = rec[33:33 + name_len]
        pos += rec_len
        if name in (b"\x00", b"\x01"):
            continue
        name = name.decode("ascii").split(";")[0]
        path = prefix + "/" + name
        if flags & 2:
            yield from parse_dir(track, ext_lba, ext_size, path)
        else:
            yield path, ext_lba, ext_size


def files(track):
    pvd = track.read(TRACK_START + 16, SECTOR_DATA)
    assert pvd[1:6] == b"CD001", "no ISO9660 PVD at LBA 45016"
    root = pvd[156:156 + 34]
    return list(parse_dir(track, struct.unpack_from("<I", root, 2)[0],
                          struct.unpack_from("<I", root, 10)[0]))


def main():
    track = Track(sys.argv[1])
    cmd = sys.argv[2]
    entries = files(track)
    if cmd == "list":
        for path, lba, size in entries:
            print(f"{lba:8d} {size:12d}  {path}")
    elif cmd == "extract":
        want = sys.argv[3].upper().lstrip("/")
        for path, lba, size in entries:
            if path.lstrip("/").upper() == want:
                with open(sys.argv[4], "wb") as out:
                    out.write(track.read(lba, size))
                return
        sys.exit(f"{want} not found")
    elif cmd == "extract-all":
        for path, lba, size in entries:
            dest = os.path.join(sys.argv[3], path.lstrip("/"))
            os.makedirs(os.path.dirname(dest), exist_ok=True)
            with open(dest, "wb") as out:
                out.write(track.read(lba, size))
    else:
        sys.exit(__doc__)


if __name__ == "__main__":
    main()
