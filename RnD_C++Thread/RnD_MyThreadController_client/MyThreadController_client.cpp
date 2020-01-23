#include "ThreadController_Client.h"

#include <string>
#include <iostream>

int main()
{

	ThreadController_Client myController(3, "192.168.0.55");

	myController.WinSocketInit();

	myController.ControllerSocketStart(9999);

	while (myController.RunCheck(5000));
	{
		
	}

	return 0;
}