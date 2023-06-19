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

#define adjtime(...) __adjtime64(__VA_ARGS__)
#define adjtimex(...) __adjtimex_time64(__VA_ARGS__)
#define aio_suspend(...) __aio_suspend_time64(__VA_ARGS__)
#define clock_adjtime(...) __clock_adjtime64(__VA_ARGS__)
#define clock_getres(...) __clock_getres_time64(__VA_ARGS__)
#define clock_gettime(...) __clock_gettime64(__VA_ARGS__)
#define clock_nanosleep(...) __clock_nanosleep_time64(__VA_ARGS__)
#define clock_settime(...) __clock_settime64(__VA_ARGS__)
#define cnd_timedwait(...) __cnd_timedwait_time64(__VA_ARGS__)
#define ctime(...) __ctime64(__VA_ARGS__)
#define ctime_r(...) __ctime64_r(__VA_ARGS__)
#define difftime(...) __difftime64(__VA_ARGS__)
#define dlsym(...) __dlsym_time64(__VA_ARGS__)
#define fstat(...) __fstat_time64(__VA_ARGS__)
#define fstatat(...) __fstatat_time64(__VA_ARGS__)
#define futimens(...) __futimens_time64(__VA_ARGS__)
#define futimes(...) __futimes_time64(__VA_ARGS__)
#define futimesat(...) __futimesat_time64(__VA_ARGS__)
#define getitimer(...) __getitimer_time64(__VA_ARGS__)
#define getrusage(...) __getrusage_time64(__VA_ARGS__)
#define gettimeofday(...) __gettimeofday_time64(__VA_ARGS__)
#define gmtime(...) __gmtime64(__VA_ARGS__)
#define gmtime_r(...) __gmtime64_r(__VA_ARGS__)
#define localtime(...) __localtime64(__VA_ARGS__)
#define localtime_r(...) __localtime64_r(__VA_ARGS__)
#define lstat(...) __lstat_time64(__VA_ARGS__)
#define lutimes(...) __lutimes_time64(__VA_ARGS__)
#define mktime(...) __mktime64(__VA_ARGS__)
#define mq_timedreceive(...) __mq_timedreceive_time64(__VA_ARGS__)
#define mq_timedsend(...) __mq_timedsend_time64(__VA_ARGS__)
#define mtx_timedlock(...) __mtx_timedlock_time64(__VA_ARGS__)
#define nanosleep(...) __nanosleep_time64(__VA_ARGS__)
#define ppoll(...) __ppoll_time64(__VA_ARGS__)
#define pselect(...) __pselect_time64(__VA_ARGS__)
#define pthread_cond_timedwait(...) __pthread_cond_timedwait_time64(__VA_ARGS__)
#define pthread_mutex_timedlock(...) __pthread_mutex_timedlock_time64(__VA_ARGS__)
#define pthread_rwlock_timedrdlock(...) __pthread_rwlock_timedrdlock_time64(__VA_ARGS__)
#define pthread_rwlock_timedwrlock(...) __pthread_rwlock_timedwrlock_time64(__VA_ARGS__)
#define pthread_timedjoin_np(...) __pthread_timedjoin_np_time64(__VA_ARGS__)
#define recvmmsg(...) __recvmmsg_time64(__VA_ARGS__)
#define sched_rr_get_interval(...) __sched_rr_get_interval_time64(__VA_ARGS__)
#define select(...) __select_time64(__VA_ARGS__)
#define sem_timedwait(...) __sem_timedwait_time64(__VA_ARGS__)
#define semtimedop(...) __semtimedop_time64(__VA_ARGS__)
#define setitimer(...) __setitimer_time64(__VA_ARGS__)
#define settimeofday(...) __settimeofday_time64(__VA_ARGS__)
#define sigtimedwait(...) __sigtimedwait_time64(__VA_ARGS__)
#define stat(...) __stat_time64(__VA_ARGS__)
#define stime(...) __stime64(__VA_ARGS__)
#define thrd_sleep(...) __thrd_sleep_time64(__VA_ARGS__)
#define time(...) __time64(__VA_ARGS__)
#define timegm(...) __timegm_time64(__VA_ARGS__)
#define timer_gettime(...) __timer_gettime64(__VA_ARGS__)
#define timer_settime(...) __timer_settime64(__VA_ARGS__)
#define timerfd_gettime(...) __timerfd_gettime64(__VA_ARGS__)
#define timerfd_settime(...) __timerfd_settime64(__VA_ARGS__)
#define timespec_get(...) __timespec_get_time64(__VA_ARGS__)
#define utime(...) __utime64(__VA_ARGS__)
#define utimensat(...) __utimensat_time64(__VA_ARGS__)
#define utimes(...) __utimes_time64(__VA_ARGS__)
#define wait3(...) __wait3_time64(__VA_ARGS__)
#define wait4(...) __wait4_time64(__VA_ARGS__)

// Include All 64-bit Time Functions
#include <aio.h>
#include <dlfcn.h>
#include <errno.h>
#include <mqueue.h>
#include <poll.h>
#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <signal.h>
#include <stddef.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/select.h>
#include <sys/sem.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/timerfd.h>
#include <sys/timex.h>
#include <sys/wait.h>
#include <threads.h>
#include <time.h>
#include <utime.h>

// Allow 32-bit Time Functions
#undef adjtime
#undef adjtimex
#undef aio_suspend
#undef clock_adjtime
#undef clock_getres
#undef clock_gettime
#undef clock_nanosleep
#undef clock_settime
#undef cnd_timedwait
#undef ctime
#undef ctime_r
#undef difftime
#undef dlsym
#undef fstat
#undef fstatat
#undef ftime
#undef futimens
#undef futimes
#undef futimesat
#undef getitimer
#undef getrusage
#undef gettimeofday
#undef gmtime
#undef gmtime_r
#undef localtime
#undef localtime_r
#undef lstat
#undef lutimes
#undef mktime
#undef mq_timedreceive
#undef mq_timedsend
#undef mtx_timedlock
#undef nanosleep
#undef ppoll
#undef pselect
#undef pthread_cond_timedwait
#undef pthread_mutex_timedlock
#undef pthread_rwlock_timedrdlock
#undef pthread_rwlock_timedwrlock
#undef pthread_timedjoin_np
#undef recvmmsg
#undef sched_rr_get_interval
#undef select
#undef sem_timedwait
#undef semtimedop
#undef setitimer
#undef settimeofday
#undef sigtimedwait
#undef stat
#undef stime
#undef thrd_sleep
#undef time
#undef timegm
#undef timer_gettime
#undef timer_settime
#undef timerfd_gettime
#undef timerfd_settime
#undef timespec_get
#undef utime
#undef utimensat
#undef utimes
#undef wait3
#undef wait4

// Helper Macros
static const short _endian_check = (short)0x1010100;
#define OFFSET_TIME_64_TO_32 (*(char*)&_endian_check)

#define POSSIBLY_UNDEFINED_ATTRIBUTE __attribute__((__weak__, visibility ("default")))

#define SIZE_FROM_FIELD(STRUCT, FIELD) (sizeof(STRUCT) - offsetof(STRUCT, FIELD))

// 32-bit Time Structures & Conversion Functions
typedef long time32_t;
typedef unsigned long utime32_t;

struct timespec32 { time32_t tv_sec; long tv_nsec; };
struct itimerspec32 { struct timespec32 it_interval; struct timespec32 it_value; };

struct timeval32 { time32_t tv_sec; long tv_usec; };
struct itimerval32 { struct timeval32 it_interval; struct timeval32 it_value; };

struct timex32 {
	unsigned modes;
	long offset, freq, maxerror, esterror;
	int status;
	long constant, precision, tolerance;
	struct timeval32 time;
	long tick, ppsfreq, jitter;
	int shift;
	long stabil, jitcnt, calcnt, errcnt, stbcnt;
	int tai;
	int __padding[11];
};

static inline struct timespec ConvertTimeSpec32To64(const struct timespec32 ts) {
	return (struct timespec){ .tv_sec = ts.tv_sec, .tv_nsec = ts.tv_nsec };
}

static inline struct timespec32 ConvertTimeSpec64To32(const struct timespec ts) {
	return (struct timespec32){ .tv_sec = (time32_t)ts.tv_sec, .tv_nsec = (long)ts.tv_nsec };
}

static inline struct itimerspec ConvertITimerSpec32To64(const struct itimerspec32 ts) {
	return (struct itimerspec){ .it_interval = ConvertTimeSpec32To64(ts.it_interval), .it_value = ConvertTimeSpec32To64(ts.it_value) };
}

static inline struct itimerspec32 ConvertITimerSpec64To32(const struct itimerspec ts) {
	return (struct itimerspec32){ .it_interval = ConvertTimeSpec64To32(ts.it_interval), .it_value = ConvertTimeSpec64To32(ts.it_value) };
}

static inline struct timeval ConvertTimeVal32To64(const struct timeval32 ts) {
	return (struct timeval){ .tv_sec = ts.tv_sec, .tv_usec = ts.tv_usec };
}

static inline struct timeval32 ConvertTimeVal64To32(const struct timeval ts) {
	return (struct timeval32){ .tv_sec = (time32_t)ts.tv_sec, .tv_usec = (long)ts.tv_usec };
}

static inline struct itimerval ConvertITimerVal32To64(const struct itimerval32 tv) {
	return (struct itimerval){ .it_interval = ConvertTimeVal32To64(tv.it_interval), .it_value = ConvertTimeVal32To64(tv.it_value) };
}

static inline struct itimerval32 ConvertITimerVal64To32(const struct itimerval tv) {
	return (struct itimerval32){ .it_interval = ConvertTimeVal64To32(tv.it_interval), .it_value = ConvertTimeVal64To32(tv.it_value) };
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"

static inline void ConvertStat32To64(struct stat *stat) {
	stat->st_atim = ConvertTimeSpec32To64(*(struct timespec32*)&stat->__st_atim32);
	stat->st_mtim = ConvertTimeSpec32To64(*(struct timespec32*)&stat->__st_mtim32);
	stat->st_ctim = ConvertTimeSpec32To64(*(struct timespec32*)&stat->__st_ctim32);
}

#pragma GCC diagnostic pop

static inline void ConvertRUsage32To64(struct rusage *usage) {
	memmove(&usage->ru_maxrss, &((struct timeval32*)usage)[2], SIZE_FROM_FIELD(struct rusage, ru_maxrss));
	usage->ru_stime = ConvertTimeVal32To64(((struct timeval32*)usage)[1]);
	usage->ru_utime = ConvertTimeVal32To64(((struct timeval32*)usage)[0]);
}

// 32-bit Time Functions
int adjtime(const struct timeval32*, struct timeval32*);
int adjtimex(struct timex32*);
int aio_suspend64(const struct aiocb *const[], int, const struct timespec32*);
int clock_adjtime(clockid_t, struct timex32*);
int clock_getres(clockid_t, struct timespec32*);
int clock_gettime(clockid_t, struct timespec32*);
int clock_nanosleep(clockid_t, int, const struct timespec32*, struct timespec32*);
int clock_settime(clockid_t, const struct timespec32*);
char *ctime(const time32_t*);
char *ctime_r(const time32_t*, char*);
void *dlsym(void*, const char*);
double difftime(time32_t, time32_t);
int futimens(int, const struct timespec32[2]);
int futimes(int, const struct timeval32[2]);
int futimesat(int, const char*, const struct timeval32[2]);
int getitimer(int, struct itimerval32*);
int getrusage(int, struct rusage*);
int gettimeofday(struct timeval32*, struct timezone*);
struct tm *gmtime(const time32_t*);
struct tm *gmtime_r(const time32_t*, struct tm*);
struct tm *localtime(const time32_t*);
struct tm *localtime_r(const time32_t*, struct tm*);
int lutimes(const char*, const struct timeval32[2]);
time32_t mktime(struct tm*);
ssize_t mq_timedreceive(mqd_t, char*, size_t, unsigned*, const struct timespec32*);
int mq_timedsend(mqd_t, const char*, size_t, unsigned, const struct timespec32*);
int nanosleep(const struct timespec32*, struct timespec32*);
int ppoll(struct pollfd*, nfds_t, const struct timespec32*, const sigset_t*);
int pselect(int, fd_set*, fd_set*, fd_set*, const struct timespec32*, const sigset_t*);
int pthread_cond_timedwait(pthread_cond_t*, pthread_mutex_t*, const struct timespec32*);
int pthread_mutex_timedlock(pthread_mutex_t*, const struct timespec32*);
int pthread_rwlock_timedrdlock(pthread_rwlock_t*, const struct timespec32*);
int pthread_rwlock_timedwrlock(pthread_rwlock_t*, const struct timespec32*);
int pthread_timedjoin_np(pthread_t, void**, const struct timespec32*);
int recvmmsg(int, struct mmsghdr*, unsigned int vlen, int flags, struct timespec32*);
int sched_rr_get_interval(pid_t, struct timespec32*);
int select(int, fd_set*, fd_set*, fd_set*, struct timeval32*);
int sem_timedwait(sem_t*, const struct timespec32*);
int semtimedop(int, struct sembuf*, size_t, const struct timespec32*);
int setitimer(int, const struct itimerval32*, struct itimerval32*);
int settimeofday(const struct timeval32*, const struct timezone*);
int sigtimedwait(const sigset_t*, siginfo_t*, const struct timespec32*);
time32_t time(time32_t*);
time32_t timegm(struct tm*);
int timer_gettime(timer_t, struct itimerspec32*);
int timer_settime(timer_t, int, const struct itimerspec32*, struct itimerspec32*);
int timerfd_settime(int, int, const struct itimerspec32*, struct itimerspec32*);
int timerfd_gettime(int, struct itimerspec32*);
int timespec_get(struct timespec32*, int);
int utime(const char*, const time32_t[2]);
int utimensat(int, const char*, const struct timespec32[2], int);
int utimes(const char*, const struct timeval32[2]);
pid_t wait3(int*, int, struct rusage*);
pid_t wait4(pid_t, int*, int, struct rusage*);

#define GLIBC_ABI_XSTAT_VERSION 3

int __fxstat64(int, int, struct stat*);
int __fxstatat64(int, int, const char*, struct stat*, int);
int __lxstat64(int, const char*, struct stat*);
int __xstat64(int, const char*, struct stat*);

// Time Functions
POSSIBLY_UNDEFINED_ATTRIBUTE int __adjtime64(const struct timeval *delta, struct timeval *olddelta) {
	int result;

	if (delta) {
		struct timeval32 tv32 = ConvertTimeVal64To32(*delta);
		result = adjtime(&tv32, (struct timeval32*)olddelta);
	} else
		result = adjtime((const struct timeval32*)0, (struct timeval32*)olddelta);

	if (result == 0 && olddelta)
		*olddelta = ConvertTimeVal32To64(*(struct timeval32*)olddelta);

	return result;
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __adjtimex_time64(struct timex *tx) {
	return __clock_adjtime64(CLOCK_REALTIME, tx);
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __aio_suspend_time64(const struct aiocb *const aiocb_list[], int nitems, const struct timespec *restrict timeout) {
	if (!timeout)
		return aio_suspend64(aiocb_list, nitems, (const struct timespec32*)0);

	struct timespec32 ts32 = ConvertTimeSpec64To32(*timeout);
	return aio_suspend64(aiocb_list, nitems, &ts32);
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __clock_adjtime64(clockid_t clk_id, struct timex *buf) {
	if (buf) {
		((struct timex32*)buf)->time = ConvertTimeVal64To32(buf->time);
		((struct timex32*)buf)->tick = buf->tick;
	}

	int result = clock_adjtime(clk_id, (struct timex32*)buf);

	if (result != -1) {
		memmove(&buf->tick, &((struct timex32*)buf)->tick, SIZE_FROM_FIELD(struct timex32, tick));
		buf->time = ConvertTimeVal32To64(((struct timex32*)buf)->time);
	}

	return result;
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __clock_getres_time64(clockid_t clockid, struct timespec *res) {
	int result = clock_getres(clockid, (struct timespec32*)res);

	if (result == 0)
		*res = ConvertTimeSpec32To64(*(struct timespec32*)res);

	return result;
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __clock_gettime64(clockid_t clockid, struct timespec *tp) {
	int result = clock_gettime(clockid, (struct timespec32*)tp);

	if (result == 0)
		*tp = ConvertTimeSpec32To64(*(struct timespec32*)tp);

	return result;
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __clock_nanosleep_time64(clockid_t clockid, int flags, const struct timespec *req, struct timespec *rem) {
	int result;
	
	if (req) {
		struct timespec32 req32 = ConvertTimeSpec64To32(*req);
		result = clock_nanosleep(clockid, flags, &req32, (struct timespec32*)rem);
	} else
		result = clock_nanosleep(clockid, flags, (const struct timespec32*)0, (struct timespec32*)rem);

	if (result == 0 && rem)
		*rem = ConvertTimeSpec32To64(*(struct timespec32*)rem);

	return result;
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __clock_settime64(clockid_t clockid, const struct timespec *tp) {
	if (!tp)
		return clock_settime(clockid, (const struct timespec32*)0);

	struct timespec32 ts32 = ConvertTimeSpec64To32(*tp);
	return clock_settime(clockid, &ts32);
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __cnd_timedwait_time64(cnd_t* restrict cond, mtx_t* restrict mutex, const struct timespec* restrict time_point) {
	int result = __pthread_cond_timedwait_time64((pthread_cond_t*)cond, (pthread_mutex_t*)mutex, time_point);

	switch (result) {
		case 0: return thrd_success;
		case ETIMEDOUT: return thrd_timedout;
		default: return thrd_error;
	}
}

POSSIBLY_UNDEFINED_ATTRIBUTE char *__ctime64(const time_t *tp) {
	return ctime((const time32_t*)tp + OFFSET_TIME_64_TO_32);
}

POSSIBLY_UNDEFINED_ATTRIBUTE char *__ctime64_r(const time_t *tp, char *buf) {
	return ctime_r((const time32_t*)tp + OFFSET_TIME_64_TO_32, buf);
}

POSSIBLY_UNDEFINED_ATTRIBUTE double __difftime64(time_t t1, time_t t0) {
	return t1 - t0;
}

POSSIBLY_UNDEFINED_ATTRIBUTE void *__dlsym_time64(void *restrict handle, const char *restrict symbol) {
	void *result = dlsym(handle, symbol);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

	if (result) {
		if (result == adjtime)
			return __adjtime64;
		if (result == adjtimex)
			return __adjtimex_time64;
		if (result == aio_suspend64)
			return __aio_suspend_time64;
		if (result == clock_adjtime)
			return __clock_adjtime64;
		if (result == clock_getres)
			return __clock_getres_time64;
		if (result == clock_gettime)
			return __clock_gettime64;
		if (result == clock_nanosleep)
			return __clock_nanosleep_time64;
		if (result == clock_settime)
			return __clock_settime64;
		if (strcmp(symbol, "cnd_timedwait") == 0)
			return __cnd_timedwait_time64;
		if (result == ctime)
			return __ctime64;
		if (result == ctime_r)
			return __ctime64_r;
		if (result == difftime)
			return __difftime64;
		if (result == dlsym)
			return __dlsym_time64;
		if (strcmp(symbol, "fstat") == 0)
			return __fstat_time64;
		if (strcmp(symbol, "fstatat") == 0)
			return __fstatat_time64;
		if (result == futimens)
			return __futimens_time64;
		if (result == futimes)
			return __futimes_time64;
		if (result == futimesat)
			return __futimesat_time64;
		if (result == getitimer)
			return __getitimer_time64;
		if (result == getrusage)
			return __getrusage_time64;
		if (result == gettimeofday)
			return __gettimeofday_time64;
		if (result == gmtime)
			return __gmtime64;
		if (result == gmtime_r)
			return __gmtime64_r;
		if (result == localtime)
			return __localtime64;
		if (result == localtime_r)
			return __localtime64_r;
		if (strcmp(symbol, "lstat") == 0)
			return __lstat_time64;
		if (result == lutimes)
			return __lutimes_time64;
		if (result == mktime)
			return __mktime64;
		if (result == mq_timedreceive)
			return __mq_timedreceive_time64;
		if (result == mq_timedsend)
			return __mq_timedsend_time64;
		if (strcmp(symbol, "mtx_timedlock") == 0)
			return __mtx_timedlock_time64;
		if (result == nanosleep)
			return __nanosleep_time64;
		if (result == ppoll)
			return __ppoll_time64;
		if (result == pselect)
			return __pselect_time64;
		if (result == pthread_cond_timedwait)
			return __pthread_cond_timedwait_time64;
		if (result == pthread_mutex_timedlock)
			return __pthread_mutex_timedlock_time64;
		if (result == pthread_rwlock_timedrdlock)
			return __pthread_rwlock_timedrdlock_time64;
		if (result == pthread_rwlock_timedwrlock)
			return __pthread_rwlock_timedwrlock_time64;
		if (result == pthread_timedjoin_np)
			return __pthread_timedjoin_np_time64;
		if (result == recvmmsg)
			return __recvmmsg_time64;
		if (result == sched_rr_get_interval)
			return __sched_rr_get_interval_time64;
		if (result == select)
			return __select_time64;
		if (result == sem_timedwait)
			return __sem_timedwait_time64;
		if (result == semtimedop)
			return __semtimedop_time64;
		if (result == setitimer)
			return __setitimer_time64;
		if (result == settimeofday)
			return __settimeofday_time64;
		if (result == sigtimedwait)
			return __sigtimedwait_time64;
		if (strcmp(symbol, "stat") == 0)
			return __stat_time64;
		if (strcmp(symbol, "stime") == 0)
			return __stime64;
		if (strcmp(symbol, "thrd_sleep") == 0)
			return __thrd_sleep_time64;
		if (result == time)
			return __time64;
		if (result == timegm)
			return __timegm_time64;
		if (result == timer_gettime)
			return __timer_gettime64;
		if (result == timer_settime)
			return __timer_settime64;
		if (result == timerfd_gettime)
			return __timerfd_gettime64;
		if (result == timerfd_settime)
			return __timerfd_settime64;
		if (result == timespec_get)
			return __timespec_get_time64;
		if (result == utime)
			return __utime64;
		if (result == utimensat)
			return __utimensat_time64;
		if (result == utimes)
			return __utimes_time64;
		if (result == wait3)
			return __wait3_time64;
		if (result == wait4)
			return __wait4_time64;
	}

#pragma GCC diagnostic pop

	return result;
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __fstat_time64(int fd, struct stat *buf) {
	int result = __fxstat64(GLIBC_ABI_XSTAT_VERSION, fd, buf);

	if (result == 0) {
		ConvertStat32To64(buf);
	}

	return result;
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __fstatat_time64(int dirfd, const char *pathname, struct stat *buf, int flags) {
	int result = __fxstatat64(GLIBC_ABI_XSTAT_VERSION, dirfd, pathname, buf, flags);

	if (result == 0) {
		ConvertStat32To64(buf);
	}

	return result;
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __futimens_time64(int fd, const struct timespec times[2]) {
	if (!times)
		return futimens(fd, (const struct timespec32*)0);

	struct timespec32 ts32[] = { ConvertTimeSpec64To32(times[0]), ConvertTimeSpec64To32(times[1]) };
	return futimens(fd, ts32);
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __futimes_time64(int fd, const struct timeval tv[2]) {
	if (!tv)
		return futimes(fd, (const struct timeval32*)0);

	struct timeval32 tv32s[] = { ConvertTimeVal64To32(tv[0]), ConvertTimeVal64To32(tv[1]) };
	return futimes(fd, tv32s);
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __futimesat_time64(int dirfd, const char *pathname, const struct timeval times[2]) {
	if (!times)
		return futimesat(dirfd, pathname, (const struct timeval32*)0);

	struct timeval32 tv32s[] = { ConvertTimeVal64To32(times[0]), ConvertTimeVal64To32(times[1]) };
	return futimesat(dirfd, pathname, tv32s);
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __getitimer_time64(int which, struct itimerval *tvp) {
	int result = getitimer(which, (struct itimerval32*)tvp);

	if (result == 0)
		*tvp = ConvertITimerVal32To64(*(struct itimerval32*)tvp);

	return result;
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __getrusage_time64(int who, struct rusage *usage) {
	int result = getrusage(who, usage);

	if (result == 0)
		ConvertRUsage32To64(usage);

	return result;
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __gettimeofday_time64(struct timeval *restrict tvp, /*struct timezone*/void *restrict tzp) {
	int result = gettimeofday((struct timeval32*)tvp, (struct timezone*)tzp);

	if (result == 0)
		*tvp = ConvertTimeVal32To64(*(struct timeval32*)tvp);

	return result;
}

POSSIBLY_UNDEFINED_ATTRIBUTE struct tm *__gmtime64(const time_t *tp) {
	return gmtime((const time32_t*)tp + OFFSET_TIME_64_TO_32);
}

POSSIBLY_UNDEFINED_ATTRIBUTE struct tm *__gmtime64_r(const time_t *tp, struct tm *result) {
	return gmtime_r((const time32_t*)tp + OFFSET_TIME_64_TO_32, result);
}

POSSIBLY_UNDEFINED_ATTRIBUTE struct tm *__localtime64(const time_t *tp) {
	return localtime((const time32_t*)tp + OFFSET_TIME_64_TO_32);
}

POSSIBLY_UNDEFINED_ATTRIBUTE struct tm *__localtime64_r(const time_t *tp, struct tm *result) {
	return localtime_r((const time32_t*)tp + OFFSET_TIME_64_TO_32, result);
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __lstat_time64(const char *path, struct stat *buf) {
	int result = __lxstat64(GLIBC_ABI_XSTAT_VERSION, path, buf);

	if (result == 0)
		ConvertStat32To64(buf);

	return result;
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __lutimes_time64(const char *filename, const struct timeval tv[2]) {
	if (!tv)
		return lutimes(filename, (const struct timeval32*)0);

	struct timeval32 tv32s[] = { ConvertTimeVal64To32(tv[0]), ConvertTimeVal64To32(tv[1]) };
	return lutimes(filename, tv32s);
}

POSSIBLY_UNDEFINED_ATTRIBUTE time_t __mktime64(struct tm *tm) {
	return mktime(tm);
}

POSSIBLY_UNDEFINED_ATTRIBUTE ssize_t __mq_timedreceive_time64(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio, const struct timespec *abs_timeout) {
	if (!abs_timeout)
		return mq_timedreceive(mqdes, msg_ptr, msg_len, msg_prio, (const struct timespec32*)0);

	struct timespec32 ts32 = ConvertTimeSpec64To32(*abs_timeout);
	return mq_timedreceive(mqdes, msg_ptr, msg_len, msg_prio, &ts32);
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __mq_timedsend_time64(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio, const struct timespec *abs_timeout) {
	if (!abs_timeout)
		return mq_timedsend(mqdes, msg_ptr, msg_len, msg_prio, (const struct timespec32*)0);

	struct timespec32 ts32 = ConvertTimeSpec64To32(*abs_timeout);
	return mq_timedsend(mqdes, msg_ptr, msg_len, msg_prio, &ts32);
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __mtx_timedlock_time64(mtx_t *restrict mutex, const struct timespec *restrict time_point) {
	int result = __pthread_mutex_timedlock_time64((pthread_mutex_t*)mutex, time_point);

	switch (result) {
		case 0: return thrd_success;
		case ETIMEDOUT: return thrd_timedout;
		default: return thrd_error;
	}
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __nanosleep_time64(const struct timespec *req, struct timespec *rem) {
	int result;

	if (req) {
		struct timespec32 req32 = ConvertTimeSpec64To32(*req);
		result = nanosleep(&req32, (struct timespec32*)rem);
	} else
		result = nanosleep((const struct timespec32*)0, (struct timespec32*)rem);

	if (result == 0 && rem)
		*rem = ConvertTimeSpec32To64(*(struct timespec32*)rem);

	return result;
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __ppoll_time64(struct pollfd *fds, nfds_t nfds, const struct timespec *timeout_ts, const sigset_t *sigmask) {
	if (!timeout_ts)
		return ppoll(fds, nfds, (const struct timespec32*)0, sigmask);

	struct timespec32 ts32 = ConvertTimeSpec64To32(*timeout_ts);
	return ppoll(fds, nfds, &ts32, sigmask);
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __pselect_time64(int nfds, fd_set *restrict readfds, fd_set *restrict writefds, fd_set *restrict exceptfds, const struct timespec *restrict timeout, const sigset_t *restrict sigmask) {
	if (!timeout)
		return pselect(nfds, readfds, writefds, exceptfds, (const struct timespec32*)0, sigmask);

	struct timespec32 ts32 = ConvertTimeSpec64To32(*timeout);
	return pselect(nfds, readfds, writefds, exceptfds, &ts32, sigmask);
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __pthread_cond_timedwait_time64(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex, const struct timespec *restrict abstime) {
	if (!abstime)
		return pthread_cond_timedwait(cond, mutex, (const struct timespec32*)0);

	struct timespec32 tp32 = ConvertTimeSpec64To32(*abstime);
	return pthread_cond_timedwait(cond, mutex, &tp32);
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __pthread_mutex_timedlock_time64(pthread_mutex_t *restrict mutex, const struct timespec *restrict abs_timeout) {
	if (!abs_timeout)
		return pthread_mutex_timedlock(mutex, (const struct timespec32*)0);

	struct timespec32 tp32 = ConvertTimeSpec64To32(*abs_timeout);
	return pthread_mutex_timedlock(mutex, &tp32);
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __pthread_rwlock_timedrdlock_time64(pthread_rwlock_t *restrict rwlock, const struct timespec *restrict abs_timeout) {
	if (!abs_timeout)
		return pthread_rwlock_timedrdlock(rwlock, (const struct timespec32*)0);

	struct timespec32 tp32 = ConvertTimeSpec64To32(*abs_timeout);
	return pthread_rwlock_timedrdlock(rwlock, &tp32);
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __pthread_rwlock_timedwrlock_time64(pthread_rwlock_t *restrict rwlock, const struct timespec *restrict abs_timeout) {
	if (!abs_timeout)
		return pthread_rwlock_timedwrlock(rwlock, (const struct timespec32*)0);

	struct timespec32 tp32 = ConvertTimeSpec64To32(*abs_timeout);
	return pthread_rwlock_timedwrlock(rwlock, &tp32);
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __pthread_timedjoin_np_time64(pthread_t thread, void **retval, const struct timespec *abstime) {
	if (!abstime)
		return pthread_timedjoin_np(thread, retval, (const struct timespec32*)0);

	struct timespec32 tp32 = ConvertTimeSpec64To32(*abstime);
	return pthread_timedjoin_np(thread, retval, &tp32);
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __recvmmsg_time64(int sockfd, struct mmsghdr *msgvec, unsigned int vlen, unsigned int flags, struct timespec *timeout) {
	if (!timeout)
		return recvmmsg(sockfd, msgvec, vlen, (int)flags, (struct timespec32*)0);

	struct timespec32 ts32 = ConvertTimeSpec64To32(*timeout);
	return recvmmsg(sockfd, msgvec, vlen, (int)flags, &ts32);
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __sched_rr_get_interval_time64(pid_t pid, struct timespec *tp) {
	int result = sched_rr_get_interval(pid, (struct timespec32*)tp);

	if (result == 0 && tp)
		*tp = ConvertTimeSpec32To64(*(struct timespec32*)tp);

	return result;
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __select_time64(int nfds, fd_set *restrict readfds, fd_set *restrict writefds, fd_set *restrict exceptfds, struct timeval *restrict timeout) {
	if (!timeout)
		return select(nfds, readfds, writefds, exceptfds, (struct timeval32*)0);

	struct timeval32 ts32 = ConvertTimeVal64To32(*timeout);
	return select(nfds, readfds, writefds, exceptfds, &ts32);
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __sem_timedwait_time64(sem_t *sem, const struct timespec *abs_timeout) {
	if (!abs_timeout)
		return sem_timedwait(sem, (const struct timespec32*)0);

	struct timespec32 ts32 = ConvertTimeSpec64To32(*abs_timeout);
	return sem_timedwait(sem, &ts32);
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __semtimedop_time64(int semid, struct sembuf *sops, size_t nsops, const struct timespec *timeout) {
	if (!timeout)
		return semtimedop(semid, sops, nsops, (const struct timespec32*)0);

	struct timespec32 ts32 = ConvertTimeSpec64To32(*timeout);
	return semtimedop(semid, sops, nsops, &ts32);
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __setitimer_time64(int which, const struct itimerval *restrict new_tvp, struct itimerval *restrict old_tvp) {
	int result;

	if (new_tvp) {
		struct itimerval32 new_tv32 = ConvertITimerVal64To32(*new_tvp);
		result = setitimer(which, &new_tv32, (struct itimerval32*)old_tvp);
	} else
		result = setitimer(which, (const struct itimerval32*)0, (struct itimerval32*)old_tvp);

	if (result == 0 && old_tvp)
		*old_tvp = ConvertITimerVal32To64(*(struct itimerval32*)old_tvp);

	return result;
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __settimeofday_time64(const struct timeval *tvp, const struct timezone *tzp) {
	if (!tvp)
		return settimeofday((const struct timeval32*)0, tzp);

	struct timeval32 tv32 = ConvertTimeVal64To32(*tvp);
	return settimeofday(&tv32, tzp);
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __sigtimedwait_time64(const sigset_t *set, siginfo_t *info, const struct timespec *timeout) {
	if (!timeout)
		return sigtimedwait(set, info, (const struct timespec32*)0);

	struct timespec32 ts32 = ConvertTimeSpec64To32(*timeout);
	return sigtimedwait(set, info, &ts32);
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __stat_time64(const char *path, struct stat *buf) {
	int result = __xstat64(GLIBC_ABI_XSTAT_VERSION, path, buf);

	if (result == 0)
		ConvertStat32To64(buf);

	return result;
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __stime64(const time_t *tp) {
	struct timespec32 time = { *((const time32_t*)tp + OFFSET_TIME_64_TO_32), 0 };
	return clock_settime(CLOCK_REALTIME, &time);
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __thrd_sleep_time64(const struct timespec *duration, struct timespec *remaining) {
	int result;

	if (duration) {
		struct timespec32 duration32 = ConvertTimeSpec64To32(*duration);
		result = nanosleep(&duration32, (struct timespec32*)remaining); // nanosleep is more portable than thrd_sleep
	} else
		result = nanosleep((const struct timespec32*)0, (struct timespec32*)remaining); // nanosleep is more portable than thrd_sleep

	if (result == 0 && remaining)
		*remaining = ConvertTimeSpec32To64(*(struct timespec32*)remaining);
	else if (errno != EINTR)
		return -2;

	return result;
}

POSSIBLY_UNDEFINED_ATTRIBUTE time_t __time64(time_t *tp) {
	utime32_t t = time((time32_t*)tp);

	if (t == (utime32_t)-1)
		return -1;
	else if (tp)
		*tp = (time_t)t;

	return (time_t)t;
}

POSSIBLY_UNDEFINED_ATTRIBUTE time_t __timegm_time64(struct tm *tm) { return timegm(tm); }

POSSIBLY_UNDEFINED_ATTRIBUTE int __timer_gettime64(timer_t timerid, struct itimerspec *tsp) {
	int result = timer_gettime(timerid, (struct itimerspec32*)tsp);

	if (result == 0 && tsp)
		*tsp = ConvertITimerSpec32To64(*(struct itimerspec32*)tsp);

	return result;
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __timer_settime64(timer_t timerid, int flags, const struct itimerspec *restrict new_tsp, struct itimerspec *restrict old_tsp) {
	int result;

	if (new_tsp) {
		struct itimerspec32 new_ts32 = ConvertITimerSpec64To32(*new_tsp);
		result = timer_settime(timerid, flags, &new_ts32, (struct itimerspec32*)old_tsp);
	} else
		result = timer_settime(timerid, flags, (const struct itimerspec32*)0, (struct itimerspec32*)old_tsp);

	if (result == 0 && old_tsp)
		*old_tsp = ConvertITimerSpec32To64(*(struct itimerspec32*)old_tsp);

	return result;
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __timerfd_gettime64(int fd, struct itimerspec *curr_value) {
	int result = timerfd_gettime(fd, (struct itimerspec32*)curr_value);

	if (result == 0 && curr_value)
		*curr_value = ConvertITimerSpec32To64(*(struct itimerspec32*)curr_value);

	return result;
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __timerfd_settime64(int fd, int flags, const struct itimerspec *new_value, struct itimerspec *old_value) {
	int result;

	if (new_value) {
		struct itimerspec32 new_ts32 = ConvertITimerSpec64To32(*new_value);
		result = timerfd_settime(fd, flags, &new_ts32, (struct itimerspec32*)old_value);
	} else
		result = timerfd_settime(fd, flags, (const struct itimerspec32*)0, (struct itimerspec32*)old_value);

	if (result == 0 && old_value)
		*old_value = ConvertITimerSpec32To64(*(struct itimerspec32*)old_value);

	return result;
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __timespec_get_time64(struct timespec *ts, int base) {
	int result = timespec_get((struct timespec32*)ts, base);

	if (result && ts)
		*ts = ConvertTimeSpec32To64(*(struct timespec32*)ts);

	return result;
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __utime64(const char *filename, const struct utimbuf *times) {
	if (!times)
		return utime(filename, (const time32_t*)0);

	time32_t times32[] = { (time32_t)times->actime, (time32_t)times->modtime };
	return utime(filename, times32);
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __utimensat_time64(int dirfd, const char *pathname, const struct timespec times[2], int flags) {
	if (!times)
		return utimensat(dirfd, pathname, (const struct timespec32*)0, flags);

	struct timespec32 ts32s[] = { ConvertTimeSpec64To32(times[0]), ConvertTimeSpec64To32(times[1]) };
	return utimensat(dirfd, pathname, ts32s, flags);
}

POSSIBLY_UNDEFINED_ATTRIBUTE int __utimes_time64(const char *filename, const struct timeval times[2]) {
	if (!times)
		return utimes(filename, (const struct timeval32*)0);

	struct timeval32 tv32s[] = { ConvertTimeVal64To32(times[0]), ConvertTimeVal64To32(times[1]) };
	return utimes(filename, tv32s);
}

POSSIBLY_UNDEFINED_ATTRIBUTE pid_t __wait3_time64(int *status, int options, struct rusage *rusage) {
	int result = wait3(status, options, rusage);

	if (result != -1)
		ConvertRUsage32To64(rusage);

	return result;
}

POSSIBLY_UNDEFINED_ATTRIBUTE pid_t __wait4_time64(pid_t pid, int *status, int options, struct rusage *rusage) {
	int result = wait4(pid, status, options, rusage);

	if (result != -1)
		ConvertRUsage32To64(rusage);

	return result;
}
