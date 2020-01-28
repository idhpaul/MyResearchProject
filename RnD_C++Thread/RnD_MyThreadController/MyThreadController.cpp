#include "ThreadController.h"

#include <string>
#include <iostream>

int main()
{

	ThreadController myController(9999,3,8090,8091,8092);

	myController.WinSocketInit();

	myController.ThreadControllerStart();

	while (myController.RunCheck(5000));
	{
		
	}

	return 0;
}