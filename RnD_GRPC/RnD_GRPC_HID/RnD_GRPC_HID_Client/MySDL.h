#pragma once
#include <SDL.h>

#include <mutex>

typedef enum MyEvent
{
	MY_KEYUP,
	MY_KEYDOWN,
	MY_MOUSEBUTTONUP,
	MY_MOUSEBUTTONDOWN,
	MY_MOUSEMOTION,
	MY_MOUSEWHEEL,
	MY_WINDOWEVENT,
	MY_SYSWMEVENT,
	MY_REDNERTARGET_RESET,
	MY_RENDERDEVICE_RESET,
	MY_USEREVENT,
	MY_QUIT,

	MY_UNKOWN = -1
} MyEventType;

class MySDL
{
public:
	MySDL();
	MySDL(uint32_t wWidth, uint32_t wHeight, uint32_t tWidth, uint32_t tHeight);
	~MySDL();

	bool Create();
	bool Destory();

	bool Process();

	bool IsIntialized();


private:
	int (MySDL::*myFunctionList_[5][4])(const SDL_Event&);
	void SetFunctionPointList();

	int MyFunctionCaller(const uint32_t type, const SDL_Event& event);

	// My SDL Fuction
	int MyQuitFunction(const SDL_Event& sdlEvent);
	int MyWindowFunction(const SDL_Event& sdlEvent);
    int MySystemFuctnion(const SDL_Event& sdlEvent);
    int MyKeyDownFunction(const SDL_Event& sdlEvent);
    int MyKeyUPFunction(const SDL_Event& sdlEvent);
    int MyMouseMotionFunction(const SDL_Event& sdlEvent);
    int MyMouseButtonDownFunction(const SDL_Event& sdlEvent);
    int MyMouseButtonUpFunction(const SDL_Event& sdlEvent);
    int MyMouseWheelFunction(const SDL_Event& sdlEvent);
    int MyUserFunction(const SDL_Event& sdlEvent);


	std::once_flag sdlinit_;

	bool isInit_ = false;
	bool isFullScreen = false;
	bool isGrab = false;

	uint32_t windowWidth_;
	uint32_t windowHeight_;
	uint32_t textureWidth_;
	uint32_t textureHeight_;

	SDL_Window* window_ = nullptr;
	SDL_Renderer* renderer_ = nullptr;
	SDL_Texture* texture_ = nullptr;

	SDL_Event sdlEvent_;

};

