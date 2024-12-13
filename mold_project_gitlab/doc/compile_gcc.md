# good to know

- build out of source
- ensure the sysroot got no absolute soft-links

# prerequists

```bash
apt install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo
```

# build

```bash
#!/bin/bash

set -ex

GCC_VERSION='gcc-9.1.0'
BINUTILS_VERSION='2.32'
INSTALL_PREFIX="$(pwd)/install"
SYSROOT='/home/mlanner/Development/projects/mold/dependencies/image_905d8264/mnt_root/'

mkdir -p "$INSTALL_PREFIX"

curl -Lo gcc.tar.xz "https://ftp.wayne.edu/gnu/gcc/$GCC_VERSION/$GCC_VERSION.tar.xz"
curl -Lo binutils.tar.bz2 "https://ftpmirror.gnu.org/binutils/binutils-$BINUTILS_VERSION.tar.bz2"
tar xf gcc.tar.xz
tar xf binutils.tar.bz2

mkdir build_binutils
cd build_binutils
"../binutils-$BINUTILS_VERSION/configure" --prefix="$INSTALL_PREFIX" --target=arm-linux-gnueabihf --with-arch=armv6 --with-fpu=vfp --with-float=hard --disable-multilib --with-sysroot="$SYSROOT" --enable-gold=yes --enable-lto

make -j 5
make install

export PATH="$INSTALL_PREFIX/bin:$PATH"

cd ..
mkdir build_gcc
cd build_gcc
"../$GCC_VERSION/configure" --prefix="$INSTALL_PREFIX" --target=arm-linux-gnueabihf --enable-languages=c,c++,d --with-arch=armv6 --with-fpu=vfp --with-float=hard --disable-multilib --with-sysroot="$SYSROOT"
# if asan errors or some other error occured you didn't fix the absolute soft links in the sysroot
make -j 5
make install
```

```
configure: error: run `make distclean' and/or `rm ./config.cache' and start over
```
