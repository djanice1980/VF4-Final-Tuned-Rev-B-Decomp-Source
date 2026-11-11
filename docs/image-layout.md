# From GD-ROM to SH-4 program

Set: MAME `vf4tuned`, *Virtua Fighter 4: Final Tuned (Ver. B)*, GD-ROM
GDS-0036F, PIC 317-0387-COM. CHD SHA1 `68edece4239e5adfef9df143defb711ff2b4db72`
(matches MAME 0.289).

## GD-ROM (high-density area, track 3, LBA 45000+)

| LBA    | Size        | File          | Role |
|--------|-------------|---------------|------|
| 303232 | 256         | `BHX.1GB`     | same layout as BHX.BIN (presumably for 1 GB DIMM boards) |
| 303233 | 256         | `BHX.BIN`     | boot descriptor named by the PIC; real image name at 0xC0 |
| 303234 | 251,658,240 | `VF41_S.BIN`  | DES-encrypted DIMM image, the game |
| 426114 | 251,658,240 | `VF41_S2.BIN` | decrypts to a byte-identical copy of VF41_S.BIN |
| 548994 | 12,288      | `NAOMIGD.BIN` | DIMM-board side file (not yet examined) |

## PIC (317-0387-COM, 16 KiB)

One byte every second address:
- `0x7C0`/`0x7E0`: boot file name, `BHX.BIN`
- `0x780` (7 bytes) + `0x7A0`: DES key, `c775d05831e0b3d3`

Decryption uses MAME's DES variant (its own bit layout, little-endian
64-bit blocks, key byte-swapped before the key schedule); see
`tools/dimm_decrypt.c`, ported verbatim from MAME `naomigd.cpp`.

## Decrypted image header (cart-style, read by the Naomi 2 BIOS)

| Offset | Value |
|--------|-------|
| 0x000  | `Naomi2` |
| 0x010  | `SEGA ENTERPRISES,LTD.` |
| 0x030  | title per region: JAPAN, USA, EXPORT, KOREA, AUSTRALIA, all `VF4 FINAL TUNED` |
| 0x130  | date 2004-08-01 |
| 0x134  | game ID `BHX` |
| 0x360  | main program load: ROM 0x020000 -> RAM 0x0C020000, 0x340000 bytes |
| 0x3C0  | test-mode load: ROM 0x360000 -> RAM 0x0C020000, 0x080000 bytes |
| 0x420  | entry points: main 0xAC020000, test 0xAC020000 (P2 uncached mirror) |

Everything after 0x3E0000 is a SimpleFlashFS volume (header at 0x800000)
holding the game's 908 asset files; see `docs/assets.md`.

## Program

- `work/main.bin`: 3,407,872 bytes, base 0x0C020000, SHA1 `e1288dda29d526c086086042b760fdc10be4662a`
- `work/test.bin`: 524,288 bytes, base 0x0C020000, SHA1 `29ab3e757ec3b734f7a9ec7d260cebfad9caa0e1`

Absolute pointers in main.bin use the U0/P0 mirror (0x0Cxxxxxx) almost
exclusively, so Ghidra loads it there. Library build dates in strings
reach Oct 19 2004, later than the header date.
