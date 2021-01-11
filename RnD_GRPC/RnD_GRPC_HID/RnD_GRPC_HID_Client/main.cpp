#include "MySDL.h"

#include <iostream>
#include <memory>

int main()
{


	uint32_t group = 0;

	/*group |= 0b0000'0000'0000'0000'0000'0000'0000'0001;
	group |= 0b0000'0000'0000'0000'0000'0000'0001'0000;
	group |= 0b0000'0000'0000'0000'0000'0001'0000'0000;
	group |= 0b0000'0000'0000'0000'0001'0000'0000'0000;*/

	group |= 0x1;
	group |= 0x10;
	group |= 0x100;
	group |= 0x1000;

	std::cout << group << std::endl;

	std::unique_ptr<MySDL> sdlRender = std::make_unique<MySDL>(1280, 720, 1920, 1080);

	sdlRender->Create();
	if (!sdlRender->IsIntialized())
		return -1;

	// Main loop
	sdlRender->Process();

	return 0;
}