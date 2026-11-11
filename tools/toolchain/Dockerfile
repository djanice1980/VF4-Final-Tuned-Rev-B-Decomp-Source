# Build environment for the period compilers: Debian 3.1 "sarge" (2005),
# whose own GCC 3.3.5 builds the GCC 3.x sources unchanged.
#   docker build -t vf4ft-gcc-build tools/toolchain
FROM debian/eol:sarge
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --force-yes \
      gcc cpp binutils libc6-dev make bison flex bzip2 patch \
    && apt-get clean
