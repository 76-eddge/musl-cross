# musl-cross

[musl-cross](https://github.com/76-eddge/musl-cross) is a set of statically-linked, cross-compilation toolchains for x86-64 Linux systems.
The [musl-cross-make](https://github.com/richfelker/musl-cross-make) project is used to build the toolchains.

## Goals

- Support compilation of executables and shared libraries for a variety of architectures using commonly available x86-64 hardware.
- Support compilation of executables and shared libraries that run on systems with older libc libraries.
- Use the latest releases of compilers / tools with support for the newest features and optimizations.

## FAQ

### How do I use the toolchains?

All the toolchains are x86-64-hosted and linked statically, meaning they can run on any x86-64 Linux distro.
The latest release for a specific target can be downloaded from the [project releases](https://github.com/76-eddge/musl-cross/releases).
The executables, located in the archive's `bin` directory, can be used to compile and link software.
(e.g. `bin/x86_64-linux-musl-g++ -o a.out main.cpp`)

### How do I support systems with glibc?

For executables, the easiest way to support glibc systems is to statically link with musl by passing the `-static` option to the compiler during the linking phase: `bin/x86_64-linux-musl-g++ -static -o a.out main.cpp`.

For shared libraries, there are a couple different ways to support this.
The toolchains target musl as the default C library, but they use an ABI compatible with older versions of glibc.
This allows libraries that are dynamically-linked against the C library to be modified, using the included `patchelf` utility, to point at glibc instead of musl: `patchelf --replace-needed libc.so libc.so.6 ...`.
If compatibility with older versions of glibc are needed, then additional dependencies may need to be added, depending on which symbols are used: `patchelf --add-needed libpthread.so.6 ...`.

Another option for shared libraries is to remove the dependency on the C library entirely: `patchelf --remove-needed libc.so ...`.
This will cause the shared library to be loaded and linked using the current C library loaded by the process.
This should be used with caution, as it relies on the process loading the shared library to already have the C library loaded (as well as any other libraries that have been removed from the shared library's dynamic section using `patchelf`).
These options should work for any C library that is binary-compatible with glibc / musl, not just glibc.

### How are the toolchains built?

The toolchains are built using the [musl-cross-make](https://github.com/richfelker/musl-cross-make) project with the following configuration options / modifications:

- Executables are built statically on Alpine Linux using `COMMON_CONFIG='CC="gcc -static --static" CXX="g++ -static --static" ...'` to support running on any x86-64 Linux distro. (<https://github.com/richfelker/musl-cross-make/issues/64#issuecomment-497881830>)
- All standard system libraries are compiled using the `-fPIC` option to support linking the static version of these libraries into shared libraries.
- Only libc is built as both a static library and shared library.
  All other standard system libraries are built as static libraries only.
- A patch to support the older glibc ABI, where `*stat` functionality was exposed through the `__*xstat` functions and `mknod*` functionality was exposed through the `__xmknod*` functions.
  (The patch is compatible with the current ABI.)
- The `patchelf` utility is included.

### Can I use the toolchains to create static executables?

Yes, the toolchains can be used with the `-static` option.
None of the modifications to the toolchains should affect creating static executables.

### What version of GCC is used?

The version of GCC will vary and depends on support from the [musl-cross-make](https://github.com/richfelker/musl-cross-make) project.
It will be one of the latest stable releases.
The specific version of GCC and its dependencies are listed in [Dockerfile](Dockerfile).
