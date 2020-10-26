#pragma once
#ifndef OVERLAY_H
#define OVERLAY_H

#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"Opengl32.lib")

#include "SDL.h"

#include "imgui.h"
#include "impl/imgui_impl_sdl.h"
#include "impl/imgui_impl_dx9.h" 
#include "impl/imgui_impl_opengl3.h"

#include <GL/gl3w.h>            // Initialize with gl3wInit()

#include <d3d9.h>

#include <string>
class Overlay
{
public:
	Overlay();
	virtual ~Overlay();

	bool Init(SDL_Window* window, IDirect3DDevice9* device);
	bool Init(SDL_Window* window, SDL_GLContext gl_context);

	void SetRect(int x, int y, int w, int h);
	void Destroy();
	bool Render();

	static void Process(SDL_Event* event);

private:
	void Init();
	bool Copy();
	bool Begin();
	bool End();


	SDL_Window* window_ = nullptr;
	IDirect3DDevice9* device_ = nullptr;
	SDL_GLContext gl_context_ = nullptr;

	SDL_Rect rect_;

	/* debug info */
	std::string debug_info_text_;

};

#endif