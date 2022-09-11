# Build using Alpine image
FROM alpine AS build
RUN apk add file make g++ git patch xz

RUN git clone -q https://github.com/richfelker/musl-cross-make.git && \
	sed -i -e 's/xvf/xf/' musl-cross-make/Makefile

RUN mv musl-cross-make/patches/gcc-11.2.0 musl-cross-make/patches/gcc-11.3.0
COPY libstdc++-v3-pic.diff musl-cross-make/patches/gcc-11.3.0/
COPY musl-glibc-symbol-abi.diff musl-cross-make/patches/musl/
RUN ln -s musl musl-cross-make/patches/musl-$(sed -nre 's/^MUSL_VER\s*=\s*([0-9.]+)\s*$/\1/p' musl-cross-make/Makefile)
RUN echo "cf86a48278f9a6f4b03d4390550577b20353b4e9  gcc-11.3.0.tar.xz" > musl-cross-make/hashes/gcc-11.3.0.tar.xz.sha1
RUN echo "15d42de8f15404a4a43a912440cf367f994779d7  binutils-2.38.tar.xz" > musl-cross-make/hashes/binutils-2.38.tar.xz.sha1
RUN echo "0578d48607ec0e272177d175fd1807c30b00fdf2  gmp-6.2.1.tar.xz" > musl-cross-make/hashes/gmp-6.2.1.tar.xz.sha1
RUN echo "2a4919abf445c6eda4e120cd669b8733ce337227  mpc-1.2.1.tar.gz" > musl-cross-make/hashes/mpc-1.2.1.tar.gz.sha1
RUN echo "159c3a58705662bfde4dc93f2617f3660855ead6  mpfr-4.1.0.tar.xz" > musl-cross-make/hashes/mpfr-4.1.0.tar.xz.sha1

ARG TARGET=x86_64-linux-musl
# aarch64[_be]-linux-musl
# arm[eb]-linux-musleabi[hf]
# i*86-linux-musl
# microblaze[el]-linux-musl
# mips-linux-musl
# mips[el]-linux-musl[sf]
# mips64[el]-linux-musl[n32][sf]
# powerpc-linux-musl[sf]
# powerpc64[le]-linux-musl
# riscv64-linux-musl
# s390x-linux-musl
# sh*[eb]-linux-musl[fdpic][sf]
# x86_64-linux-musl[x32]
ARG GCC_VER=11.3.0
# 11.3.0
# 10.3.0
# 9.4.0
# 9.2.0
# 8.5.0
# 7.5.0
# 6.5.0
# 5.3.0
# 4.2.1
ARG BINUTILS_VER=2.38
ARG GMP_VER=6.2.1
ARG MPC_VER=1.2.1
ARG MPFR_VER=4.1.0
ARG LINUX_VER=headers-4.19.88-1

# Build compiler
# - The `-static --static` options are used to build a statically-linked toolchain (https://github.com/richfelker/musl-cross-make/issues/64)
RUN make -C musl-cross-make \
	COMMON_CONFIG='CC="gcc -static --static" CXX="g++ -static --static" CFLAGS="-g0 -O3" CXXFLAGS="-g0 -O3" LDFLAGS="-s" --disable-shared --enable-static' \
	GCC_CONFIG='--enable-default-pie --with-pic' \
	MUSL_CONFIG='CFLAGS="-DNO_GLIBC_ABI_COMPATIBLE"' \
	TARGET=${TARGET} \
	GCC_VER=${GCC_VER} \
	BINUTILS_VER=${BINUTILS_VER} \
	GMP_VER=${GMP_VER} \
	MPC_VER=${MPC_VER} \
	MPFR_VER=${MPFR_VER} \
	LINUX_VER=${LINUX_VER} \
	install && \
	ln -s libc.so musl-cross-make/output/${TARGET}/lib/libc.so.6

RUN ar rc musl-cross-make/output/${TARGET}/lib/libc_dl.a musl-cross-make/build/local/${TARGET}/obj_musl/obj/ldso/*.lo && \
	ranlib musl-cross-make/output/${TARGET}/lib/libc_dl.a


# Install patchelf
ARG PATCHELF_GZ_URI=https://github.com/NixOS/patchelf/releases/download/0.15.0/patchelf-0.15.0-x86_64.tar.gz

RUN wget -O - "$PATCHELF_GZ_URI" | tar xz -C musl-cross-make/output ./bin/patchelf


# Build libuuid
RUN apk add autoconf automake bison gettext-dev libtool pkgconfig

ARG UTIL_LINUX_GZ_URI=https://github.com/karelzak/util-linux/archive/v2.38.tar.gz
RUN wget -O - "$UTIL_LINUX_GZ_URI" | tar xz && cd util-linux-* && ./autogen.sh && \
	./configure --disable-all-programs --enable-libuuid --host ${TARGET} CC=/musl-cross-make/output/bin/${TARGET}-gcc CFLAGS="-g -O2 -fPIC" && make -j && mv libuuid.la .libs/libuuid.a /musl-cross-make/output/${TARGET}/lib/


# Copy toolchain into scratch image
FROM scratch AS deploy
COPY --from=build /musl-cross-make/output/ /
