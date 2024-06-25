#include <cstdint>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <string>

extern "C" {
#include <dlfcn.h>
#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
}

#define QUOTE_(X) #X
#define QUOTE(X) QUOTE_(X)
#define REQUIRE(X) do { bool REQUIRE_result_ = bool(X); if (!REQUIRE_result_) throw std::runtime_error("Assertion failed: " QUOTE(X) " at " __FILE__ ":" QUOTE(__LINE__)); } while (false)

namespace test {

class LibC {
public:
	static bool WithRandom(void (*withRandom)(std::size_t)) {
		std::size_t random = 0;

		if (getentropy(&random, sizeof(random)) != 0)
			return false;

		withRandom(random);
		return true;
	}

	static bool WithTruncatedFile(void (*withFile)(ino_t inode, mode_t mode, uid_t uid, gid_t gid, off_t size, long long m_sec), const char *filename, off_t size) {
		struct stat info;
		FILE *file = fopen(filename, "a");

		if (!file)
				throw std::runtime_error(std::string("fopen() failed: ") + strerror(errno));

		(void)fclose(file);

		if (truncate(filename, size) != 0)
				throw std::runtime_error(std::string("truncate() failed: ") + strerror(errno));

		if (stat(filename, &info) != 0)
			return false;

		withFile(info.st_ino, info.st_mode, info.st_uid, info.st_gid, info.st_size, info.st_mtime);
		return true;
	}

	static double GetVersion() {
		static void *getVersionFunction = nullptr;

		if (!getVersionFunction)
			getVersionFunction = dlsym(RTLD_DEFAULT, "gnu_get_libc_version");

		return !getVersionFunction ? 0 : std::atof(((const char*(*)())getVersionFunction)());
	}
};

class LibDL {
public:
	class Handle {
		typedef void *(SymbolLookupFunc)(void *handle, const char *name, const char *version);

		static void *LookupSymbol(void *handle, const char *name, const char*) { return dlsym(handle, name); }

		static SymbolLookupFunc *GetSymbolLookupFun() {
			static SymbolLookupFunc *symbolLookupFunc = nullptr;

			if (!symbolLookupFunc)
				symbolLookupFunc = reinterpret_cast<SymbolLookupFunc*>(dlsym(RTLD_DEFAULT, "dlvsym"));

			if (!symbolLookupFunc)
				symbolLookupFunc = LookupSymbol;

			return symbolLookupFunc;
		}

		void *_handle;

		Handle(void *handle) : _handle(handle) { }

		Handle(const Handle &);
		Handle &operator=(const Handle &);

	public:
		static bool HasVersionLookup() { return GetSymbolLookupFun() != LookupSymbol; }

		~Handle() { (void)dlclose(_handle); }

		void *GetSymbol(const char *name, const char *version = nullptr) { return version ? GetSymbolLookupFun()(_handle, name, version) : LookupSymbol(_handle, name, version); }

		static std::unique_ptr<Handle> Load(const char *filename = nullptr) {
			void *handle = dlopen(filename, RTLD_LAZY);

			if (!handle && filename)
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

private:
	enum ForkStatus {
		UNFORKED,
		PREPARING_TO_FORK,
		FORKED_AS_PARENT,
		FORKED_AS_CHILD
	};

	static ForkStatus &GetForkStatus() {
		static ForkStatus status = UNFORKED;
		return status;
	}

	// See glibc issue 16742:
	//  [16742] malloc: race condition: pthread_atfork() called before first
	//    malloc() results in unexpected locking behaviour/deadlocks
	static void *HackToFixGlibcAtForkBug() {
		return malloc(42);
	}

	static void OnPreparingToFork() {
		REQUIRE(GetForkStatus() == UNFORKED);
		GetForkStatus() = PREPARING_TO_FORK;
	}

	static void OnForkedAsParent() {
		REQUIRE(GetForkStatus() == PREPARING_TO_FORK);
		GetForkStatus() = FORKED_AS_PARENT;
	}

	static void OnForkedAsChild() {
		REQUIRE(GetForkStatus() == PREPARING_TO_FORK);
		GetForkStatus() = FORKED_AS_CHILD;
		_exit(0);
	}

public:
	static bool TestFork() {
		if (GetForkStatus() == UNFORKED) {
			(void)HackToFixGlibcAtForkBug();
			int error = pthread_atfork(OnPreparingToFork, OnForkedAsParent, OnForkedAsChild);

			if (error != 0)
				throw std::runtime_error(std::string("pthread_atfork() failed: ") + strerror(error));

			pid_t child = fork();

			if (child <= 0)
				throw std::runtime_error(std::string("fork() failed: ") + strerror(errno));

			int childStatus;

			if (waitpid(child, &childStatus, 0) != child)
				throw std::runtime_error(std::string("waitpid() failed: ") + strerror(errno));
			else if (!(WIFEXITED(childStatus)) || WEXITSTATUS(childStatus) != 0)
				throw std::runtime_error(std::string("bad fork()ed child exit status: ") + std::to_string(childStatus));
		}

		return GetForkStatus() == FORKED_AS_PARENT;
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
