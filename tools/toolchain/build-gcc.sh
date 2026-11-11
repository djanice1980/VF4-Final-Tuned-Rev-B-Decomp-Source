#!/bin/sh
# Build the C compiler proper (cc1 + the xgcc driver) of GCC $1 for sh-elf,
# inside the vf4ft-gcc-build image. /dl holds gcc-core-$1.tar.gz (or .bz2),
# /work is where it builds (obj-$1/gcc/xgcc and cc1 are the result).
# libgcc fails to build afterwards (no SH assembler in the image); it is
# not needed: assemble the compiler's output with the host's sh4-elf-as.
#   docker run --rm --user $(id -u):$(id -g) -v <downloads>:/dl:ro -v <work>:/work \
#     -v "$PWD/tools/toolchain:/tc:ro" vf4ft-gcc-build /tc/build-gcc.sh 3.2.3
V=$1
cd /work
rm -rf gcc-$V obj-$V
case $V in 3.4.*) bzip2 -dc /dl/gcc-core-$V.tar.bz2 | tar xf - ;; *) gzip -dc /dl/gcc-core-$V.tar.gz | tar xf - ;; esac
mkdir obj-$V && cd obj-$V
../gcc-$V/configure --target=sh-elf --prefix=/work/inst-$V --enable-languages=c --disable-nls \
  --without-headers --with-newlib --disable-shared --disable-threads > configure.log 2>&1 || { echo "configure failed"; tail -20 configure.log; exit 1; }
make all-gcc > make.log 2>&1
ls -l gcc/cc1 gcc/xgcc
