#include <cstdint>
#include <memory>
#include <stdexcept>

extern "C" {
#include <dlfcn.h>
#include <math.h>
#include <pthread.h>
#include <time.h>
}

#define QUOTE(X) #X
#define REQUIRE(X) do { bool REQUIRE_result_ = bool(X); if (!REQUIRE_result_) throw std::runtime_error("Assertion failed: " QUOTE(X) " at " QUOTE(__FILE__) ":" QUOTE(__LINE__)); } while (false)

namespace test {

class LibDL {
public:
	class Handle {
		void *_handle;

		Handle(void *handle) : _handle(handle) { }

		Handle(const Handle &);
		Handle &operator=(const Handle &);

	public:
		~Handle() { (void)dlclose(_handle); }

		void *GetSymbol(const char *name) { return dlsym(_handle, name); }

		static std::unique_ptr<Handle> Load(const char *filename = nullptr) {
			void *handle = dlopen(filename, RTLD_LAZY);

			if (!handle)
				throw std::runtime_error(std::string("dlopen() failed: ") + dlerror());

			return std::unique_ptr<Handle>(new Handle(handle));
		}
	};
};

class LibM {
public:
	static double AbsoluteValue(double x) { return fabs(x); }
	static double Arctangent(double y, double x) { return atan2(y, x); }

	static bool AreClose(double a, double b) {
		return AbsoluteValue(a - b) < 0.0001;
	}
};

class LibPThread {
public:
	class Lock;

	class Mutex {
		friend class Lock;

		pthread_mutex_t _mutex;

	public:
		Mutex() : _mutex() { _mutex = PTHREAD_MUTEX_INITIALIZER; }
	};

	class Lock {
		Mutex &_mutex;

	public:
		Lock(Mutex &mutex) : _mutex(mutex) { pthread_mutex_lock(&_mutex._mutex); }
		~Lock() { pthread_mutex_unlock(&_mutex._mutex); }
	};

	static Mutex &GetGlobalMutex() {
		static Mutex mutex;
		return mutex;
	}

	static bool Run(std::size_t count, void *(*threadFn)(void *), void *(threadArgumentFn)(std::size_t)) {
		pthread_t *threads = new pthread_t[count];
		std::size_t threadCount;

		for (threadCount = 0; threadCount < count; threadCount++)
			if (pthread_create(&threads[threadCount], nullptr, threadFn, threadArgumentFn(threadCount)) != 0)
				break;

		for (std::size_t i = 0; i < threadCount; i++)
			(void)pthread_join(threads[i], nullptr);

		return threadCount == count;
	}
};

class LibRT {
public:
	static bool Sleep(std::uint64_t ns) {
		timespec time{0};
		time.tv_sec = static_cast<time_t>(ns / 1000000000);
		time.tv_nsec = static_cast<long>(ns % 1000000000);
		return clock_nanosleep(CLOCK_MONOTONIC, 0, &time, &time) == 0;
	}
};

} // test
