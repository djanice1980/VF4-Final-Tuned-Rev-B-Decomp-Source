#!/bin/sh
# Compile zlib 1.1.4 (/zlib, the unpacked public source) to assembly with the
# GCC $1 built by build-gcc.sh (/work/obj-$1), into /work/out/$2, with the
# flags that follow; the headers zlib needs from a C library are the minimal
# declarations in tools/toolchain/stub (/tc/stub), given with -I so they come
# before GCC's own include directory (it has a placeholder stdlib.h that
# declares nothing, and an undeclared free changes zcfree). Inside vf4ft-gcc-build:
#   docker run --rm --user $(id -u):$(id -g) -v <work>:/work -v <zlib-1.1.4>:/zlib:ro \
#     -v "$PWD/tools/toolchain:/tc:ro" vf4ft-gcc-build \
#     /tc/compile-zlib.sh 3.2.3 gcc323 -ml -m4-single-only -O1 -g
# (the flags of tools/match/match.py's FLAGS; -g since 2026-11-10, it changes no code here)
V=$1; TAG=$2; shift 2
O=/work/out/$TAG; rm -rf $O; mkdir -p $O
B=/work/obj-$V/gcc
for f in adler32 compress crc32 uncompr deflate trees zutil inflate infblock inftrees infcodes infutil inffast; do
  $B/xgcc -B$B/ -nostdinc -I/tc/stub -isystem $B/include -I/zlib "$@" -S /zlib/$f.c -o $O/$f.s 2> $O/$f.err || echo "compile failed: $f"
done
