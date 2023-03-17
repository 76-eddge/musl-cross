/*
	Copyright 2023 Nick Little

	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <pthread.h>
#include <stdlib.h>
#include <sys/stat.h>

int __xstat(int, const char *__restrict, struct stat *__restrict);
int __fxstat(int, int, struct stat *);
int __lxstat(int, const char *__restrict, struct stat *__restrict);
int __fxstatat(int, int, const char *__restrict, struct stat *__restrict, int);

// See glibc xstatver.h
#if defined(__aarch64__) || (defined(__riscv) && __riscv_xlen == 64)
#define GLIBC_ABI_XSTAT_VERSION 0
#elif defined(__x86_64__) || defined(__ia64__) || defined(__powerpc64__) || defined(__s390x__)
#define GLIBC_ABI_XSTAT_VERSION 1
#else
#define GLIBC_ABI_XSTAT_VERSION 3
#endif

__attribute__((__weak__)) int stat(const char *__restrict path, struct stat *__restrict buf)
{
	return __xstat(GLIBC_ABI_XSTAT_VERSION, path, buf);
}

__attribute__((__weak__)) int fstat(int fd, struct stat *buf)
{
	return __fxstat(GLIBC_ABI_XSTAT_VERSION, fd, buf);
}

__attribute__((__weak__)) int lstat(const char *__restrict path, struct stat *__restrict buf)
{
	return __lxstat(GLIBC_ABI_XSTAT_VERSION, path, buf);
}

__attribute__((__weak__)) int fstatat(int fd, const char *__restrict path, struct stat *__restrict buf, int flag)
{
	return __fxstatat(GLIBC_ABI_XSTAT_VERSION, fd, path, buf, flag);
}

int __xmknod(int, const char *, mode_t, dev_t*);
int __xmknodat(int, int, const char *, mode_t, dev_t*);

// See glibc xstatver.h
#if defined(__x86_64__) || defined(__alpha__) || defined(__ia64__) || defined(__s390x__)
#define GLIBC_ABI_XMKNOD_VERSION 0
#else
#define GLIBC_ABI_XMKNOD_VERSION 1
#endif

__attribute__((__weak__)) int mknod(const char *path, mode_t mode, dev_t dev)
{
	return __xmknod(GLIBC_ABI_XMKNOD_VERSION, path, mode, &dev);
}

__attribute__((__weak__)) int mknodat(int fd, const char *path, mode_t mode, dev_t dev)
{
	return __xmknodat(GLIBC_ABI_XMKNOD_VERSION, fd, path, mode, &dev);
}

// Functions using __dso_handle
extern void *__dso_handle __attribute__((__visibility__("hidden")));

int __cxa_atexit(void (*)(void *), void *, void *);

__attribute__((__weak__)) int __cxa_at_quick_exit(void (*func)(void *), void *handle)
{
	return at_quick_exit((void (*)(void))func);
}

__attribute__((__weak__)) int atexit(void (*func)(void))
{
	return __cxa_atexit((void (*)(void *))func, 0, __dso_handle);
}

__attribute__((__weak__)) int at_quick_exit(void (*func)(void))
{
	return __cxa_at_quick_exit((void (*)(void *))func, __dso_handle);
}

__attribute__((__weak__)) int __register_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void), void *handle)
{
	return pthread_atfork(prepare, parent, child);
}

__attribute__((__weak__)) int pthread_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void))
{
	return __register_atfork(prepare, parent, child, __dso_handle);
}

