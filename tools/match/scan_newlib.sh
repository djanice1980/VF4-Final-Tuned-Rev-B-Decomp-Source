#!/usr/bin/env bash
# Compile a newlib release's C library and float math with the game's compiler
# and find each function in the game's C library region (tools/match/find_in_game.py).
#
#   tools/match/scan_newlib.sh <version>     (newlib extracted in build/newlib/src-<version>)
#
# The defines are newlib's own for sh*-*-* (newlib/configure.host); vfprintf.c is
# built a second time integer-only, as newlib's stdio Makefile does. Output: one
# line per function (file, name, size, where in the game).
set -euo pipefail
cd "$(dirname "$0")/../.."
v=$1; N=build/newlib/src-$v/newlib
[[ -d $N ]] || { echo "no $N (see README: Matching against the game's compiler)" >&2; exit 1; }
F=(--range 0c12dac0-0c15e000 -D HAVE_GETTIMEOFDAY -D WANT_PRINTF_LONG_LONG -I "$N/libc/include")
for d in string stdlib stdio ctype time reent errno misc locale signal; do
  python3 -I tools/match/find_in_game.py "${F[@]}" -I "$N/libc/$d" "$N"/libc/$d/*.c
done
python3 -I tools/match/find_in_game.py "${F[@]}" -I "$N/libc/stdio" -DINTEGER_ONLY -fshort-enums "$N/libc/stdio/vfprintf.c"
python3 -I tools/match/find_in_game.py "${F[@]}" -I "$N/libm/common" "$N"/libm/math/*.c "$N"/libm/common/*.c
python3 -I tools/match/find_in_game.py --range 0c12dac0-0c15e000 -I "$N/libc/machine/sh" "$N"/libc/machine/sh/*.S
