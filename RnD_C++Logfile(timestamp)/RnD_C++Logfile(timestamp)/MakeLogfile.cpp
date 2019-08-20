#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <algorithm>
#include <iomanip>
#include <sstream>

#include <direct.h> //mkdir

int main(void)
{

	char LogFileName[100];
	auto time = std::time(nullptr);
	std::stringstream ss;

	ss << std::put_time(std::localtime(&time), "%F_%p %I:%M:%S"); // ISO 8601 without timezone information.
	auto s = ss.str();
	std::replace(s.begin(), s.end(), ':', '-');

	std::cout << s << std::endl;

	int nResult = _mkdir("Logs");
	if (true == nResult)
	{
		std::cout << "make dir" << std::endl;
	}
	else
	{
		std::cout << "err make dir or already have dir" << std::endl;
	}

	sprintf(LogFileName, "Logs\\MSLM_LOG %s.txt", s.c_str());

	FILE *file = fopen(LogFileName, "a+");

	fwrite("1", 2, 1, file);

	fclose(file);

	return 0;
}