#include <SDL.h>
#include <stdio.h>

#undef main

#define frameWidth 1920
#define frameHeight 1080

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;
SDL_Surface *surface;
SDL_Event event;
bool loop;

void updateScreen();
void handleEvent(SDL_Event *);

void show_flags(int flags) {

	printf("\nFLAGS ENABLED: ( %d )\n", flags);
	printf("=======================\n");
	if (flags & SDL_WINDOW_FULLSCREEN)         printf("SDL_WINDOW_FULLSCREEN\n");
	if (flags & SDL_WINDOW_OPENGL)             printf("SDL_WINDOW_OPENGL\n");
	if (flags & SDL_WINDOW_SHOWN)              printf("SDL_WINDOW_SHOWN\n");
	if (flags & SDL_WINDOW_HIDDEN)             printf("SDL_WINDOW_HIDDEN\n");
	if (flags & SDL_WINDOW_BORDERLESS)         printf("SDL_WINDOW_BORDERLESS\n");
	if (flags & SDL_WINDOW_RESIZABLE)          printf("SDL_WINDOW_RESIZABLE\n");
	if (flags & SDL_WINDOW_MINIMIZED)          printf("SDL_WINDOW_MINIMIZED\n");
	if (flags & SDL_WINDOW_MAXIMIZED)          printf("SDL_WINDOW_MAXIMIZED\n");
	if (flags & SDL_WINDOW_INPUT_GRABBED)      printf("SDL_WINDOW_INPUT_GRABBED\n");
	if (flags & SDL_WINDOW_INPUT_FOCUS)        printf("SDL_WINDOW_INPUT_FOCUS\n");
	if (flags & SDL_WINDOW_MOUSE_FOCUS)        printf("SDL_WINDOW_MOUSE_FOCUS\n");
	if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) printf("SDL_WINDOW_FULLSCREEN_DESKTOP\n");
	if (flags & SDL_WINDOW_FOREIGN)            printf("SDL_WINDOW_FOREIGN\n");
}

int main() {
	uint32_t wflags = 0;
	//wflags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	//wflags |= SDL_WINDOW_INPUT_GRABBED;
	//wflags |= SDL_WINDOW_INPUT_FOCUS; //윈도우가 활설화 여부(키보드 입력 및 다른 컨트롤러 입력도 있음)
	wflags |= SDL_WINDOW_MOUSE_FOCUS; //윈도우가 활성화 되어 있지 않은 경우에서도 마우스 윈도우 상에 있을지
	loop = true;

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_ShowCursor(SDL_ENABLE);

	window = SDL_CreateWindow(
		"Mouse coordinates test", 0, 0, frameWidth, frameHeight,
		wflags);

	SDL_Log("frameWidth %d frameHeight %d\n", frameWidth, frameHeight);
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	SDL_Log("window->w %d window->h %d\n", w, h);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	texture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		frameWidth, frameHeight);

	surface = SDL_CreateRGBSurface(0, frameWidth, frameHeight, 32, 0, 0, 0, 0);

	int i;
	for (i = 0; i < (frameWidth * frameHeight); i++) {
		*(((uint32_t*)surface->pixels) + i) = 0x00FF0000;
	}

	//SDL_SetWindowGrab(window, SDL_TRUE); //== window flags `SDL_WINDOW_INPUT_GRABBED`
	SDL_SetWindowResizable(window, SDL_TRUE);

	int flags = SDL_GetWindowFlags(window);
	//int window_shown = (flags & SDL_WINDOW_SHOWN) ? true : false;
	//int window_borderless = (flags & SDL_WINDOW_BORDERLESS) ? true : false;
	//int window_fullscreen = (flags & SDL_WINDOW_FULLSCREEN) ? true : false;

	show_flags(flags);


	int texturew, textureh;

	while (loop) {
		while (SDL_PollEvent(&event)) {
			handleEvent(&event);
		}

		updateScreen();

		SDL_QueryTexture(texture, NULL,NULL, &texturew, &textureh);
		printf("\ttexture width: %d, height: %d\n", texturew, textureh);

	}
	SDL_Log("Exiting now...\n");
	SDL_Quit();
	return 0;
}

void handleEvent(SDL_Event *event) {
	switch (event->type) {
	case SDL_KEYDOWN:
		//loop = false;
		//break;
	case SDL_MOUSEMOTION:
		if (event->motion.x > frameWidth) {
			//SDL_WarpMouseInWindow(window, frameWidth, event->motion.y);
		}

		if (event->motion.y > frameHeight) {
			//SDL_WarpMouseInWindow(window, event->motion.x, frameHeight);
		}

		SDL_Log("motion.x = %d, motion.y = %d\n", event->motion.x, event->motion.y);
		break;
	default:
		break;
	}
}

void updateScreen() {
	SDL_UpdateTexture(texture, NULL, surface->pixels, frameWidth * sizeof(Uint32));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}