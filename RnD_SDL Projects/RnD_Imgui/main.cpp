#pragma comment(lib,"SDL2.lib")

#include "SDL.h"
#include "SDL_syswm.h"

#include "overlay.h"

#include <functional>
#include <iostream>
#include <random>			// test for rand sdl render color
#include <mutex>
#include <thread>

#include <Windows.h>

SDL_Window* window_ = nullptr;
HWND window_handle_ = nullptr;

std::string renderer_name_;
SDL_Renderer* renderer_ = nullptr;
SDL_Texture* texture_ = nullptr;

int gTotalDisplays = 0;
SDL_Rect* gDisplayBounds = NULL;

IDirect3DDevice9* device_ = nullptr;
SDL_GLContext gl_context_ = nullptr;

Overlay* overlay_ = nullptr;

int texture_format_ = SDL_PIXELFORMAT_UNKNOWN;
uint32_t texture_width_ = 0;
uint32_t texture_height_ = 0;

int window_width_ = 1280;
int window_height_ = 720;


bool SDL_Create();
void SDL_Destroy();

bool SDL_Init();
void SDL_Clear();

void SDL_Resize();

bool SDL_Create()
{
	static std::once_flag init_flag;
	std::call_once(init_flag, [=]() {
		SDL_assert(SDL_Init(SDL_INIT_EVERYTHING) == 0);
		});

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	// Create window with graphics context
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	int window_flag = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE;
	window_ = SDL_CreateWindow("SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		window_width_, window_height_, window_flag);
	SDL_GLContext gl_context = SDL_GL_CreateContext(window_);
	SDL_SysWMinfo wm_info;
	SDL_VERSION(&wm_info.version);
	if (SDL_GetWindowWMInfo(window_, &wm_info)) {
		window_handle_ = wm_info.info.win.window;
	}

	if (!SDL_Init()) {
		SDL_Destroy();
		return false;
	}

}

void SDL_Destroy()
{
	if (window_) {
		SDL_Clear();
		SDL_DestroyWindow(window_);
		window_ = nullptr;
		window_handle_ = nullptr;
		//SDL_Quit();
	}
}

bool SDL_Init()
{
	int driver_index = -1;
	int driver_count = SDL_GetNumRenderDrivers();
	int renderer_flags = SDL_RENDERER_SOFTWARE;

	renderer_name_ = "opengl"; //direct3d opengl

	for (int i = 0; i < driver_count; i++) {
		SDL_RendererInfo info;
		if (SDL_GetRenderDriverInfo(i, &info) < 0) {
			continue;
		}

		if (strcmp(info.name, renderer_name_.c_str()) == 0) {
			driver_index = i;
			if (info.flags & SDL_RENDERER_ACCELERATED) {
				renderer_flags = SDL_RENDERER_ACCELERATED;
			}
		}
	}

	if (driver_index < 0) {
		return false;
	}

	renderer_ = SDL_CreateRenderer(window_, driver_index, renderer_flags);
	SDL_assert(renderer_ != nullptr);


	SDL_SetRenderDrawColor(renderer_, 114, 144, 154, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer_);
	SDL_RenderPresent(renderer_);

	gTotalDisplays = SDL_GetNumVideoDisplays();
	if (gTotalDisplays < 2)
	{
		printf("Warning: Only one display connected!");
	}

	//Get bounds of each display
	gDisplayBounds = new SDL_Rect[gTotalDisplays];
	for (int i = 0; i < gTotalDisplays; ++i)
	{
		SDL_GetDisplayBounds(i, &gDisplayBounds[i]);
	}

	overlay_ = new Overlay;

	SDL_RendererInfo renderer_info;
	SDL_GetRendererInfo(renderer_, &renderer_info);

	bool ret = false;
	if (strcmp(renderer_info.name, "direct3d") == 0) {
		device_ = SDL_RenderGetD3D9Device(renderer_);
		SDL_assert(device_ != nullptr);
		ret = overlay_->Init(window_, device_);
	}
	else if (strcmp(renderer_info.name, "opengl") == 0) {
		gl_context_ = SDL_GL_CreateContext(window_);
		SDL_assert(gl_context_ != nullptr);
		SDL_GL_MakeCurrent(window_, gl_context_);
		SDL_GL_SetSwapInterval(1); // Enable vsync
		ret = overlay_->Init(window_, gl_context_);
	}

	if (!ret) {
		delete overlay_;
		overlay_ = nullptr;
	}

	overlay_->SetRect(0, 0, 500, 300);
	//overlay_->RegisterObserver(this);

	return true;
}
void SDL_Clear()
{
	if (overlay_) {
		overlay_->Destroy();
		delete overlay_;
		overlay_ = nullptr;
	}

	if (texture_) {
		SDL_DestroyTexture(texture_);
		texture_ = nullptr;
		texture_format_ = SDL_PIXELFORMAT_UNKNOWN;
		texture_width_ = 0;
		texture_height_ = 0;
	}

	if (renderer_) {
		if (gl_context_) {
			SDL_GL_DeleteContext(gl_context_);
			gl_context_ = nullptr;
		}

		SDL_DestroyRenderer(renderer_);
		renderer_ = nullptr;
	}
}

void SDL_Resize()
{
	if (window_) {
		int width = 0, height = 0;
		SDL_GetWindowSize(window_, &width, &height);

		window_width_ = width;
		window_height_ = height;
		//video_width_ = window_width_;
		//video_height_ = window_height_ - kMinOverlayHeight;
		//overlay_width_ = window_width_;
		//overlay_height_ = kMinOverlayHeight;

		SDL_Clear();
		SDL_Init();
	}
}

void test_render(bool& done)
{
	std::cout << "check point 2" << std::endl;

	if (texture_format_ != SDL_PIXELFORMAT_ARGB8888 ||
		(texture_width_ != window_width_) || (texture_height_ != window_width_)) {
		if (texture_) {
			SDL_DestroyTexture(texture_);
			texture_ = nullptr;
		}
	}

	if (!texture_) {
		texture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_STREAMING, window_width_, window_width_);
		SDL_assert(texture_ != nullptr);
		texture_format_ = SDL_PIXELFORMAT_ARGB8888;
		texture_width_ = window_width_;
		texture_height_ = window_width_;
	}

	if (texture_) {
		char* pixels = nullptr;
		int pitch = 0;

		int ret = SDL_LockTexture(texture_, nullptr, (void**)&pixels, &pitch);
		//SDL_assert(ret >= 0);
		if (ret < 0) {
			//return false;
		}

		//memcpy(pixels, data, texture_width_ * texture_height_ * 4);
		SDL_UnlockTexture(texture_);


		SDL_RenderClear(renderer_);

		SDL_Rect rect = { 0, 0, window_width_, window_width_ };
		//SDL_RenderCopy(renderer_, texture_, nullptr, &rect);

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> dis(0, SDL_ALPHA_OPAQUE);
		SDL_SetRenderDrawColor(renderer_, dis(gen), dis(gen), dis(gen), SDL_ALPHA_OPAQUE);
		if (overlay_)
		{
			overlay_->Render();
		}
		SDL_RenderPresent(renderer_);

	}
	std::cout << "check point 3" << std::endl;

}

using Test_Render = std::function<void(bool&)>;

int main()
{

	SDL_Create();

	Test_Render callbackRender = test_render;

	bool done = false;
	while (!done && window_ != nullptr) {
		SDL_Event event;
		if (SDL_WaitEvent(&event)) {
			//window.Porcess(event);
			//ImGui_ImplSDL2_ProcessEvent(&event);

			switch (event.type)
			{
			case SDL_WINDOWEVENT: {
				if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{
					std::cout << "Resize on" << std::endl;
					SDL_Resize();
				}
				break;
			}

			case SDL_QUIT: {
				done = true;
				break;
			}

			default: {
				break;
			}
			}
			std::cout << "check point 1" << std::endl;
			callbackRender(std::ref(done));
			std::cout << "check point 4" << std::endl;

		}
	}

	SDL_Destroy();

	return 0;
}