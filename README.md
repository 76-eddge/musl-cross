# musl-cross

[musl-cross](https://github.com/76-eddge/musl-cross) is a set of statically-linked, cross-compilation toolchains for x86-64 Linux systems.
It uses the [musl-cross-make](https://github.com/richfelker/musl-cross-make) project to build the toolchains.

## Goals

- Support compilation of executables and shared libraries for a variety of architectures using commonly available x86-64 hardware.
- Support compilation of executables and shared libraries that run on systems with older libc libraries.
- Use the latest releases of compilers / tools with support for the newest features and optimizations.

## FAQ

### How do I use the toolchains?

All the toolchains are x86-64-hosted and linked statically, meaning they can run on any x86-64 Linux distro.
The latest release for a specific target can be downloaded from the [project releases](https://github.com/76-eddge/musl-cross/releases).
The executables, located in the archive's `bin` directory, can be used to compile and link software.
(e.g., `bin/x86_64-linux-musl-g++ -o a.out main.cpp`)

### How do I support systems with glibc?

For executables, the easiest way to support glibc systems is to statically link with musl by passing the `-static` option to the compiler during the linking phase: `bin/x86_64-linux-musl-g++ -static -o a.out main.cpp`.
This completely bypasses dynamic linking against any system libraries when the executable is run.

For shared libraries, there are a couple different ways to support glibc-based systems.
The toolchains target musl as the default C library, but they use a modified version of musl that exposes an ABI compatible with older versions of glibc.
This allows libraries that are dynamically-linked against the musl C library to be modified, using the included `patchelf` utility, to point at glibc instead of musl: `patchelf --replace-needed libc.so libc.so.6 ...`.
If compatibility with older versions of glibc are needed, then additional dependencies may need to be added, depending on which symbols are used: `patchelf --add-needed libpthread.so.6 ...`.

Another option for shared libraries is to entirely remove the dependency on the C library after building: `patchelf --remove-needed libc.so ...`.
This will cause the shared library to be loaded and linked using the current C library loaded by the process.
This should be used with caution, as it relies on the process that is loading the shared library to already have the C library loaded (as well as any other libraries needed).
These options should work for any C library that is binary-compatible with glibc / musl, not just glibc.

The `test` directory contains a `Makefile` and set of C++ files that can be used to demonstrate this functionality.
The command below can be used to test these functions on a local x86-64 system.
All variables prefixed with `CC_` are used during cross compilation against musl, and all other variables are used to refer to the system tools for native compilation.
First, several executables and libraries are built using the musl cross compiler.
Next, the native executables are built.
Finally, the tests are run using the native system libraries.

```sh
make SRC_DIR=test BIN_DIR=native-bin CC_DIR=cc-bin CXX=g++ CC_CXX=bin/x86_64-linux-musl-g++ CXXFLAGS='-O2 -Wall' -f test/Makefile run-tests
```

### How are the toolchains built?

The toolchains are built using the [musl-cross-make](https://github.com/richfelker/musl-cross-make) project with the following configuration options / modifications:

- Executables are built statically on Alpine Linux using `COMMON_CONFIG='CC="gcc -static --static" CXX="g++ -static --static" ...'` to support running on any x86-64 Linux distro. (<https://github.com/richfelker/musl-cross-make/issues/64#issuecomment-497881830>)
- All standard system libraries are compiled using the `-fPIC` option to support linking the static version of these libraries into shared libraries.
- Only libc is built as both a static library and shared library.
  All other standard system libraries are built as static libraries only.
- A glibc-compatible library is provided to support the older glibc ABI, where `*stat` functionality was exposed through the `__*xstat` functions and `mknod*` functionality was exposed through the `__xmknod*` functions.
  The compatibility library also supports the `atexit`, `at_quick_exit`, and `pthread_atfork` functions as well.
  (It is also compatible with the current ABI.)
- A glibc-compatible time64 library is provided to support the time64 glibc ABI on systems where 64-bit time has not been implemented.
- The `patchelf` utility is included.

### Can I use the toolchains to create static executables?

Yes, the toolchains can be used with the `-static` option.
None of the modifications to the toolchains should affect creating static executables.

### What version of GCC is used?

The version of GCC will vary and depends on support from the [musl-cross-make](https://github.com/richfelker/musl-cross-make) project.
It will be one of the latest stable releases.
The specific version of GCC and its dependencies are listed in [Dockerfile](Dockerfile).
