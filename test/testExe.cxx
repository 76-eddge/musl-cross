#include "test.hxx"

#include <cstdint>
#include <cstring>
#include <iostream>

int main(int argc, const char *argv[])
{
	REQUIRE(test::LibRT::Sleep(100000000));
	REQUIRE(test::LibM::AreClose(test::LibM::Arctangent(1.0, 1.0), 0.78539816339744830961566084581988));
	REQUIRE(test::LibPThread::Run(10, [](void *i) { test::LibPThread::Lock lock(test::LibPThread::GetGlobalMutex()); std::cout.put(char('0' + std::uintptr_t(i))); return i; }, [](std::size_t i) { return reinterpret_cast<void *>(i); }));

	std::cout << std::endl << "Hello, world!" << std::endl;

	if (!test::LibC::WithRandom([](std::size_t random) { std::cout << " Random: " << random << std::endl; }))
		std::cout << " Random functions unsupported" << std::endl;
}
