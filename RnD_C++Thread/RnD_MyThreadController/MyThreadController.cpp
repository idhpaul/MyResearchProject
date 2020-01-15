#include "ThreadController.h"

#include <string>
#include <iostream>

int main()
{

	std::string input;

	ThreadController myController(3);

	myController.Start();

	while (true)
	{
		std::cout << "Input String : ";
		std::getline(std::cin, input);

		if (!input.compare("stop"))
		{
			myController.Stop();
			break;
		}

	}

	return 0;
}