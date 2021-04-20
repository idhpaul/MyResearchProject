#pragma once
#include <SDL.h>

#include <functional>
#include <mutex>
#include <string>

#include <stdint.h>

enum RenderDriverType
{
	DXD9 = 0,
	DXD11 = 1,
	OPENGL = 2,
	SOFTWARE = 3
};

class RenderD11
{
public:
	RenderD11();
	RenderD11(int window_width, int window_heigth, int texture_width, int texture_height);
	~RenderD11();

	int32_t Initialize(RenderDriverType driver_type);
	int32_t Finalize();

	int32_t Procedd_wait(int fps);
	int32_t Procedd_poll(int fps);

	int32_t Render(uint8_t* out_buf, uint32_t width, uint32_t height);

	int32_t GetDesktopFrame(int32_t timeout);

private:

	//ref : http://sdl.beuc.net/sdl.wiki/SDL_Average_FPS_Measurement
	void FpsInit();
	void FpsCount();
	void FpsFinalize();

	int32_t CreateSDLWindow();
	int32_t CreateSDLRender();
	int32_t CreateSDLTexture();

	int32_t DestroySDLWindow();
	int32_t DestroySDLRender();
	int32_t DestroySDLTexture();

	int32_t QueryDriver(RenderDriverType driver_type);

	inline void GetWindowSize(int* width, int* hegith);


	int (RenderD11::* functionList_[6][4])(const SDL_Event&);
	void SetFunctionPointList();

	int FunctionCaller(const uint32_t type, const SDL_Event& event);

	// My SDL Fuction
	int QuitFunction(const SDL_Event& sdlEvent);
	int WindowFunction(const SDL_Event& sdlEvent);
	int SystemFuctnion(const SDL_Event& sdlEvent);
	int KeyDownFunction(const SDL_Event& sdlEvent);
	int KeyUPFunction(const SDL_Event& sdlEvent);
	int MouseMotionFunction(const SDL_Event& sdlEvent);
	int MouseButtonDownFunction(const SDL_Event& sdlEvent);
	int MouseButtonUpFunction(const SDL_Event& sdlEvent);
	int MouseWheelFunction(const SDL_Event& sdlEvent);
	int ClipBoardFunction(const SDL_Event& sdlEvent);
	int UserFunction(const SDL_Event& sdlEvent);

	std::once_flag sdl_init_flag_;

	int window_width_;
	int window_height_;
	int texture_width_;
	int texture_height_;

	std::string render_driver_type_;

	SDL_Window* window_ = nullptr;
	SDL_Renderer* renderer_ = nullptr;
	SDL_Texture* texture_ = nullptr;

	bool isInit_ = false;
	bool isFullScreen = false;
	bool isGrab = false;
	bool isKeyboardFocus = false;

	SDL_Event sdlEvent_;

	int fps_;
	Uint32* frametimes_;	// An array to store frame times:
	Uint32 frametimelast_;				// Last calculated SDL_GetTicks
	Uint32 framecount_;					// total frames rendered
	float framespersecond_;				// the value you want


};