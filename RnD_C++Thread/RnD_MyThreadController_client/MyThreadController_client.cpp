#include "ThreadController_Client.h"

#include <string>
#include <iostream>

int main()
{

	ThreadController_Client myController(9999, 3, "192.168.0.55", 8090, 8091, 8092);

	myController.WinSocketInit();

	myController.ControllerSocketStart();

	while (myController.RunCheck(5000));
	{
		
	}

	return 0;
}