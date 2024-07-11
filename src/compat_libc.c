/*
	Copyright 2022-2024 Nick Little

	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <errno.h>

#ifndef NULL
	#define NULL ((void *)0)
#endif

#define POSSIBLY_UNDEFINED __attribute__((__weak__))

void *dlsym(void *restrict handle, const char *restrict name);

#define RTLD_DEFAULT ((void *)0)
#define RTLD_NEXT ((void *)-1)

#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
	#if defined(__ILP32__)
		#define MIN_GLIBC 2
		#define MIN_GLIBC_MINOR 16
		#define WORD_SIZE 32
	#else
		#define MIN_GLIBC 2
		#define MIN_GLIBC_MINOR 3
		#define WORD_SIZE 64
	#endif
#elif defined(__aarch64__) || defined(__aarch64)
	#define MIN_GLIBC 2
	#define MIN_GLIBC_MINOR 17
	#define WORD_SIZE 64
#elif defined(__arm__) || defined(__arm)
	#define MIN_GLIBC 2
	#define MIN_GLIBC_MINOR 4
	#define WORD_SIZE 32
#elif defined(__i386__) || defined(__i386) || defined(__386) || defined(__i486__) || defined(__i586__) || defined(__i686__)
	#define MIN_GLIBC 2
	#define MIN_GLIBC_MINOR 0
	#define WORD_SIZE 32
#elif defined(__ia64__) || defined(__ia64)
	#define MIN_GLIBC 2
	#define MIN_GLIBC_MINOR 2
	#define WORD_SIZE 64
#elif defined(__powerpc64__) || defined(__ppc64__)
	#define MIN_GLIBC 2
	#define MIN_GLIBC_MINOR 3
	#define WORD_SIZE 64
#elif defined(__LP64__)
	#define MIN_GLIBC 2
	#define MIN_GLIBC_MINOR 0
	#define WORD_SIZE 64
#elif defined(__sh__)
	#define MIN_GLIBC 2
	#define MIN_GLIBC_MINOR 2
	#define WORD_SIZE 32
#else
	#define MIN_GLIBC 2
	#define MIN_GLIBC_MINOR 0
	#define WORD_SIZE 32
#endif

// Bypass Functions:
//   The standard public functions that map to these functions are marked hidden by the static glibc library (libc_nonshared.a), resulting in link-time failures when linking any DSO compiled against musl-libc.
//   Therefore, the standard public functions must be bypassed and looked up by name (or using the glibc exposed replacement functions).

POSSIBLY_UNDEFINED int __cxa_at_quick_exit(void (*func)(void *), void *dso_handle)
{
	static void *cxa_at_quick_exit_fn = 0;
	static void *at_quick_exit_fn = 0;

	for (;;) {
		// Try to use the glibc replacement function (__cxa_at_quick_exit) first, then the standard public function (at_quick_exit)
		if (cxa_at_quick_exit_fn)
			return ((int (*)(void (*)(void *), void *))cxa_at_quick_exit_fn)(func, dso_handle);
		else if (at_quick_exit_fn)
			return ((int (*)(void (*)(void *)))at_quick_exit_fn)(func);

		// Attempt to find the glibc replacement function (__cxa_at_quick_exit) or the standard public function (at_quick_exit)
		cxa_at_quick_exit_fn = dlsym(RTLD_NEXT, "__cxa_at_quick_exit");
		at_quick_exit_fn = dlsym(RTLD_DEFAULT, "at_quick_exit");

		// If neither function was found then return ENOSYS
		if (!cxa_at_quick_exit_fn && !at_quick_exit_fn)
			return errno = ENOSYS;
	}
}

POSSIBLY_UNDEFINED int __register_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void), void *dso_handle)
{
	static void *register_atfork_fn = 0;
	static void *pthread_atfork_fn = 0;

	for (;;) {
		// Try to use the glibc replacement function (__register_atfork) first, then the standard public function (pthread_atfork)
		if (register_atfork_fn)
			return ((int (*)(void (*)(void), void (*)(void), void (*)(void), void *))register_atfork_fn)(prepare, parent, child, dso_handle);
		else if (pthread_atfork_fn)
			return ((int (*)(void (*)(void), void (*)(void), void (*)(void)))pthread_atfork_fn)(prepare, parent, child);

		// Attempt to find the glibc replacement function (__register_atfork) or the standard public function (pthread_atfork)
		register_atfork_fn = dlsym(RTLD_NEXT, "__register_atfork");
		pthread_atfork_fn = dlsym(RTLD_DEFAULT, "pthread_atfork");

		// If neither function was found then return ENOSYS
		if (!register_atfork_fn && !pthread_atfork_fn)
			return errno = ENOSYS;
	}
}

// Use LOOKUP_LIBC_FUNC*() to use the latest version of a symbol with some minor overhead used to lookup the symbol the first time it is called.
// Note: Looking up the function (using dlsym()) will find the latest version of the symbol.
//   Whereas linking against the symbol (without a specific version) will always use the oldest version of the symbol.

#define LOOKUP_LIBC_FUNC_NO_RET(NAME, PARAM_LIST, ARG_LIST) \
void NAME PARAM_LIST \
{ \
	static void *NAME ## _fn = 0; \
\
	if (!NAME ## _fn) \
		NAME ## _fn = dlsym(RTLD_DEFAULT, #NAME); \
\
	((void (*)PARAM_LIST)NAME ## _fn)ARG_LIST; \
	__builtin_unreachable(); \
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
			return errno = ENOSYS, NOT_FOUND_RETURN; \
	} \
\
	return ((RETURN_TYPE (*)PARAM_LIST)NAME ## _fn)ARG_LIST; \
}

struct aiocb;
struct cpu_set;
struct dirent;
struct file;
struct fpos;
struct glob;
struct group;
struct hostent;
struct passwd;
struct rlimit;
struct sem;
struct servent;
struct sigevent;
struct spwd;
struct timespec;

struct pthread_attr;
struct pthread_cond;
struct pthread_condattr;
typedef unsigned pthread_key_t;
struct pthread_mutex;
struct pthread_mutexattr;
struct pthread_once;
typedef unsigned long pthread_t;

typedef unsigned long long fpos_t;
typedef unsigned gid_t;
typedef unsigned long long off_t;
typedef union { int value; void *pointer; } size_t;
typedef unsigned socklen_t;
typedef unsigned uid_t;

#if MIN_GLIBC == 2 && MIN_GLIBC_MINOR == 0
POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(void*, dlopen, NULL, (const char *filename, int flags), (filename, flags))

POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(int, fclose, -1, (struct file *stream), (stream))
POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(void*, fdopen, NULL, (int fildes, const char *mode), (fildes, mode))
POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(void*, fopen, NULL, (const char *restrict pathname, const char *restrict mode), (pathname, mode))
POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(int, pclose, -1, (struct file *stream), (stream))
POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(void*, popen, NULL, (const char *command, const char *mode), (command, mode))

POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(int, getgrgid_r, (*result = NULL, ENOSYS), (gid_t gid, struct group *grp, char *buf, size_t buflen, struct group **result), (gid, grp, buf, buflen, result))
POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(int, getgrnam_r, (*result = NULL, ENOSYS), (const char *name, struct group *grp, char *buf, size_t buflen, struct group **result), (name, grp, buf, buflen, result))
POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(int, gethostbyaddr_r, (*result = NULL, ENOSYS), (const void *addr, socklen_t len, int type, struct hostent *ret, char *buf, size_t buflen, struct hostent **result, int *h_errnop), (addr, len, type, ret, buf, buflen, result, h_errnop))
POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(int, gethostbyname_r, (*result = NULL, ENOSYS), (const char *name, socklen_t len, int type, struct hostent *ret, char *buf, size_t buflen, struct hostent **result, int *h_errnop), (name, len, type, ret, buf, buflen, result, h_errnop))
POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(int, gethostbyname2_r, (*result = NULL, ENOSYS), (const char *name, int af, socklen_t len, int type, struct hostent *ret, char *buf, size_t buflen, struct hostent **result, int *h_errnop), (name, af, len, type, ret, buf, buflen, result, h_errnop))
POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(int, getpwnam_r, (*result = NULL, ENOSYS), (const char *name, struct passwd *pwd, char *buffer, size_t bufsize, struct passwd **result), (name, pwd, buffer, bufsize, result))
POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(int, getpwuid_r, (*result = NULL, ENOSYS), (uid_t uid, struct passwd *pwd, char *buffer, size_t bufsize, struct passwd **result), (uid, pwd, buffer, bufsize, result))
POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(int, getservbyname_r, (*result = NULL, ENOSYS), (const char *name, const char *proto, struct servent *result_buf, char *buf, size_t buflen, struct servent **result), (name, proto, result_buf, buf, buflen, result))
POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(int, getservbyport_r, (*result = NULL, ENOSYS), (int port, const char *proto, struct servent *result_buf, char *buf, size_t buflen, struct servent **result), (port, proto, result_buf, buf, buflen, result))
POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(int, getspnam_r, ENOSYS, (const char *name, struct spwd *spbuf, char *buf, size_t buflen, struct spwd **spbufp), (name, spbuf, buf, buflen, spbufp))

POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(int, pthread_attr_init, ENOSYS, (struct pthread_attr *attr), (attr))
POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(int, pthread_create, ENOSYS, (pthread_t *restrict thread, const struct pthread_attr *restrict attr, void *(*start_routine)(void*), void *restrict arg), (thread, attr, start_routine, arg))

POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(int, sem_init, -1, (struct sem *sem, int pshared, unsigned value), (sem, pshared, value))
POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(int, sem_getvalue, -1, (struct sem *restrict sem, int *restrict sval), (sem, sval))
POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(int, sem_post, -1, (struct sem *sem), (sem))
POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(int, sem_trywait, -1, (struct sem *sem), (sem))
POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(int, sem_wait, -1, (struct sem *sem), (sem))
#endif // MIN_GLIBC == 2 && MIN_GLIBC_MINOR == 0

#if MIN_GLIBC == 2 && MIN_GLIBC_MINOR < 2
POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(int, fgetpos, -1, (struct file *restrict stream, struct fpos *restrict pos), (stream, pos))
POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(int, fsetpos, -1, (struct file *restrict stream, const struct fpos *restrict pos), (stream, pos))
POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(int, glob64, -1, (const char *restrict pattern, int flags, int (*errfunc)(void*), struct glob *restrict pglob), (pattern, flags, errfunc, pglob))
#endif // MIN_GLIBC == 2 && MIN_GLIBC_MINOR < 2

#if MIN_GLIBC == 2 && MIN_GLIBC_MINOR <= 3
POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(int, nftw, ENOSYS, (const char *path, void *fn, int fd_limit, int flags), (path, fn, fd_limit, flags))

POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(int, pthread_cond_broadcast, ENOSYS, (struct pthread_cond *cond), (cond))
POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(int, pthread_cond_destroy, ENOSYS, (struct pthread_cond *cond), (cond))
POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(int, pthread_cond_init, ENOSYS, (struct pthread_cond *cond, const struct pthread_condattr *attr), (cond, attr))
POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(int, pthread_cond_signal, ENOSYS, (struct pthread_cond *cond), (cond))
POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(int, pthread_cond_timedwait, ENOSYS, (struct pthread_cond *cond, struct pthread_mutex *mutex, const struct timespec *abstime), (cond, mutex, abstime))
POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(int, pthread_cond_wait, ENOSYS, (struct pthread_cond *cond, struct pthread_mutex *mutex), (cond, mutex))

POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(int, pthread_setaffinity_np, ENOSYS, (pthread_t thread, size_t cpusetsize, const struct cpu_set *cpuset), (thread, cpusetsize, cpuset))
POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(int, sched_getaffinity, -1, (unsigned pid, size_t cpusetsize, struct cpu_set *mask), (pid, cpusetsize, mask))
#endif // MIN_GLIBC == 2 && MIN_GLIBC_MINOR <= 3

#if MIN_GLIBC == 2 && MIN_GLIBC_MINOR < 22
POSSIBLY_UNDEFINED LOOKUP_LIBC_FUNC(void*, fmemopen, NULL, (void *restrict buf, size_t size, const char *restrict mode), (buf, size, mode))
#endif // MIN_GLIBC == 2 && MIN_GLIBC_MINOR < 22

#if MIN_GLIBC == 2 && MIN_GLIBC_MINOR < 24
__attribute__((__weak__, __noreturn__)) LOOKUP_LIBC_FUNC_NO_RET(quick_exit, (int exit_code), (exit_code))
#endif // MIN_GLIBC == 2 && MIN_GLIBC_MINOR < 24

#if WORD_SIZE == 32 && MIN_GLIBC == 2 && MIN_GLIBC < 34
	#define TIME_FUNCTION(FUNC) __ ## FUNC ## _time64
#else
	#define TIME_FUNCTION(FUNC) FUNC
#endif

#if MIN_GLIBC == 2 && MIN_GLIBC_MINOR < 28
int pthread_once(struct pthread_once *once_control, void (*init_routine)(void));

POSSIBLY_UNDEFINED void call_once(struct pthread_once *once_control, void (*init_routine)(void)) { (void)pthread_once(once_control, init_routine); }

enum
{
	thrd_success  = 0,
	thrd_busy     = 1,
	thrd_error    = 2,
	thrd_nomem    = 3,
	thrd_timedout = 4
};

enum
{
	mtx_plain     = 0,
	mtx_recursive = 1,
	mtx_timed     = 2
};

// Remap cnd_* functions
extern int pthread_cond_broadcast(struct pthread_cond *);
extern int pthread_cond_destroy(struct pthread_cond *);
extern int pthread_cond_init(struct pthread_cond *cond, const struct pthread_condattr *attr);
extern int pthread_cond_signal(struct pthread_cond *);
extern int TIME_FUNCTION(pthread_cond_timedwait)(struct pthread_cond *__restrict, struct pthread_mutex *__restrict, const struct timespec *__restrict);
extern int pthread_cond_wait(struct pthread_cond *__restrict, struct pthread_mutex *__restrict);

POSSIBLY_UNDEFINED int cnd_broadcast(struct pthread_cond *cond) { return pthread_cond_broadcast(cond) == 0 ? thrd_success : thrd_error; }
POSSIBLY_UNDEFINED void cnd_destroy(struct pthread_cond *cond) { (void)pthread_cond_destroy(cond); }
POSSIBLY_UNDEFINED int cnd_init(struct pthread_cond *cond) { return pthread_cond_init(cond, NULL); }
POSSIBLY_UNDEFINED int cnd_signal(struct pthread_cond *cond) { return pthread_cond_signal(cond) == 0 ? thrd_success : thrd_error; }
POSSIBLY_UNDEFINED int cnd_wait(struct pthread_cond *cond, struct pthread_mutex *mutex) { return pthread_cond_wait(cond, mutex) == 0 ? thrd_success : thrd_error; }

POSSIBLY_UNDEFINED int TIME_FUNCTION(cnd_timedwait)(struct pthread_cond *cond, struct pthread_mutex *mutex, const struct timespec *__restrict time_point)
{
	switch (TIME_FUNCTION(pthread_cond_timedwait)(cond, mutex, time_point))
	{
		case 0: return thrd_success;
		case ETIMEDOUT: return thrd_timedout;
		default: return thrd_error;
	}
}

// Remap mtx_* functions
extern int pthread_mutex_destroy(struct pthread_mutex *mutex);
extern int pthread_mutex_init(struct pthread_mutex *mutex, const struct pthread_mutexattr *attr);
extern int pthread_mutex_lock(struct pthread_mutex *mutex);
extern int TIME_FUNCTION(pthread_mutex_timedlock)(struct pthread_mutex *restrict mutex, const struct timespec *restrict abs_timeout);
extern int pthread_mutex_trylock(struct pthread_mutex *mutex);
extern int pthread_mutex_unlock(struct pthread_mutex *mutex);

POSSIBLY_UNDEFINED void mtx_destroy(struct pthread_mutex *mutex) { (void)pthread_mutex_destroy(mutex); }
POSSIBLY_UNDEFINED int mtx_lock(struct pthread_mutex *mutex) { return pthread_mutex_lock(mutex) == 0 ? thrd_success : thrd_error; }
POSSIBLY_UNDEFINED int mtx_unlock(struct pthread_mutex *mutex) { return pthread_mutex_unlock(mutex) == 0 ? thrd_success : thrd_error; }

POSSIBLY_UNDEFINED int mtx_init(struct pthread_mutex *mutex, int type)
{
	type &= mtx_recursive;
	return pthread_mutex_init(mutex, (struct pthread_mutexattr*)&type) == 0 ? thrd_success : thrd_error;
}

POSSIBLY_UNDEFINED int TIME_FUNCTION(mtx_timedlock)(struct pthread_mutex *restrict mutex, const struct timespec *restrict time_point)
{
	switch (TIME_FUNCTION(pthread_mutex_timedlock)(mutex, time_point))
	{
		case 0: return thrd_success;
		case ETIMEDOUT: return thrd_timedout;
		default: return thrd_error;
	}
}

POSSIBLY_UNDEFINED int mtx_trylock(struct pthread_mutex *mutex)
{
	switch (pthread_mutex_trylock(mutex))
	{
		case 0: return thrd_success;
		case EBUSY: return thrd_busy;
		default: return thrd_error;
	}
}

// Remap thrd_* functions
extern int pthread_create(pthread_t *restrict thread, const struct pthread_attr *restrict attr, void *(*start_routine)(void*), void *restrict arg);
extern int pthread_detach(pthread_t thread);
extern int pthread_join(pthread_t thread, void **value_ptr);
extern pthread_t pthread_self(void);
extern void pthread_exit(void *result) __attribute__((__noreturn__));

POSSIBLY_UNDEFINED int thrd_create(pthread_t *restrict thread, void *(*start_routine)(void*), void *restrict arg)
{
	switch (pthread_create(thread, NULL, start_routine, arg))
	{
		case 0: return thrd_success;
		case EAGAIN: return thrd_nomem;
		default: return thrd_error;
	}
}

POSSIBLY_UNDEFINED pthread_t thrd_current() { return pthread_self(); }
POSSIBLY_UNDEFINED int thrd_detach(pthread_t thread) { return pthread_detach(thread) == 0 ? thrd_success : thrd_error; }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"

__attribute__((__weak__, __noreturn__)) void thrd_exit(int result) { pthread_exit((void*)result); }

POSSIBLY_UNDEFINED int thrd_join(pthread_t thread, int *value_ptr)
{
	void *result;

	if (pthread_join(thread, &result) != 0)
		return thrd_error;

	if (value_ptr)
		*value_ptr = (int)result;

	return thrd_success;
}

#pragma GCC diagnostic pop

// Remap tss_* functions
extern int pthread_key_create(pthread_key_t *key, void (*destructor)(void*));
extern int pthread_key_delete(pthread_key_t key);
extern void* pthread_getspecific(pthread_key_t key);
extern int pthread_setspecific(pthread_key_t key, const void *value);

POSSIBLY_UNDEFINED int tss_create(pthread_key_t *key, void (*destructor)(void*)) { return pthread_key_create(key, destructor) == 0 ? thrd_success : thrd_error; }
POSSIBLY_UNDEFINED void tss_delete(pthread_key_t key) { (void)pthread_key_delete(key); }
POSSIBLY_UNDEFINED void* tss_get(pthread_key_t key) { return pthread_getspecific(key); }
POSSIBLY_UNDEFINED int tss_set(pthread_key_t key, const void *value) { return pthread_setspecific(key, value) == 0 ? thrd_success : thrd_error; }
#endif // MIN_GLIBC == 2 && MIN_GLIBC_MINOR < 28

// LFS functions
#if WORD_SIZE == 32
	#define LOOKUP_LIBC64_FUNC(RETURN_TYPE, NAME, END, NOT_FOUND_RETURN, PARAM_LIST, ARG_LIST) \
RETURN_TYPE NAME ## 64 ## END PARAM_LIST \
{ \
	static void *NAME ## _fn = 0; \
\
	if (!NAME ## _fn) \
	{ \
		NAME ## _fn = dlsym(RTLD_NEXT, #NAME "64" #END); \
\
		if (!NAME ## _fn) \
			NAME ## _fn = dlsym(RTLD_DEFAULT, #NAME #END); \
\
		if (!NAME ## _fn) \
			return errno = ENOSYS, NOT_FOUND_RETURN; \
	} \
\
	return ((RETURN_TYPE (*)PARAM_LIST)NAME ## _fn)ARG_LIST; \
}

POSSIBLY_UNDEFINED LOOKUP_LIBC64_FUNC(int, aio_cancel, , -1, (int fd, struct aiocb *cb), (fd, cb))
POSSIBLY_UNDEFINED LOOKUP_LIBC64_FUNC(int, aio_error, , ENOSYS, (const struct aiocb *cb), (cb))
POSSIBLY_UNDEFINED LOOKUP_LIBC64_FUNC(int, aio_fsync, , -1, (int op, struct aiocb *cb), (op, cb))
POSSIBLY_UNDEFINED LOOKUP_LIBC64_FUNC(int, aio_read, , -1, (struct aiocb *cb), (cb))
POSSIBLY_UNDEFINED LOOKUP_LIBC64_FUNC(size_t, aio_return, , (size_t)-1, (struct aiocb *cb), (cb))
POSSIBLY_UNDEFINED LOOKUP_LIBC64_FUNC(int, aio_write, , -1, (struct aiocb *cb), (cb))
POSSIBLY_UNDEFINED LOOKUP_LIBC64_FUNC(int, lio_listio, , -1, (int mode, struct aiocb *restrict const *restrict cbs, int cnt, struct sigevent *restrict sev), (mode, cbs, cnt, sev))

POSSIBLY_UNDEFINED LOOKUP_LIBC64_FUNC(int, alphasort, , -1, (const struct dirent **a, const struct dirent **b), (a, b))
POSSIBLY_UNDEFINED LOOKUP_LIBC64_FUNC(int, readdir, _r, -1, (struct dirent *restrict dirp, struct dirent *restrict entry, struct dirent **restrict result), (dirp, entry, result))
POSSIBLY_UNDEFINED LOOKUP_LIBC64_FUNC(int, scandir, , -1, (const char *path, struct dirent ***res, \
	int (*sel)(const struct dirent *), \
	int (*cmp)(const struct dirent **, const struct dirent **)), (path, res, sel, cmp))
POSSIBLY_UNDEFINED LOOKUP_LIBC64_FUNC(int, versionsort, , -1, (const struct dirent **a, const struct dirent **b), (a, b))

POSSIBLY_UNDEFINED LOOKUP_LIBC64_FUNC(int, fgetpos, , -1, (struct file *restrict f, fpos_t *restrict pos), (f, pos))
POSSIBLY_UNDEFINED LOOKUP_LIBC64_FUNC(int, fseeko, , -1, (struct file *f, off_t off, int whence), (f, off, whence))
POSSIBLY_UNDEFINED LOOKUP_LIBC64_FUNC(int, fsetpos, , -1, (struct file *f, const fpos_t *pos), (f, pos))
POSSIBLY_UNDEFINED LOOKUP_LIBC64_FUNC(off_t, ftello, , (off_t)-1, (struct file *f), (f))
POSSIBLY_UNDEFINED LOOKUP_LIBC64_FUNC(void *, mmap, , (void *)-1, (void *start, size_t len, int prot, int flags, int fd, off_t off), (start, len, prot, flags, fd, off))

#endif // WORD_SIZE == 32
