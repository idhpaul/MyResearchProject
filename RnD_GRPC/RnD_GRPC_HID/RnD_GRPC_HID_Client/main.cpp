#include "MySDL.h"

#include <iostream>
#include <memory>

int main()
{

	std::unique_ptr<MySDL> sdlRender = std::make_unique<MySDL>(1280, 720, 1920, 1080);

	sdlRender->Create();
	if (!sdlRender->IsIntialized())
		return -1;

	sdlRender->Create_Injector(std::string("localhost"), std::string("8099"));

	// Main loop
	sdlRender->Process();

	return 0;
}