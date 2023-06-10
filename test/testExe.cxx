#include "test.hxx"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ios>
#include <iostream>

int main(int argc, const char *argv[])
{
	REQUIRE(test::LibRT::Sleep(100000000));
	REQUIRE(test::LibM::AreClose(test::LibM::Arctangent(1.0, 1.0), 0.78539816339744830961566084581988));
	REQUIRE(test::LibPThread::Run(10, [](void *i) { test::LibPThread::Lock lock(test::LibPThread::GetGlobalMutex()); std::cout.put(char('0' + std::uintptr_t(i))); return i; }, [](std::size_t i) { return reinterpret_cast<void *>(i); }));

	std::cout << std::endl << "Hello, world!" << std::endl;

	if (!test::LibC::WithRandom([](std::size_t random) { std::cout << " Random: " << random << std::endl; }))
		std::cout << " Random functions unsupported" << std::endl;

	REQUIRE(test::LibC::WithTruncatedFile([](ino_t inode, mode_t mode, uid_t uid, gid_t gid, off_t size, long long m_sec) {
		std::cout << " WithFile {inode: " << inode << ", mode: " << std::oct << mode << std::dec << ", uid: " << uid << ", gid: " << gid << ", size: " << size << ", modification time: " << m_sec << "}" << std::endl;
		REQUIRE(size == 1);
	}, "TestFile.txt", 1));

	REQUIRE(std::atexit([]{ std::cout << " Exiting..." << std::endl; }) == 0);
	REQUIRE(std::at_quick_exit([]{ std::cout << " Quick exiting..." << std::endl; }) == 0);
}
