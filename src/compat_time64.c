/*
	Copyright 2022-2023 Nick Little

	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

// Disable 64-bit Time Redirection
#define _BSD_SOURCE 1
#define _DEFAULT_SOURCE 1
#define _GNU_SOURCE 1
#define _POSIX_C_SOURCE 200809L

#include <features.h>
#undef __REDIR
#define __REDIR(...)

#define aio_suspend(...) __aio_suspend_time64(__VA_ARGS__)
#define cnd_timedwait(...) __cnd_timedwait_time64(__VA_ARGS__)
#define ctime(...) __ctime64(__VA_ARGS__)
#define ctime_r(...) __ctime64_r(__VA_ARGS__)
#define difftime(...) __difftime64(__VA_ARGS__)
#define dlsym(...) __dlsym_time64(__VA_ARGS__)
#define localtime(...) __localtime64(__VA_ARGS__)
#define localtime_r(...) __localtime64_r(__VA_ARGS__)
#define mktime(...) __mktime64(__VA_ARGS__)
#define mtx_timedlock(...) __mtx_timedlock_time64(__VA_ARGS__)
#define pthread_cond_timedwait(...) __pthread_cond_timedwait_time64(__VA_ARGS__)
#define pthread_mutex_timedlock(...) __pthread_mutex_timedlock_time64(__VA_ARGS__)
#define pthread_rwlock_timedrdlock(...) __pthread_rwlock_timedrdlock_time64(__VA_ARGS__)
#define pthread_rwlock_timedwrlock(...) __pthread_rwlock_timedwrlock_time64(__VA_ARGS__)
#define pthread_timedjoin_np(...) __pthread_timedjoin_np_time64(__VA_ARGS__)
#define sem_timedwait(...) __sem_timedwait_time64(__VA_ARGS__)
#define semtimedop(...) __semtimedop_time64(__VA_ARGS__)

// Include All 64-bit Time Functions
#include <aio.h>
#include <dlfcn.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/sem.h>
#include <threads.h>
#include <time.h>

// Allow 32-bit Time Functions
#undef aio_suspend
#undef cnd_timedwait
#undef ctime
#undef ctime_r
#undef difftime
#undef dlsym
#undef localtime
#undef localtime_r
#undef mktime
#undef mtx_timedlock
#undef pthread_cond_timedwait
#undef pthread_mutex_timedlock
#undef pthread_rwlock_timedrdlock
#undef pthread_rwlock_timedwrlock
#undef pthread_timedjoin_np
#undef sem_timedwait
#undef semtimedop

// Helper Macros
static const short _endian_check = (short)0x1010100;
#define OFFSET_TIME_64_TO_32 (*(char*)&_endian_check)

#define POSSIBLY_UNDEFINED __attribute__((__weak__, visibility ("default")))

// 32-bit Time Structures & Conversion Functions
typedef long time32_t;

struct timespec32 { time32_t tv_sec; long tv_nsec; };

static inline struct timespec32 ConvertTimeSpec64To32(const struct timespec ts) {
	return (struct timespec32){ .tv_sec = (time32_t)ts.tv_sec, .tv_nsec = (long)ts.tv_nsec };
}

// 32-bit Time Functions
int aio_suspend(const struct aiocb *const[], int, const struct timespec32*);
char *ctime(const time32_t*);
char *ctime_r(const time32_t*, char*);
void *dlsym(void*, const char*);
double difftime(time32_t, time32_t);
struct tm *localtime(const time32_t*);
struct tm *localtime_r(const time32_t*, struct tm*);
time32_t mktime(struct tm*);
int pthread_cond_timedwait(pthread_cond_t*, pthread_mutex_t*, const struct timespec32*);
int pthread_mutex_timedlock(pthread_mutex_t*, const struct timespec32*);
int pthread_rwlock_timedrdlock(pthread_rwlock_t*, const struct timespec32*);
int pthread_rwlock_timedwrlock(pthread_rwlock_t*, const struct timespec32*);
int pthread_timedjoin_np(pthread_t, void**, const struct timespec32*);
int sem_timedwait(sem_t*, const struct timespec32*);
int semtimedop(int, struct sembuf*, size_t, const struct timespec32*);

// pthread Functions
POSSIBLY_UNDEFINED int __pthread_cond_timedwait64(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex, const struct timespec *restrict abstime) {
	if (!abstime)
		return pthread_cond_timedwait(cond, mutex, (const struct timespec32*)0);

	struct timespec32 tp32 = ConvertTimeSpec64To32(*abstime);
	return pthread_cond_timedwait(cond, mutex, &tp32);
}

POSSIBLY_UNDEFINED int __pthread_cond_timedwait_time64(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex, const struct timespec *restrict abstime) { return __pthread_cond_timedwait64(cond, mutex, abstime); }

POSSIBLY_UNDEFINED int __pthread_mutex_timedlock64(pthread_mutex_t *restrict mutex, const struct timespec *restrict abs_timeout) {
	if (!abs_timeout)
		return pthread_mutex_timedlock(mutex, (const struct timespec32*)0);

	struct timespec32 tp32 = ConvertTimeSpec64To32(*abs_timeout);
	return pthread_mutex_timedlock(mutex, &tp32);
}

POSSIBLY_UNDEFINED int __pthread_mutex_timedlock_time64(pthread_mutex_t *restrict mutex, const struct timespec *restrict abs_timeout) { return __pthread_mutex_timedlock64(mutex, abs_timeout); }

POSSIBLY_UNDEFINED int __pthread_rwlock_timedrdlock64(pthread_rwlock_t *restrict rwlock, const struct timespec *restrict abs_timeout) {
	if (!abs_timeout)
		return pthread_rwlock_timedrdlock(rwlock, (const struct timespec32*)0);

	struct timespec32 tp32 = ConvertTimeSpec64To32(*abs_timeout);
	return pthread_rwlock_timedrdlock(rwlock, &tp32);
}

POSSIBLY_UNDEFINED int __pthread_rwlock_timedrdlock_time64(pthread_rwlock_t *restrict rwlock, const struct timespec *restrict abs_timeout) { return __pthread_rwlock_timedrdlock64(rwlock, abs_timeout); }

POSSIBLY_UNDEFINED int __pthread_rwlock_timedwrlock64(pthread_rwlock_t *restrict rwlock, const struct timespec *restrict abs_timeout) {
	if (!abs_timeout)
		return pthread_rwlock_timedwrlock(rwlock, (const struct timespec32*)0);

	struct timespec32 tp32 = ConvertTimeSpec64To32(*abs_timeout);
	return pthread_rwlock_timedwrlock(rwlock, &tp32);
}

POSSIBLY_UNDEFINED int __pthread_rwlock_timedwrlock_time64(pthread_rwlock_t *restrict rwlock, const struct timespec *restrict abs_timeout) { return __pthread_rwlock_timedwrlock64(rwlock, abs_timeout); }

POSSIBLY_UNDEFINED int __pthread_timedjoin_np64(pthread_t thread, void **retval, const struct timespec *abstime) {
	if (!abstime)
		return pthread_timedjoin_np(thread, retval, (const struct timespec32*)0);

	struct timespec32 tp32 = ConvertTimeSpec64To32(*abstime);
	return pthread_timedjoin_np(thread, retval, &tp32);
}

POSSIBLY_UNDEFINED int __pthread_timedjoin_np_time64(pthread_t thread, void **retval, const struct timespec *abstime) { return __pthread_timedjoin_np64(thread, retval, abstime); }

// Time Functions
POSSIBLY_UNDEFINED int aio_suspend64(const struct aiocb *const aiocb_list[], int nitems, const struct timespec32 *timeout) {
	return aio_suspend(aiocb_list, nitems, timeout);
}

POSSIBLY_UNDEFINED int __aio_suspend_time64(const struct aiocb *const aiocb_list[], int nitems, const struct timespec *restrict timeout) {
	if (!timeout)
		return aio_suspend64(aiocb_list, nitems, (const struct timespec32*)0);

	struct timespec32 ts32 = ConvertTimeSpec64To32(*timeout);
	return aio_suspend64(aiocb_list, nitems, &ts32);
}

POSSIBLY_UNDEFINED int __cnd_timedwait64(cnd_t* restrict cond, mtx_t* restrict mutex, const struct timespec* restrict time_point) {
	switch (__pthread_cond_timedwait64((pthread_cond_t*)cond, (pthread_mutex_t*)mutex, time_point)) {
		case 0: return thrd_success;
		case ETIMEDOUT: return thrd_timedout;
		default: return thrd_error;
	}
}

POSSIBLY_UNDEFINED int __cnd_timedwait_time64(cnd_t* restrict cond, mtx_t* restrict mutex, const struct timespec* restrict time_point) {
	switch (__pthread_cond_timedwait_time64((pthread_cond_t*)cond, (pthread_mutex_t*)mutex, time_point)) {
		case 0: return thrd_success;
		case ETIMEDOUT: return thrd_timedout;
		default: return thrd_error;
	}
}

POSSIBLY_UNDEFINED char *__ctime64(const time_t *tp) {
	return ctime((const time32_t*)tp + OFFSET_TIME_64_TO_32);
}

POSSIBLY_UNDEFINED char *__ctime64_r(const time_t *tp, char *buf) {
	return ctime_r((const time32_t*)tp + OFFSET_TIME_64_TO_32, buf);
}

POSSIBLY_UNDEFINED double __difftime64(time_t t1, time_t t0) {
	return t1 - t0;
}

POSSIBLY_UNDEFINED void *__dlsym_time64(void *restrict handle, const char *restrict symbol) {
	return dlsym(handle, symbol);
}

POSSIBLY_UNDEFINED struct tm *__localtime64(const time_t *tp) {
	return localtime((const time32_t*)tp + OFFSET_TIME_64_TO_32);
}

POSSIBLY_UNDEFINED struct tm *__localtime64_r(const time_t *tp, struct tm *result) {
	return localtime_r((const time32_t*)tp + OFFSET_TIME_64_TO_32, result);
}

POSSIBLY_UNDEFINED time_t __mktime64(struct tm *tm) {
	return mktime(tm);
}

POSSIBLY_UNDEFINED int __mtx_timedlock64(mtx_t *restrict mutex, const struct timespec *restrict time_point) {
	switch (__pthread_mutex_timedlock64((pthread_mutex_t*)mutex, time_point)) {
		case 0: return thrd_success;
		case ETIMEDOUT: return thrd_timedout;
		default: return thrd_error;
	}
}

POSSIBLY_UNDEFINED int __mtx_timedlock_time64(mtx_t *restrict mutex, const struct timespec *restrict time_point) {
	switch (__pthread_mutex_timedlock_time64((pthread_mutex_t*)mutex, time_point)) {
		case 0: return thrd_success;
		case ETIMEDOUT: return thrd_timedout;
		default: return thrd_error;
	}
}

POSSIBLY_UNDEFINED int __sem_timedwait64(sem_t *sem, const struct timespec *abs_timeout) {
	if (!abs_timeout)
		return sem_timedwait(sem, (const struct timespec32*)0);

	struct timespec32 ts32 = ConvertTimeSpec64To32(*abs_timeout);
	return sem_timedwait(sem, &ts32);
}

POSSIBLY_UNDEFINED int __sem_timedwait_time64(sem_t *sem, const struct timespec *abs_timeout) { return __sem_timedwait64(sem, abs_timeout); }

POSSIBLY_UNDEFINED int __semtimedop64(int semid, struct sembuf *sops, size_t nsops, const struct timespec *timeout) {
	if (!timeout)
		return semtimedop(semid, sops, nsops, (const struct timespec32*)0);

	struct timespec32 ts32 = ConvertTimeSpec64To32(*timeout);
	return semtimedop(semid, sops, nsops, &ts32);
}

POSSIBLY_UNDEFINED int __semtimedop_time64(int semid, struct sembuf *sops, size_t nsops, const struct timespec *timeout) { return __semtimedop64(semid, sops, nsops, timeout); }
