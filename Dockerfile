# Build using Alpine image
FROM alpine AS setup
RUN apk add --no-cache file make g++ git linux-headers patch xz

# Need basename() in string.h to build gcc
RUN printf "#if defined(_GNU_SOURCE) && !defined(__cplusplus)\nchar *basename();\n#endif" >> /usr/include/string.h

# (Needed for binutils and libuuid)
RUN apk add --no-cache autoconf automake bison flex gettext-dev gettext-static libtool pkgconfig

ARG MUSL_VER=1.2.5

RUN git clone -q https://github.com/richfelker/musl-cross-make.git && \
	sed -i -e 's/xvf/xf/' -e 's,hashes/%.sha1 |,| hashes/%.sha1,' musl-cross-make/Makefile && \
	sed -i -re 's,([$][(]MAKE[)].*) [$]@,\1 obj_gcc/gcc/.lc_built \&\& find obj_binutils \\( -name "*-new" -o -name "ar" -o -name "nm" -o -name "ranlib" \\) -delete \&\& \1 install-binutils \&\& &,' musl-cross-make/Makefile

COPY --link hashes musl-cross-make/hashes/
COPY --link patches musl-cross-make/patches/
COPY --link sources musl-cross-make/sources/

RUN cd musl-cross-make/patches/ && mkdir -p musl-${MUSL_VER} && cp -f musl/* musl-${MUSL_VER}/


# Build the toolchain
FROM setup AS build

# aarch64[_be]-linux-musl, arm[eb]-linux-musleabi[hf], i*86-linux-musl, microblaze[el]-linux-musl, mips-linux-musl, mips[el]-linux-musl[sf], mips64[el]-linux-musl[n32][sf], powerpc-linux-musl[sf], powerpc64[le]-linux-musl, riscv64-linux-musl, s390x-linux-musl, sh*[eb]-linux-musl[fdpic][sf], x86_64-linux-musl[x32]
ARG TARGET=x86_64-linux-musl
ARG GCC_CONFIG=
# 15.1.0, 14.2.0, 14.1.0, 13.2.0, 12.2.0, 11.3.0
ARG GCC_VER=15.1.0
ARG BINUTILS_VER=2.44
ARG GMP_VER=6.3.0
ARG MPC_VER=1.3.1
ARG MPFR_VER=4.2.2
ARG ISL_VER=0.27
ARG LINUX_VER=headers-4.19.88-1

# Build compiler
# - Use COMMON_CONFIG='CC="gcc -static --static" CXX="g++ -static --static" --disable-shared --enable-static' to build a statically-linked toolchain (https://github.com/richfelker/musl-cross-make/issues/64) to support any Linux distro, but some components requiring shared libraries will not work correctly
RUN make -j4 -C musl-cross-make \
	COMMON_CONFIG='CC="gcc -static --static" CXX="g++ -static --static" AR="gcc-ar" RANLIB="gcc-ranlib" CFLAGS="-D_LARGEFILE64_SOURCE -D__USE_LARGEFILE64 -g0 -O3 -flto -fno-fat-lto-objects" CXXFLAGS="-D_LARGEFILE64_SOURCE -D__USE_LARGEFILE64 -g0 -O3 -flto -fno-fat-lto-objects" LDFLAGS="-s -flto" --disable-shared --enable-static' \
	GCC_CONFIG="--disable-cet --disable-fixed-point --disable-libstdcxx-pch --disable-multilib --disable-nls --disable-symvers \
		--disable-checking --enable-__cxa_atexit --enable-default-pie --enable-default-ssp --enable-threads \
		${GCC_CONFIG} --disable-libssp --disable-libsanitizer --with-linker-hash-style=gnu --with-pic --enable-libstdcxx-backtrace=yes" \
	BINUTILS_CONFIG='--enable-gold' \
	MUSL_CONFIG='CFLAGS="-DNO_GLIBC_ABI_COMPATIBLE"' \
	MUSL_VARS='AR="../obj_binutils/binutils/ar --plugin liblto_plugin.so" RANLIB="../obj_binutils/binutils/ranlib --plugin liblto_plugin.so"' \
	TARGET=${TARGET} \
	GCC_VER=${GCC_VER} \
	MUSL_VER=${MUSL_VER} \
	BINUTILS_VER=${BINUTILS_VER} \
	GMP_VER=${GMP_VER} \
	MPC_VER=${MPC_VER} \
	MPFR_VER=${MPFR_VER} \
	ISL_VER=${ISL_VER} \
	LINUX_VER=${LINUX_VER} \
	install && \
	musl-cross-make/output/bin/${TARGET}-gcc-ar rc musl-cross-make/output/${TARGET}/lib/libc_dl.a musl-cross-make/build/local/${TARGET}/obj_musl/obj/ldso/*.lo && \
	ln -sf libc.so musl-cross-make/output/${TARGET}/lib/ld-* && \
	make -C musl-cross-make clean

# Build patchelf
ARG PATCHELF_BZ2_URI=https://github.com/NixOS/patchelf/releases/download/0.18.0/patchelf-0.18.0.tar.bz2
RUN (cat musl-cross-make/sources/patchelf-*.tar.bz2 || wget -O - "$PATCHELF_BZ2_URI") | tar xj && cp -a patchelf-* cross-patchelf && cd patchelf-* && \
	./configure CXXFLAGS="-static -Os" && \
	make && \
	strip src/patchelf && \
	make check CXXFLAGS= && \
	mv src/patchelf ../musl-cross-make/output/bin/ && \
	cd ../cross-patchelf && \
	./configure --host=${TARGET/-musl/-gnu} CXX=/musl-cross-make/output/bin/${TARGET}-g++ CXXFLAGS="-static -Os -flto" && \
	make && \
	/musl-cross-make/output/bin/${TARGET}-strip src/patchelf && \
	mkdir -p ../musl-cross-make/output/bin-native && \
	mv src/patchelf ../musl-cross-make/output/bin-native/ && \
	cd .. && rm -rf patchelf-* cross-patchelf

COPY --link src /musl-cross-src/
COPY --link results /musl-cross-results/

# Build patchar
RUN g++ -static -Os -Wall -flto -o /musl-cross-make/output/bin/patchar /musl-cross-src/patchar.cxx && \
	strip /musl-cross-make/output/bin/patchar

RUN /musl-cross-make/output/bin/${TARGET}-g++ -static -Os -Wall -flto -o /musl-cross-make/output/bin-native/patchar /musl-cross-src/patchar.cxx && \
	/musl-cross-make/output/bin/${TARGET}-strip /musl-cross-make/output/bin-native/patchar

RUN /musl-cross-make/output/bin/patchar /musl-cross-make/output/${TARGET}/lib/libc.a /musl-cross-make/output/${TARGET}/lib/libgabi.a -ar /musl-cross-make/output/bin/${TARGET}-gcc-ar -nm /musl-cross-make/output/bin/${TARGET}-gcc-nm -objcopy /musl-cross-make/output/bin/${TARGET}-objcopy \
		-ignore '_GLOBAL_OFFSET_TABLE_,[.]TOC[.],_.*[.]get_pc_thunk[.].*,_(rest|save)[gf]pr[0-2]?_[0-9]+.*,__stack_chk_.*' -defined '_*environ,_*errno_location,pthread_.*' -exclude '.*,-__a_.*,-__libc,-__lock,-__procfdname,-__syscall_.*,-__unlock' \
		-ignore '__aeabi_.*,__.*di[34],__.*[dst]f[ds]i,__.*[ds]i[dst]f,__mul[dstx]c3,__.*tf[23],__.*tf[ds]f2' \
		-defined 'strlen' -exclude '-.*basename' \
		-defined 'memset' -exclude '-explicit_bzero' \
		-exclude '-fcntl,-msgctl,-semctl,-shmctl' \
		-defined 'getpid' -exclude '-readdir(64)?' \
		-exclude '-creat,-fallocate,-ftruncate,-getdents,-getrlimit,-lockf,-_*lseek,-open,-openat,-posix_fadvise,-posix_fallocate,-pread,-preadv,-prlimit,-pwrite,-pwritev,-sendfile,-setrlimit,-truncate' \
		-exclude '-__exp(2f)?_.*,-__fpclassify.?,-__log(2f)?_.*,-__math_.*,-__p1evll,-__polevll,-__powf?_.*,-__rsqrt_tab,-__signbit.?,-ceil.?,-div,-fabs.?,-floor.?,-fmod.?,-frexp.?,-ilogb.?,-log.?,-log10.?,-log1p.?,-log2.?,-l*rint.?,-l*round.?,-ldexp.?,-modf.?,-nan.?,-pow.?,-remquo.?,-scalbl?n.?,-sqrt.?,-trunc.?' \
		-exclude '-getentropy,-getrandom' \
		-exclude '-mknod,-mknodat' \
		-exclude '-renameat2' \
		-defined '-pthread_equal' -exclude '-_*pthread_equal,-thrd_equal,-_*thrd_sleep.*,-thrd_yield' \
		-defined 'getenv' -exclude '-_*secure_getenv' \
		-exclude '-_*stat.*,-_*fstat.*,-_*lstat.*,-_*fstatat.*' \
		-defined 'strnlen' -exclude '-strlcat,-strlcpy' \
		-exclude '-__convert_scm_timestamps,-_*p?poll.*,-_*recvm?msg.*,-_*sendm?msg.*,-_*p?select.*' \
		-defined 'asctime(_r)?,localtime(_r)?,memcpy,strcmp' -exclude '-__vdsosym,-__.*_to_secs,-__secs_to_.*,-__utc,-_*clock_nanosleep.*,-_*clock_getres.*,-_*clock_gettime.*,-_*futimesat.*,-_*gmtime(_r)?,-_*nanosleep.*,-_*timegm.*,-_*timerfd_.*,-timespec_get,-.*time64.*(includes 39/62)*' -info | tee patchar.log && \
	diff <(sed -nre '/^[^ ]+$/p' -e '/objcopy /p' -e 's/^Including symbol ([^ ]+).*/\1/p' /musl-cross-results/libgabi.${TARGET}.sym | sort) <(sed -nre '/objcopy /p' -e 's/^Including symbol ([^ ]+).*/\1/p' patchar.log | sort) && \
	/musl-cross-make/output/bin/${TARGET}-gcc -DNO_GLIBC_ABI_COMPATIBLE -O3 -flto -ffat-lto-objects -ffunction-sections -fdata-sections -fPIC -Wall -c -o compat_libc.o /musl-cross-src/compat_libc.c && \
	/musl-cross-make/output/bin/${TARGET}-gcc-ar r /musl-cross-make/output/${TARGET}/lib/libgabi.a compat_libc.o && \
	rm -rf compat_libc.o

# Build libuuid
ARG UTIL_LINUX_GZ_URI=https://github.com/util-linux/util-linux/archive/refs/tags/v2.41.tar.gz
RUN (cat musl-cross-make/sources/util-linux.tar.gz || wget -O - "$UTIL_LINUX_GZ_URI") | tar xz && cd util-linux-* && \
	./autogen.sh && \
	./configure --disable-all-programs --enable-libuuid --host ${TARGET} CC=/musl-cross-make/output/bin/${TARGET}-gcc AR=/musl-cross-make/output/bin/${TARGET}-gcc-ar RANLIB=/musl-cross-make/output/bin/${TARGET}-gcc-ranlib CFLAGS="-g -O3 -flto -ffat-lto-objects -ffunction-sections -fdata-sections -fPIC -lgabi" && \
	make -j && \
	mv libuuid.la .libs/libuuid.a ../musl-cross-make/output/${TARGET}/lib/ && \
	mkdir -p ../musl-cross-make/output/${TARGET}/include/uuid && cp libuuid/src/uuid.h ../musl-cross-make/output/${TARGET}/include/uuid/ && \
	cd .. && \
	rm -rf util-linux-*

# Build libcompat_* libraries (time64 only on 32-bit architectures)
RUN if [[ " arm armeb i386 i686 mips mipsel powerpc " =~ " ${TARGET%%-*} " ]]; then \
		/musl-cross-make/output/bin/${TARGET}-gcc -DNO_GLIBC_ABI_COMPATIBLE -O3 -nostdlib -fPIC -fvisibility=hidden -Wall -pedantic -shared -o /musl-cross-make/output/${TARGET}/lib/libcompat_time64.so /musl-cross-src/compat_time64.c -lgabi -lgcc && \
		/musl-cross-make/output/bin/${TARGET}-strip --remove-section=.comment --remove-section=.note.* /musl-cross-make/output/${TARGET}/lib/libcompat_time64.so; \
	fi;


# Copy toolchain into scratch image
FROM scratch AS deploy
COPY --from=build /musl-cross-make/output/ /
