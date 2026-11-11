#!/usr/bin/env bash
# Rebuild everything under work/ from your own dump of the MAME "vf4tuned"
# set (Virtua Fighter 4 Final Tuned Ver. B, GDS-0036F + 317-0387-COM PIC).
#
# usage: tools/extract.sh <dir containing gds-0036f.chd and vf4tuned.zip>
#
# needs: chdman (pacman: mame-tools), unzip, python3, a C compiler
set -euo pipefail

src=${1:?usage: tools/extract.sh <dump dir>}
root=$(cd "$(dirname "$0")/.." && pwd)
work=$root/work
mkdir -p "$work/gdrom" "$work/files" "$work/pic" "$root/build"

chd_sha1=68edece4239e5adfef9df143defb711ff2b4db72
got=$(chdman info -i "$src/gds-0036f.chd" | awk '/^SHA1:/ {print $2}')
[[ $got == "$chd_sha1" ]] || { echo "CHD SHA1 $got != MAME $chd_sha1" >&2; exit 1; }

[[ -f $work/gdrom/disc03.bin ]] ||
    chdman extractcd -i "$src/gds-0036f.chd" -o "$work/gdrom/disc.gdi"
unzip -o -q "$src/vf4tuned.zip" -d "$work/pic"

cc -O2 -Wall -o "$root/build/dimm_decrypt" "$root/tools/dimm_decrypt.c"

# The PIC names BHX.BIN; that 256-byte file names the real image at 0xC0.
python3 -I "$root/tools/gdrom.py" "$work/gdrom/disc03.bin" extract VF41_S.BIN "$work/files/VF41_S.BIN"
"$root/build/dimm_decrypt" "$work/pic/317-0387-com.pic" "$work/files/VF41_S.BIN" "$work/vf41_s.dec"

# Naomi header load tables (0x360 main, 0x3C0 test): rom offset, RAM addr, size
dd if="$work/vf41_s.dec" of="$work/main.bin" bs=64K skip=2 count=52 status=none   # 0x020000 -> 0x0C020000, 0x340000
dd if="$work/vf41_s.dec" of="$work/test.bin" bs=64K skip=54 count=8 status=none   # 0x360000 -> 0x0C020000, 0x080000

sha1sum "$work/main.bin" "$work/test.bin"
