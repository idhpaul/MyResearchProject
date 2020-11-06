#include "overlay.h"
#include <iostream>
#include <mutex>

extern SDL_Rect* gDisplayBounds;

Overlay::Overlay()
{
}

Overlay::~Overlay()
{
	Destroy();
}

bool Overlay::Init(SDL_Window* window, IDirect3DDevice9* device)
{
	Init();

	if (gl_context_ || device_) {
		Destroy();
	}

	if (device) {
		ImGui_ImplSDL2_InitForD3D(window);
		ImGui_ImplDX9_Init(reinterpret_cast<IDirect3DDevice9*>(device));
		window_ = window;
		device_ = (IDirect3DDevice9*)device;
		return true;
	}

	return false;
}

bool Overlay::Init(SDL_Window* window, SDL_GLContext gl_context)
{
	Init();

	if (gl_context_ || device_) {
		Destroy();
	}

	if (gl_context) {
		static std::once_flag init_flag;
		std::call_once(init_flag, [=]() {
			gl3wInit();
			});

		const char* glsl_version = "#version 130";

		ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
		ImGui_ImplOpenGL3_Init(glsl_version);
		window_ = window;
		gl_context_ = gl_context;
		return true;
	}

	return false;
}

void Overlay::SetRect(int x, int y, int w, int h)
{
	rect_.x = x;
	rect_.y = y;
	rect_.w = w;
	rect_.h = h;
}

void Overlay::Destroy()
{
	window_ = nullptr;
	memset(&rect_, 0, sizeof(SDL_Rect));

	if (device_) {
		device_ = nullptr;
		ImGui_ImplDX9_Shutdown();
		ImGui_ImplSDL2_Shutdown();
	}

	if (gl_context_) {
		gl_context_ = nullptr;
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
	}
}

bool Overlay::Render()
{
	if (!Begin()) {
		return false;
	}

	Copy();
	End();
	return true;
}

void Overlay::Process(SDL_Event* event)
{
}

void Overlay::Init()
{
	static std::once_flag init_flag;
	std::call_once(init_flag, [=]() {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		//ImGui::StyleColorsDark();
		ImGui::StyleColorsLight();
		//ImGui::StyleColorsClassic();
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
		ImGui::GetStyle().AntiAliasedLines = false;
		ImGui::GetStyle().WindowRounding = 0;

		ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", 15.0f,NULL, ImGui::GetIO().Fonts->GetGlyphRangesKorean());
		});
}


bool Overlay::Copy()
{
	if (!rect_.w || !rect_.h) {
		return false;
	}

	//int widget_flag = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize ;
	//ImGui::SetNextWindowPos(ImVec2((float)rect_.x, (float)rect_.y), ImGuiCond_Always);
	//ImGui::SetNextWindowSize(ImVec2((float)rect_.w * 3 / 4, (float)rect_.h), ImGuiCond_Always);

	ImGuiIO& io = ImGui::GetIO();
	int widget_flag = /*ImGuiWindowFlags_NoDecoration |*/ ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	widget_flag |= ImGuiWindowFlags_NoMove;
	/*ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
	ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);*/
	ImGui::SetNextWindowPos(ImVec2((float)rect_.x, (float)rect_.y), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2((float)rect_.w * 3 / 4, (float)rect_.h), ImGuiCond_Always);
	ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background

	//ImGui::Begin("Stream Info", nullptr, widget_flag);

	////int input_flag = 0;
	////float start_x = 10.0, start_y = 20.0;

	/////* Encoder selection */
	////int encoder_index = 0;
	////ImGui::SetCursorPos(ImVec2(start_x, start_y));
	////ImGui::Text("Video Encoder: "); //ImGui::SameLine();
	////ImGui::SetCursorPos(ImVec2(start_x + 105, start_y - 3));
	//////ImGui::RadioButton("x264", &encoder_index_, 1); ImGui::SameLine(0, 10);
	//////ImGui::RadioButton("nvenc", &encoder_index_, 2); ImGui::SameLine(0, 10);
	//////ImGui::RadioButton("qsv", &encoder_index_, 3);

	////ImGui::SetCursorPos(ImVec2(start_x + 100, start_y + 28));
	////ImGui::Text(" framerate:");
	////ImGui::SetNextItemWidth(30);
	////ImGui::SetCursorPos(ImVec2(start_x + 180, start_y + 26));

	////ImGui::SameLine(0, 10);
	////ImGui::SetCursorPos(ImVec2(start_x + 225, start_y + 26));
	////ImGui::Text("bitrate(kbps):");
	////ImGui::SetNextItemWidth(80);
	////ImGui::SetCursorPos(ImVec2(start_x + 330, start_y + 26));
	//ImGui::End();

	if (!ImGui::Begin("Steam Info",nullptr, widget_flag))
	{
		ImGui::End();
		return false;
	}
 
    if (ImGui::BeginTabBar("##TabBar"))
    {
        if (ImGui::BeginTabItem("Video Info"))
        {
			if (ImGui::Button(u8"전체 화면"))
			{
				printf("전체 화면\n");

				int mWindowDisplayID;
				mWindowDisplayID = SDL_GetWindowDisplayIndex(window_);

#if _DEBUG
				int mHeight;
				int mWidth;
				SDL_GetWindowPosition(window_, &mWidth, &mHeight);


				std::cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << std::endl;
				std::cout << "Display x,y : " << mWidth << ", " << mHeight << std::endl;
				std::cout << "Display num : " << mWindowDisplayID << std::endl;

				std::cout << "gDisplayBounds[mWindowDisplayID].x and y : " << gDisplayBounds[mWindowDisplayID].x << ", " << gDisplayBounds[mWindowDisplayID].y << std::endl;
				std::cout << "gDisplayBounds[mWindowDisplayID].w and h : " << gDisplayBounds[mWindowDisplayID].w << ", " << gDisplayBounds[mWindowDisplayID].h << std::endl;

				std::cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << std::endl;
#endif

				SDL_SetWindowResizable(window_, SDL_FALSE);
				SDL_SetWindowSize(window_, 1920, 1080);
				SDL_SetWindowPosition(window_, gDisplayBounds[mWindowDisplayID].x, gDisplayBounds[mWindowDisplayID].y);
			}

			ImGui::SameLine();
			if (ImGui::Button(u8"창모드"))
			{
				printf("창모드\n");

				int mHeight;
				int mWidth;
				SDL_GetWindowPosition(window_, &mWidth, &mHeight);


				SDL_SetWindowResizable(window_, SDL_TRUE);
				SDL_SetWindowSize(window_, 1280, 720);
				SDL_SetWindowPosition(window_, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
			}

			ImGui::SameLine();

			ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Network Info"))
        {

			ImGui::Text("Network Bandwidth");
			ImGui::Separator();
			if (ImGui::IsMousePosValid())
				ImGui::Text("Bandwidth: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
			else
				ImGui::Text("Bandwidth: <invalid>");

			ImGui::EndTabItem();

        }

        ImGui::EndTabBar();
    }

	ImGui::End();

	return true;
}

bool Overlay::Begin()
{
	if ((!device_ && !gl_context_) || !window_) {
		return false;
	}

	if (device_) {
		ImGui_ImplDX9_NewFrame();
	}
	else if (gl_context_) {
		ImGui_ImplOpenGL3_NewFrame();
	}

	ImGui_ImplSDL2_NewFrame(window_);
	ImGui::NewFrame();
	return true;
}

bool Overlay::End()
{
	if ((!device_ && !gl_context_) || !window_) {
		return false;
	}

	ImGui::EndFrame();
	ImGui::Render();

	if (device_) {
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}
	else if (gl_context_) {
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	return true;
}
