#include <cstdint>
#include <iostream>

extern "C" void uuid_generate(std::uint8_t out[]);

static const char HEX_CHARS[] = "0123456789abcdef";

extern "C" int runSO(int argc, const char *argv[])
{
	std::cout << "Hello, world!" << std::endl;

	for (int i = 0; i < argc; i++)
	{
		std::uint8_t uuid[16];

		uuid_generate(uuid);
		std::cout << ' ' << argv[i] << ": ";

		for (unsigned j = 0; j < sizeof(uuid) / sizeof(uuid[0]); j++)
			std::cout << HEX_CHARS[uuid[j] >> 4] << HEX_CHARS[uuid[j] & 0xF] << (j == 4 || j == 6 || j == 8 || j == 10 ? "-" : "");

		std::cout << std::endl;
	}

	return 0;
}
