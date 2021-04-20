#include "Render_D11.h"

#include <iostream>

#include <string.h>

#if _DEBUG
#define ALLOW_DEBUG_PRINT 1
#endif

RenderD11::RenderD11()
	: window_width_(1280), window_height_(720), texture_width_(1920), texture_height_(1080)
{
	std::cout << "Render Constructor" << std::endl;

	SetFunctionPointList();

}

RenderD11::RenderD11(int window_width, int window_heigth, int texture_width, int texture_height)
	: window_width_(window_width), window_height_(window_heigth), texture_width_(texture_width), texture_height_(texture_height)
{
	SetFunctionPointList();
}


RenderD11::~RenderD11()
{
	std::cout << "Render Destructor" << std::endl;

	Finalize();
}

int32_t RenderD11::Initialize(RenderDriverType driver_type)
{
	std::call_once(sdl_init_flag_, [=]() {
		SDL_assert(SDL_Init(SDL_INIT_EVERYTHING) == 0);
		});

	if (QueryDriver(driver_type) < 0)
		return -1;

	if (CreateSDLWindow() < 0)
		return -2;

	if (CreateSDLRender() < 0)
		return -3;

	if (CreateSDLTexture() < 0)
		return -4;

	return 0;
}

int32_t RenderD11::Finalize()
{

	if (DestroySDLTexture() < 0)
		return -1;

	if (DestroySDLRender() < 0)
		return -1;

	if (DestroySDLWindow() < 0)
		return -1;

	SDL_Quit();

	return 0;
}

int32_t RenderD11::Render(uint8_t* out_buf, uint32_t width, uint32_t height)
{
	uint8_t* texture_data = nullptr;
	int texture_pitch = 0;

	SDL_UpdateTexture(texture_, NULL, out_buf, width);

	SDL_RenderClear(renderer_);
	SDL_RenderCopy(renderer_, texture_, nullptr, nullptr);

	SDL_RenderPresent(renderer_);

	return 0;
}

int32_t RenderD11::GetDesktopFrame(int32_t timeout)
{
	return int32_t();
}


int32_t RenderD11::Procedd_wait(int fps)
{
	fps_ = fps;
	int64_t timoutMillis = 1000 / 60;

	FpsInit();

	int reslut = 0;
	while (reslut >= 0)
	{
		if (SDL_WaitEvent(&sdlEvent_))
		{
			reslut = FunctionCaller(sdlEvent_.type, sdlEvent_);


			FpsCount();
			std::string fpsStr = "RX / FPS:" + std::to_string(framespersecond_) + "fps";
			SDL_SetWindowTitle(window_, fpsStr.c_str());

			SDL_Delay(timoutMillis);
		}
		else
		{
			std::cout << "SDL Wait Event failed" << std::endl;
			return false;
		}
	}

	return true;
}

int32_t RenderD11::Procedd_poll(int fps)
{
	fps_ = 1000 / fps;

	FpsInit();

	int reslut = 0;
	while (reslut >= 0)
	{
		if (SDL_PollEvent(&sdlEvent_))
		{
			reslut = FunctionCaller(sdlEvent_.type, sdlEvent_);
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		auto startProcceed = SDL_GetTicks();
		

		//Render(&nv12_texture[0], 1920, 1080);

		auto endProcceed = SDL_GetTicks();
		auto totalProccedd = (endProcceed - startProcceed);
		//printf("total Proccedd time : %d\n", totalProccedd);
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		FpsCount();
		std::string fpsStr = "RX / FPS : " + std::to_string(framespersecond_) + "fps";
		SDL_SetWindowTitle(window_, fpsStr.c_str());

		int delayTime = fps_ - totalProccedd;
		std::cout << "delay time : " << delayTime << std::endl;

		if(delayTime > 0)
			SDL_Delay(delayTime);
	}

	FpsFinalize();

	return true;
}

void RenderD11::FpsInit()
{
	// Set all frame times to 0ms.
	frametimes_ = new Uint32[fps_];

	memset(frametimes_, 0, sizeof(Uint32)*fps_);
	framecount_ = 0;
	framespersecond_ = 0;
	frametimelast_ = SDL_GetTicks();
}

void RenderD11::FpsCount()
{
	Uint32 frametimesindex;
	Uint32 getticks;
	Uint32 count;
	Uint32 i;

	// frametimesindex is the position in the array. It ranges from 0 to FRAME_VALUES.
	// This value rotates back to 0 after it hits FRAME_VALUES.
	frametimesindex = framecount_ % fps_;

	// store the current time
	getticks = SDL_GetTicks();

	// save the frame time value
	frametimes_[frametimesindex] = getticks - frametimelast_;

	// save the last frame time for the next fpsthink
	frametimelast_ = getticks;

	// increment the frame count
	framecount_++;

	// Work out the current framerate

	// The code below could be moved into another function if you don't need the value every frame.

	// I've included a test to see if the whole array has been written to or not. This will stop
	// strange values on the first few (FRAME_VALUES) frames.
	if (framecount_ < fps_) {

		count = framecount_;

	}
	else {

		count = fps_;

	}

	// add up all the values and divide to get the average frame time.
	framespersecond_ = 0;
	for (i = 0; i < count; i++) {

		framespersecond_ += frametimes_[i];

	}

	framespersecond_ /= count;

	// now to make it an actual frames per second value...
	framespersecond_ = 1000.f / framespersecond_;
}

void RenderD11::FpsFinalize()
{
	delete[] frametimes_;
}

int32_t RenderD11::CreateSDLWindow()
{

	int window_flag = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

	if (render_driver_type_ == "opengl")
		window_flag |= SDL_WINDOW_OPENGL;

	window_ = SDL_CreateWindow("RX", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width_, window_height_, window_flag);
	if (window_ == NULL)
	{
		std::cout << "Failed to create sdl window" << std::endl;
		return -1;
	}

	return 0;
}

int32_t RenderD11::CreateSDLRender()
{
	int driver_index = -1;
	int driver_count = SDL_GetNumRenderDrivers();
	int renderer_flags = SDL_RENDERER_SOFTWARE;

	for (int i = 0; i < driver_count; i++)
	{
		SDL_RendererInfo info;
		if (SDL_GetRenderDriverInfo(i, &info) < 0)
		{
			continue;
		}

		if (strcmp(info.name, render_driver_type_.c_str()) == 0) {
			driver_index = i;
			if (info.flags & SDL_RENDERER_ACCELERATED)
			{
				renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
			}
		}
	}

	if (driver_index < 0)
	{
		return false;
	}

	renderer_ = SDL_CreateRenderer(window_, driver_index, renderer_flags);
	SDL_assert(renderer_ != nullptr);

	SDL_SetRenderDrawColor(renderer_, 114, 144, 154, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer_);
	SDL_RenderPresent(renderer_);

	return 0;
}

int32_t RenderD11::CreateSDLTexture()
{
	int textureflag = SDL_TEXTUREACCESS_STATIC | SDL_TEXTUREACCESS_TARGET;
	texture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_NV12, textureflag, texture_width_, texture_height_);

	return 0;
}

int32_t RenderD11::DestroySDLWindow()
{
	if (window_)
	{
		SDL_DestroyWindow(window_);
		window_ = nullptr;
	}

	return 0;
}

int32_t RenderD11::DestroySDLRender()
{
	if (renderer_)
	{
		SDL_DestroyRenderer(renderer_);
		renderer_ = nullptr;
	}

	return 0;
}

int32_t RenderD11::DestroySDLTexture()
{
	if (texture_)
	{
		SDL_DestroyTexture(texture_);
		texture_ = nullptr;
	}

	return 0;
}

int32_t RenderD11::QueryDriver(RenderDriverType driver_type)
{

	std::string choicetype;

	switch (driver_type) {
	case DXD9:
		choicetype = "direct3d";
		break;
	case DXD11:
		choicetype = "direct3d11";
		break;
	case OPENGL:
		choicetype = "opengl";
		break;
	}

	if (choicetype.empty())
		return -1;

	int driver_count = SDL_GetNumRenderDrivers();
	int renderer_flags = SDL_RENDERER_SOFTWARE;

	for (int i = 0; i < driver_count; i++)
	{
		SDL_RendererInfo info;
		if (SDL_GetRenderDriverInfo(i, &info) < 0)
		{
			continue;
		}

		std::cout << "Query Driver name : " << info.name << std::endl;

		if (info.flags & SDL_RENDERER_SOFTWARE)
		{
			std::cout << "Support SDL_RENDERER_SOFTWARE" << std::endl;
		}
		if (info.flags & SDL_RENDERER_ACCELERATED)
		{
			std::cout << "Support SDL_RENDERER_ACCELERATED" << std::endl;
		}
		if (info.flags & SDL_RENDERER_PRESENTVSYNC)
		{
			std::cout << "Support SDL_RENDERER_PRESENTVSYNC" << std::endl;
		}
		if (info.flags & SDL_RENDERER_TARGETTEXTURE)
		{
			std::cout << "Support SDL_RENDERER_TARGETTEXTURE" << std::endl;
		}

		if (!strcmp(info.name, choicetype.c_str()))
		{
			std::cout << "Set render driver " << choicetype << std::endl;

			render_driver_type_ = choicetype;

			break;
		}

	}

	// Set SDL HW env
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2"))
	{
		printf("Warning: Linear texture filtering not enabled!");
	}


	if (render_driver_type_.empty())
		return -1;

	return 0;
}

inline void RenderD11::GetWindowSize(int* width, int* hegith)
{
	SDL_GetWindowSize(window_, width, hegith);
}

void RenderD11::SetFunctionPointList()
{
	memset(functionList_, 0, sizeof(functionList_[0][0]) * 6 * 4);

	functionList_[0][0] = &RenderD11::QuitFunction;

	functionList_[1][0] = &RenderD11::WindowFunction;
	functionList_[1][1] = &RenderD11::SystemFuctnion;

	functionList_[2][0] = &RenderD11::KeyDownFunction;
	functionList_[2][1] = &RenderD11::KeyUPFunction;

	functionList_[3][0] = &RenderD11::MouseMotionFunction;
	functionList_[3][1] = &RenderD11::MouseButtonDownFunction;
	functionList_[3][2] = &RenderD11::MouseButtonUpFunction;
	functionList_[3][3] = &RenderD11::MouseWheelFunction;

	functionList_[4][0] = &RenderD11::ClipBoardFunction;

	functionList_[5][0] = &RenderD11::UserFunction;
}

int RenderD11::FunctionCaller(const uint32_t type, const SDL_Event& event)
{

	unsigned int groupIndex = type >> 8;
	if (groupIndex == 0b0000'1001)
		groupIndex = 5;                             // for SDL Clipboard Event

	if (groupIndex == 0b1000'0000)
		groupIndex = 6;                             // for SDL User Event

	if (groupIndex > 7)                             // Boundary out
		return 0;

	unsigned int subIndex = type & 0b0000'1111;     // C++14 binary literal type

	if (!functionList_[groupIndex - 1][subIndex])
		return 0;

	return (this->*functionList_[groupIndex - 1][subIndex])(event);
}

int RenderD11::QuitFunction(const SDL_Event& sdlEvent)
{
	std::cout << "Quit func called" << std::endl;
	return -1;
}

int RenderD11::WindowFunction(const SDL_Event& sdlEvent)
{
	switch (sdlEvent.window.event) {
	case SDL_WINDOWEVENT_SHOWN:
		SDL_Log("Window %d shown", sdlEvent.window.windowID);
		break;
	case SDL_WINDOWEVENT_HIDDEN:
		SDL_Log("Window %d hidden", sdlEvent.window.windowID);
		break;
	case SDL_WINDOWEVENT_EXPOSED:
		SDL_Log("Window %d exposed", sdlEvent.window.windowID);
		break;
	case SDL_WINDOWEVENT_MOVED:
		SDL_Log("Window %d moved to %d,%d", sdlEvent.window.windowID, sdlEvent.window.data1, sdlEvent.window.data2);
		break;
	case SDL_WINDOWEVENT_RESIZED:
		SDL_Log("Window %d resized to %dx%d", sdlEvent.window.windowID, sdlEvent.window.data1, sdlEvent.window.data2);
		break;
	case SDL_WINDOWEVENT_SIZE_CHANGED:
		SDL_Log("Window %d size changed to %dx%d", sdlEvent.window.windowID, sdlEvent.window.data1, sdlEvent.window.data2);
		break;
	case SDL_WINDOWEVENT_MINIMIZED:
		SDL_Log("Window %d minimized", sdlEvent.window.windowID);
		break;
	case SDL_WINDOWEVENT_MAXIMIZED:
		SDL_Log("Window %d maximized", sdlEvent.window.windowID);
		break;
	case SDL_WINDOWEVENT_RESTORED:
		SDL_Log("Window %d restored", sdlEvent.window.windowID);
		break;
	case SDL_WINDOWEVENT_ENTER:
		SDL_Log("Mouse entered window %d", sdlEvent.window.windowID);
		break;
	case SDL_WINDOWEVENT_LEAVE:
		SDL_Log("Mouse left window %d", sdlEvent.window.windowID);
		break;
	case SDL_WINDOWEVENT_FOCUS_GAINED:
		SDL_Log("Window %d gained keyboard focus", sdlEvent.window.windowID);
		isKeyboardFocus = true;
		break;
	case SDL_WINDOWEVENT_FOCUS_LOST:
		SDL_Log("Window %d lost keyboard focus", sdlEvent.window.windowID);
		isKeyboardFocus = false;;
		break;
	case SDL_WINDOWEVENT_CLOSE:
		SDL_Log("Window %d closed", sdlEvent.window.windowID);
		break;
	case SDL_WINDOWEVENT_TAKE_FOCUS:
		SDL_Log("Window %d is offered a focus", sdlEvent.window.windowID);
		//_IsSDLKeyboardFocus = true;
		break;
	case SDL_WINDOWEVENT_HIT_TEST:
		SDL_Log("Window %d has a special hit test", sdlEvent.window.windowID);
		break;
	default:
		SDL_Log("Window %d got unknown event %d",
			sdlEvent.window.windowID, sdlEvent.window.event);
		break;
	}
	return 0;
}

int RenderD11::SystemFuctnion(const SDL_Event& sdlEvent)
{
	return 0;
}

int RenderD11::KeyDownFunction(const SDL_Event& sdlEvent)
{
#if ALLOW_DEBUG_PRINT
	std::cout << "SDL Get Key Down" << std::endl;
	printf("key code : %x\n", sdlEvent.key.keysym.unused);
	printf("key extend : %x\n", sdlEvent.key.keysym.mod);
#endif

	if ((sdlEvent.key.keysym.mod & KMOD_CTRL) &&
		(sdlEvent.key.keysym.mod & KMOD_ALT) &&
		(sdlEvent.key.keysym.mod & KMOD_SHIFT))
	{

		// Check for quit combo (Ctrl+Alt+Shift+Q)
		if (sdlEvent.key.keysym.sym == SDLK_RETURN) {
			SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
				"Detected Change Screen Mode");

			if (isFullScreen)
			{
				SDL_SetWindowFullscreen(window_, 0);
				isFullScreen = false;
			}
			else
			{
				SDL_SetWindowFullscreen(window_, SDL_WINDOW_FULLSCREEN_DESKTOP);
				isFullScreen = true;
			}
		}
	}

	if ((sdlEvent.key.keysym.mod & KMOD_CTRL) &&
		(sdlEvent.key.keysym.mod & KMOD_ALT) &&
		(sdlEvent.key.keysym.mod & KMOD_SHIFT))
	{

		// Check for quit combo (Ctrl+Alt+Shift+Q)
		if (sdlEvent.key.keysym.sym == SDLK_SPACE) {
			SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
				"Detected Change Grab");

			if (isGrab)
			{
				SDL_SetWindowGrab(window_, SDL_FALSE);
				isGrab = false;
			}
			else
			{
				SDL_SetWindowGrab(window_, SDL_TRUE);
				isGrab = true;
			}
		}
	}
	return 0;
}

int RenderD11::KeyUPFunction(const SDL_Event& sdlEvent)
{
#if ALLOW_DEBUG_PRINT
	std::cout << "SDL Get Key Up" << std::endl;
	printf("key code : %x\n", sdlEvent.key.keysym.unused);
	printf("key extend : %x\n", sdlEvent.key.keysym.mod);
#endif

	return 0;
}

int RenderD11::MouseMotionFunction(const SDL_Event& sdlEvent)
{
	int relativew = 0, relativeh = 0;
	GetWindowSize(&relativew, &relativeh);

#if ALLOW_DEBUG_PRINT
	std::cout << "Mouse move : " << (sdlEvent.motion.x * texture_width_) / relativew << "x" << (sdlEvent.motion.y * texture_height_) / relativeh << std::endl;
#endif

	return 0;
}

int RenderD11::MouseButtonDownFunction(const SDL_Event& sdlEvent)
{
	int relativew = 0, relativeh = 0;
	GetWindowSize(&relativew, &relativeh);

	//std::cout << "Mouse bt Down" << std::endl;


	return 0;
}

int RenderD11::MouseButtonUpFunction(const SDL_Event& sdlEvent)
{
	int relativew = 0, relativeh = 0;
	GetWindowSize(&relativew, &relativeh);

	//std::cout << "Mouse bt Up" << std::endl;

	return 0;
}

int RenderD11::MouseWheelFunction(const SDL_Event& sdlEvent)
{
	int relativew = 0, relativeh = 0;
	GetWindowSize(&relativew, &relativeh);

	if (sdlEvent.wheel.y > 0)
	{
#if ALLOW_DEBUG_PRINT
		std::cout << "Mouse wheel Up" << std::endl;
#endif
	}
	else
	{
#if ALLOW_DEBUG_PRINT
		std::cout << "Mouse wheel Down" << std::endl;
#endif
	}

	return 0;
}

int RenderD11::ClipBoardFunction(const SDL_Event& sdlEvent)
{
#if ALLOW_DEBUG_PRINT
	std::cout << "Clipboard called" << std::endl;
#endif

	const char* clipboard_data = SDL_GetClipboardText();

	SDL_free((void*)clipboard_data);

	return 0;
}

int RenderD11::UserFunction(const SDL_Event& sdlEvent)
{
#if ALLOW_DEBUG_PRINT
	std::cout << "User called" << std::endl;
#endif

	return 0;
}