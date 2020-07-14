#include "sha256.h"
#include <iostream>

int main()
{
	std::string PlainStr = "New1234!";

	std::string SHA256_InputStr = sha256(PlainStr);
	std::cout << "SHA256 Hash : " << SHA256_InputStr << std::endl;

	return 0;
}