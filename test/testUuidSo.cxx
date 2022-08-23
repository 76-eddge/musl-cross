#include <cstdint>
#include <iostream>

extern "C" void uuid_generate(std::uint8_t out[]);

static const char HEX_CHARS[] = "0123456789abcdef";

int runSO(int argc, const char *argv[])
{
	for (int i = 0; i < argc; i++)
	{
		std::uint8_t uuid[16];

		uuid_generate(uuid);
		std::cout << ' ' << argv[i] << ':';

		for (int j = 0; j < sizeof(uuid); j++)
			std::cout << HEX_CHARS[uuid[j] >> 4] << HEX_CHARS[uuid[j] & 0xF] << (j == 8 || j == 12 || j == 16 || j == 20 ? "-" : "");
	}

	return 0;
}
