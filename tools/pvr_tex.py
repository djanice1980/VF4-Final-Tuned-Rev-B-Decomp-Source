#!/usr/bin/env python3
"""Decode the obj_/tex_ texture sets to PNG.

Format (measured; see docs/asset-formats.md):
  obj_<set>.bin  header word 2 = offset of the texture descriptor table:
                 16-byte records { short w, h; u32 format; u32 offset; u32 ? }
                 until the first record that is not a texture
  tex_<set>.bin  the texture data; offset is relative to the file; textures
                 are packed at 32-byte alignment
  format         PowerVR2 / Kamui, as tune_texture.c (0x0C0EAC40) reads it:
                 bits 0-7 pixel format: 0 ARGB1555, 1 RGB565, 2 ARGB4444, 4 bump
                 bits 8-11 type: 1 twiddled, 2 twiddled + mipmaps, 3 VQ,
                 4 VQ + mipmaps, 9 rectangle, 13 twiddled rectangle
  Twiddled = Morton order with y in the low bit. VQ = a 2 KiB codebook of 256
  2x2-texel entries, then one index byte per 2x2 block (twiddled). Mipmaps
  are stored smallest first: the top level of a 2^k texture starts at texel
  MIP[k] (twiddled) or index byte VQ_MIP[k] (VQ); both tables measured on this data.

usage: pvr_tex.py <unpacked asset dir> <out dir> [set ...]
       (sets are names like stg_djo; default: every obj_/tex_ pair)
"""
import os
import struct
import sys

import numpy as np
from PIL import Image

POW = (8, 16, 32, 64, 128, 256, 512, 1024)
# Top-level start (in texels) of a 2^k twiddled mipmap chain. Measured: with
# the often-quoted table (3, 4, 8, 0x18, ...) 19 of the 20 twiddled-mipmap
# textures show a grid of wrong texels; 2 texels earlier is smoothest for all
# 19 (the 20th is flat). The VQ table below was confirmed the same way
# (452 of 457 smoothest at it; the rest near-flat).
MIP = [0x1, 0x2, 0x6, 0x16, 0x56, 0x156, 0x556, 0x1556, 0x5556, 0x15556, 0x55556]
VQ_MIP = [0x0, 0x1, 0x2, 0x6, 0x16, 0x56, 0x156, 0x556, 0x1556, 0x5556, 0x15556]
PIXEL = {0: "argb1555", 1: "rgb565", 2: "argb4444", 4: "bump"}
TYPE = {0x100: "tw", 0x200: "twmm", 0x300: "vq", 0x400: "vqmm", 0x900: "rect", 0xD00: "twrect"}

_twiddle_cache = {}


def twiddle_map(w, h):
    """Array [h, w] of the twiddled index of each texel (y bit first)."""
    key = (w, h)
    if key in _twiddle_cache:
        return _twiddle_cache[key]
    ys, xs = np.mgrid[0:h, 0:w]
    idx = np.zeros((h, w), dtype=np.int64)
    sh = 0
    xsz, ysz = w >> 1, h >> 1
    x, y = xs.copy(), ys.copy()
    while xsz or ysz:
        if ysz:
            idx |= (y & 1) << sh
            y >>= 1
            ysz >>= 1
            sh += 1
        if xsz:
            idx |= (x & 1) << sh
            x >>= 1
            xsz >>= 1
            sh += 1
    _twiddle_cache[key] = idx
    return idx


def to_rgba(px, pixel):
    """uint16 array -> uint8 [.., 4] RGBA."""
    px = px.astype(np.uint32)
    if pixel == 0:
        a = np.where(px & 0x8000, 255, 0)
        r, g, b = (px >> 10) & 31, (px >> 5) & 31, px & 31
        r, g, b = (r * 255 + 15) // 31, (g * 255 + 15) // 31, (b * 255 + 15) // 31
    elif pixel == 1:
        a = np.full(px.shape, 255)
        r, g, b = (px >> 11) & 31, (px >> 5) & 63, px & 31
        r, g, b = (r * 255 + 15) // 31, (g * 255 + 31) // 63, (b * 255 + 15) // 31
    elif pixel == 2:
        a, r, g, b = (px >> 12) & 15, (px >> 8) & 15, (px >> 4) & 15, px & 15
        a, r, g, b = a * 17, r * 17, g * 17, b * 17
    else:  # bump: show the 16-bit value as two 8-bit channels
        a = np.full(px.shape, 255)
        r, g, b = px >> 8, px & 255, np.zeros(px.shape, dtype=np.uint32)
    return np.stack([r, g, b, a], axis=-1).astype(np.uint8)


def decode(tex, w, h, fmt, off):
    pixel, ttype = fmt & 0xFF, fmt & 0xF00
    k = w.bit_length() - 1
    if ttype in (0x100, 0x200, 0xD00):
        start = off + (MIP[k] * 2 if ttype == 0x200 else 0)
        px = np.frombuffer(tex, dtype="<u2", count=w * h, offset=start)
        img = px[twiddle_map(w, h)]
    elif ttype == 0x900:
        img = np.frombuffer(tex, dtype="<u2", count=w * h, offset=off).reshape(h, w)
    elif ttype in (0x300, 0x400):
        book = np.frombuffer(tex, dtype="<u2", count=1024, offset=off).reshape(256, 4)
        start = off + 0x800 + (VQ_MIP[k] if ttype == 0x400 else 0)
        idx = np.frombuffer(tex, dtype=np.uint8, count=(w // 2) * (h // 2), offset=start)
        blocks = idx[twiddle_map(w // 2, h // 2)]          # [h/2, w/2]
        quad = book[blocks]                                 # [h/2, w/2, 4]
        img = np.empty((h, w), dtype=np.uint16)
        # entry texel k: x = k >> 1, y = k & 1 (twiddled order inside the 2x2)
        img[0::2, 0::2] = quad[:, :, 0]
        img[1::2, 0::2] = quad[:, :, 1]
        img[0::2, 1::2] = quad[:, :, 2]
        img[1::2, 1::2] = quad[:, :, 3]
    else:
        raise ValueError(f"texture type {ttype:#x}")
    # Rows come out bottom-up: the texture's first row is the image's bottom
    # row. Evidence: the NYC helipad's painted "AM2 R&D" and the city skyline
    # read upright only after a vertical flip (not a mirror), and the sprite
    # cells (spr_tex.py) cover 98-100% of the visible pixels of a flipped page
    # against 33-76% unflipped. Flipping here makes every PNG upright, with
    # the cells' stored v measured from the top.
    return to_rgba(img[::-1], pixel)


def descriptors(obj):
    d = open(obj, "rb").read()
    if len(d) < 16:
        return []
    o = struct.unpack_from("<I", d, 8)[0]
    out = []
    while o + 16 <= len(d):
        w, h, fmt, off, x = struct.unpack_from("<hhIII", d, o)
        if w not in POW or h not in POW or fmt >> 16 or (fmt & 0xFF) not in PIXEL or (fmt & 0xF00) not in TYPE:
            break
        out.append((w, h, fmt, off, x))
        o += 16
    return out


def main():
    src, dest = sys.argv[1], sys.argv[2]
    sets = sys.argv[3:] or sorted(n[4:-4] for n in os.listdir(src)
                                  if n.startswith("obj_") and n.endswith(".bin"))
    total = errors = 0
    for s in sets:
        obj, texf = os.path.join(src, f"obj_{s}.bin"), os.path.join(src, f"tex_{s}.bin")
        if not os.path.exists(texf):
            continue
        tex = open(texf, "rb").read()
        descs = descriptors(obj)
        if not descs:
            continue
        os.makedirs(os.path.join(dest, s), exist_ok=True)
        for i, (w, h, fmt, off, x) in enumerate(descs):
            name = f"{i:03d}_{w}x{h}_{PIXEL[fmt & 0xFF]}_{TYPE[fmt & 0xF00]}.png"
            try:
                Image.fromarray(decode(tex, w, h, fmt, off), "RGBA").save(os.path.join(dest, s, name))
                total += 1
            except (ValueError, IndexError) as e:
                errors += 1
                print(f"{s} #{i}: {e}", file=sys.stderr)
    print(f"{total} textures written to {dest}, {errors} errors", file=sys.stderr)


if __name__ == "__main__":
    main()
