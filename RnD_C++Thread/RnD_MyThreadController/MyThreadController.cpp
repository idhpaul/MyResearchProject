#include "ThreadController.h"

#include <string>
#include <iostream>

int main()
{

	ThreadController myController(3);

	myController.WinSocketInit();

	myController.ControllerSocketStart(9999);

	while (myController.RunCheck(5000));
	{
		
	}

	return 0;
}