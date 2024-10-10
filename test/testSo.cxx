#include "test.hxx"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ios>
#include <iostream>

extern "C" int runSO(int argc, const char *argv[])
{
	bool quickExit = false;

	REQUIRE(strcmp(((const char *(*)(const char *))test::LibDL::Handle::Load()->GetSymbol("basename"))("test-dir/test.txt"), "test.txt") == 0);
	REQUIRE(test::LibDL::Handle::Load()->GetSymbol("fmemopen") == test::LibDL::Handle::Load()->GetSymbol("fmemopen", "GLIBC_2.22") || (test::LibC::GetVersion() > 0 && test::LibC::GetVersion() < 2.22));
	REQUIRE(test::LibRT::Sleep(100000000));
	REQUIRE(test::LibM::AreClose(test::LibM::Arctangent(1.0, 1.0), 0.78539816339744830961566084581988));
	REQUIRE(test::LibPThread::Run(10, [](void *i) { test::LibPThread::Lock lock(test::LibPThread::GetGlobalMutex()); std::cout.put(char('0' + std::uintptr_t(i))); return i; }, [](std::size_t i) { return reinterpret_cast<void *>(i); }));
	REQUIRE(test::LibPThread::TestFork());

	std::cout << std::endl << "Hello, world:";

	for (int i = 0; i < argc; i++) {
		std::cout << " " << argv[i];

		if (strcmp(argv[i], "quick_exit=yes") == 0)
			quickExit = true;
	}

	std::cout << std::endl <<
		" Version symbol lookup " << (test::LibDL::Handle::HasVersionLookup() ? "supported" : "unsupported") << " (libc " << test::LibC::GetVersion() << ")" << std::endl;

	if (!test::LibC::WithRandom([](std::size_t random) { std::cout << " Random: " << random << std::endl; }))
		std::cout << " Random functions unsupported" << std::endl;

	REQUIRE(test::LibC::DeallocatePage(test::LibC::AllocatePage()));

	REQUIRE(test::LibC::WithTruncatedFile([](ino_t inode, mode_t mode, uid_t uid, gid_t gid, off_t size, long long m_sec) {
		std::cout << " WithFile {inode: " << inode << ", mode: " << std::oct << mode << std::dec << ", uid: " << uid << ", gid: " << gid << ", size: " << size << ", modification time: " << m_sec << "}" << std::endl;
		REQUIRE(size == 1);
	}, "TestFile.txt", 1));

	REQUIRE(std::atexit([]{ std::cout << " Exiting..." << std::endl; }) == 0);
	REQUIRE(std::at_quick_exit([]{ std::cout << " Quick exiting..." << std::endl; }) == 0);

	if (quickExit)
		std::quick_exit(0);

	return 0;
}
