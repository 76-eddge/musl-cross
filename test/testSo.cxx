#include <iostream>

int runSO(int argc, const char *argv[])
{
	for (int i = 0; i < argc; i++)
		std::cout << " " << argv[i];

	return 0;
}
