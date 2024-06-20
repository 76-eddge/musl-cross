/*
	Copyright 2022-2023 Nick Little

	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <errno.h>

#ifndef NULL
	#define NULL ((void *)0)
#endif

typedef union { int value; void *pointer; } size_t;

void *dlsym(void *restrict handle, const char *restrict name);

#define RTLD_DEFAULT ((void *)0)
#define RTLD_NEXT ((void *)-1)

#if defined(__alpha__) || defined(__alpha)
	#define MIN_GLIBC 2
	#define MIN_GLIBC_MINOR 0
#elif defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
	#define MIN_GLIBC 2
	#define MIN_GLIBC_MINOR 3
#elif defined(__aarch64__) || defined(__aarch64)
	#define MIN_GLIBC 2
	#define MIN_GLIBC_MINOR 17
#elif defined(__arm__) || defined(__arm)
	#define MIN_GLIBC 2
	#define MIN_GLIBC_MINOR 4
#elif defined(__i386__) || defined(__i386) || defined(__386) || defined(__i486__) || defined(__i586__) || defined(__i686__)
	#define MIN_GLIBC 2
	#define MIN_GLIBC_MINOR 0
#elif defined(__ia64__) || defined(__ia64)
	#define MIN_GLIBC 2
	#define MIN_GLIBC_MINOR 2
#elif defined(__powerpc64__) || defined(__ppc64__)
	#define MIN_GLIBC 2
	#define MIN_GLIBC_MINOR 3
#elif defined(__powerpc__) || defined(__powerpc) || defined(__ppc__)
	#define MIN_GLIBC 2
	#define MIN_GLIBC_MINOR 0
#elif defined(mips) || defined(__mips__) || defined(__mips)
	#define MIN_GLIBC 2
	#define MIN_GLIBC_MINOR 0
#elif defined(__sh__)
	#define MIN_GLIBC 2
	#define MIN_GLIBC_MINOR 2
#else
	#define MIN_GLIBC 2
	#define MIN_GLIBC_MINOR 0
#endif // defined(__alpha__) || defined(__alpha)

__attribute__((__weak__)) int __register_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void), void *dso_handle)
{
	static void *pthread_atfork_fn = 0;
	static void *register_atfork_fn = 0;

	if (!pthread_atfork_fn)
	{
		if (!register_atfork_fn)
		{
			pthread_atfork_fn = dlsym(RTLD_DEFAULT, "pthread_atfork");
			register_atfork_fn = dlsym(RTLD_NEXT, "__register_atfork");

			if (!pthread_atfork_fn && !register_atfork_fn)
				return ENOMEM;

			return __register_atfork(prepare, parent, child, dso_handle);
		}

		return ((int (*)(void (*)(void), void (*)(void), void (*)(void), void *))register_atfork_fn)(prepare, parent, child, dso_handle);
	}

	return ((int (*)(void (*)(void), void (*)(void), void (*)(void)))pthread_atfork_fn)(prepare, parent, child);
}

#define LOOKUP_LIBC_FUNC(RETURN_TYPE, NAME, NOT_FOUND_RETURN, PARAM_LIST, ARG_LIST) \
RETURN_TYPE NAME PARAM_LIST \
{ \
	static void *NAME ## _fn = 0; \
\
	if (!NAME ## _fn) \
	{ \
		NAME ## _fn = dlsym(RTLD_DEFAULT, #NAME); \
\
		if (!NAME ## _fn) \
			return NOT_FOUND_RETURN; \
	} \
\
	return ((RETURN_TYPE (*)PARAM_LIST)NAME ## _fn)ARG_LIST; \
}

#if MIN_GLIBC == 2 && MIN_GLIBC_MINOR <= 0
__attribute__((__weak__)) LOOKUP_LIBC_FUNC(void*, dlopen, NULL, (const char *filename, int flags), (filename, flags))

__attribute__((__weak__)) LOOKUP_LIBC_FUNC(int, fclose, (errno = EAGAIN, -1), (void *stream), (stream))
__attribute__((__weak__)) LOOKUP_LIBC_FUNC(void*, fdopen, (errno = ENOMEM, NULL), (int fildes, const char *mode), (fildes, mode))
__attribute__((__weak__)) LOOKUP_LIBC_FUNC(void*, fopen, (errno = ENOMEM, NULL), (const char *restrict pathname, const char *restrict mode), (pathname, mode))

__attribute__((__weak__)) LOOKUP_LIBC_FUNC(int, getpwuid_r, EIO, (unsigned uid, struct passwd *pwd, char *buffer, size_t bufsize, struct passwd **result), (uid, pwd, buffer, bufsize, result))
__attribute__((__weak__)) LOOKUP_LIBC_FUNC(int, getservbyname_r, ENOENT, (const char *name, const char *proto, struct servent *result_buf, char *buf, size_t buflen, struct servent **result), (name, proto, result_buf, buf, buflen, result))

__attribute__((__weak__)) LOOKUP_LIBC_FUNC(int, pthread_attr_init, ENOMEM, (struct pthread_attr *attr), (attr))
__attribute__((__weak__)) LOOKUP_LIBC_FUNC(int, pthread_create, EAGAIN, (struct pthread *restrict thread, const struct pthread_attr *restrict attr, void *(*start_routine)(void*), void *restrict arg), (thread, attr, start_routine, arg))

__attribute__((__weak__)) LOOKUP_LIBC_FUNC(int, sem_init, (errno = ENOSPC, -1), (struct sem *sem, int pshared, unsigned value), (sem, pshared, value))
__attribute__((__weak__)) LOOKUP_LIBC_FUNC(int, sem_post, (errno = EINVAL, -1), (struct sem *sem), (sem))
__attribute__((__weak__)) LOOKUP_LIBC_FUNC(int, sem_wait, (errno = EINVAL, -1), (struct sem *sem), (sem))
#endif // MIN_GLIBC == 2 && MIN_GLIBC_MINOR <= 0

#if MIN_GLIBC == 2 && MIN_GLIBC_MINOR <= 3
__attribute__((__weak__)) LOOKUP_LIBC_FUNC(int, nftw, EMFILE, (const char *path, void *fn, int fd_limit, int flags), (path, fn, fd_limit, flags))

__attribute__((__weak__)) LOOKUP_LIBC_FUNC(int, pthread_cond_init, ENOMEM, (struct pthread_cond *cond, const struct pthread_condattr *attr), (cond, attr))
__attribute__((__weak__)) LOOKUP_LIBC_FUNC(int, pthread_cond_destroy, EINVAL, (struct pthread_cond *cond), (cond))

__attribute__((__weak__)) LOOKUP_LIBC_FUNC(int, pthread_cond_signal, EINVAL, (struct pthread_cond *cond), (cond))
__attribute__((__weak__)) LOOKUP_LIBC_FUNC(int, pthread_cond_broadcast, EINVAL, (struct pthread_cond *cond), (cond))
__attribute__((__weak__)) LOOKUP_LIBC_FUNC(int, pthread_cond_wait, EINVAL, (struct pthread_cond *cond, struct pthread_mutex *mutex), (cond, mutex))
__attribute__((__weak__)) LOOKUP_LIBC_FUNC(int, pthread_cond_timedwait, EINVAL, (struct pthread_cond *cond, struct pthread_mutex *mutex, const struct timespec *abstime), (cond, mutex, abstime))

__attribute__((__weak__)) LOOKUP_LIBC_FUNC(int, pthread_setaffinity_np, EINVAL, (struct pthread *thread, size_t cpusetsize, const struct cpu_set *cpuset), (thread, cpusetsize, cpuset))

__attribute__((__weak__)) LOOKUP_LIBC_FUNC(int, sched_getaffinity, (errno = EINVAL, -1), (unsigned pid, size_t cpusetsize, struct cpu_set *mask), (pid, cpusetsize, mask))
#endif // MIN_GLIBC == 2 && MIN_GLIBC_MINOR <= 3
