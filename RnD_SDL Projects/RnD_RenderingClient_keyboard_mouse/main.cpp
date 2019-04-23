#include <stdio.h>

#include "SDL.h"

#include <Windows.h>
#pragma comment(lib,"ws2_32.lib")

#ifdef main
#undef main
#endif

#define	SDL_USEREVENT_CREATE_OVERLAY	0x0001
#define	SDL_USEREVENT_OPEN_AUDIO		0x0002
#define	SDL_USEREVENT_RENDER_IMAGE		0x0004
#define	SDL_USEREVENT_RENDER_TEXT		0x0008

const Uint8 *keyboard_state_array = SDL_GetKeyboardState(NULL);

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;


#define DELTA_EPOCH_IN_USEC	11644473600000000Ui64
// save files
static FILE *savefp_keyts = NULL;

static FILE *
ga_save_init_internal(const char *filename, const char *mode) {
	FILE *fp = NULL;
	if (filename != NULL) {
		fp = fopen(filename, mode);
		if (fp == NULL) {
			printf("save file: open %s failed.\n", filename);
		}
	}
	return fp;
}
FILE *
ga_save_init_txt(const char *filename) {
	return ga_save_init_internal(filename, "wt");
}
int
ga_save_close(FILE *fp) {
	if (fp != NULL) {
		fclose(fp);
	}
	return 0;
}

static unsigned __int64
filetime_to_unix_epoch(const FILETIME *ft) {
	unsigned __int64 res = (unsigned __int64)ft->dwHighDateTime << 32;
	res |= ft->dwLowDateTime;
	res /= 10;                   /* from 100 nano-sec periods to usec */
	res -= DELTA_EPOCH_IN_USEC;  /* from Win epoch to Unix epoch */
	return (res);
}

int
gettimeofday(struct timeval *tv, void *tz) {
	FILETIME  ft;
	unsigned __int64 tim;
	if (!tv) {
		//errno = EINVAL;
		return (-1);
	}
	GetSystemTimeAsFileTime(&ft);
	tim = filetime_to_unix_epoch(&ft);
	tv->tv_sec = (long)(tim / 1000000L);
	tv->tv_usec = (long)(tim % 1000000L);
	return (0);
}

int
ga_save_printf(FILE *fp, const char *fmt, ...) {
	int wlen;
	va_list ap;
	if (fp == NULL)
		return -1;
	va_start(ap, fmt);
	wlen = vfprintf(fp, fmt, ap);
	va_end(ap);
	fflush(fp);
	return wlen;
}


#define	SDL_EVENT_MSGTYPE_NULL		0
#define	SDL_EVENT_MSGTYPE_KEYBOARD	1
#define	SDL_EVENT_MSGTYPE_MOUSEKEY	2
#define SDL_EVENT_MSGTYPE_MOUSEMOTION	3
#define SDL_EVENT_MSGTYPE_MOUSEWHEEL	4

struct sdlmsg_s {
	unsigned short msgsize;		// size of this data-structure
					// every message MUST start from a
					// unsigned short message size
					// the size includes the 'msgsize'
	unsigned char msgtype;
	unsigned char which;
	unsigned char padding[60];	// must be large enough to fit
					// all supported type of messages
};
typedef struct sdlmsg_s			sdlmsg_t;

// keyboard event
struct sdlmsg_keyboard_s {
	unsigned short msgsize;
	unsigned char msgtype;		// SDL_EVENT_MSGTYPE_KEYBOARD
	unsigned char which;
	unsigned char is_pressed;
	unsigned char unused0;
	unsigned short scancode;	// scancode
	int sdlkey;			// SDLKey
	unsigned int unicode;		// unicode or ASCII value
	unsigned short sdlmod;		// SDLMod
};
typedef struct sdlmsg_keyboard_s	sdlmsg_keyboard_t;

// mouse event
struct sdlmsg_mouse_s {
	unsigned short msgsize;
	unsigned char msgtype;		// SDL_EVENT_MSGTYPE_MOUSEKEY
					// SDL_EVENT_MSGTYPE_MOUSEMOTION
					// SDL_EVENT_MSGTYPE_MOUSEWHEEL
	unsigned char which;
	unsigned char is_pressed;	// for mouse button
	unsigned char mousebutton;	// mouse button
	unsigned char mousestate;	// mouse stat
	unsigned char relativeMouseMode;
	unsigned short mousex;
	unsigned short mousey;
	unsigned short mouseRelX;
	unsigned short mouseRelY;
};
typedef struct sdlmsg_mouse_s		sdlmsg_mouse_t;

#define	VIDEO_SOURCE_CHANNEL_MAX	2
static int windowSizeX[VIDEO_SOURCE_CHANNEL_MAX];
static int windowSizeY[VIDEO_SOURCE_CHANNEL_MAX];
// support resizable window
static int nativeSizeX[VIDEO_SOURCE_CHANNEL_MAX];
static int nativeSizeY[VIDEO_SOURCE_CHANNEL_MAX];

static int
xlat_mouseX(int ch, int x) {
	return (1.0 * nativeSizeX[ch] / windowSizeX[ch]) * x;
}

static int
xlat_mouseY(int ch, int y) {
	return (1.0 * nativeSizeY[ch] / windowSizeY[ch]) * y;
}


sdlmsg_t *
sdlmsg_keyboard(sdlmsg_t *msg, unsigned char pressed, unsigned short scancode, SDL_Keycode key, unsigned short mod, unsigned int unicode)
{
	sdlmsg_keyboard_t *msgk = (sdlmsg_keyboard_t*)msg;
	//ga_error("sdl client: key event code=%x key=%x mod=%x pressed=%u\n", scancode, key, mod, pressed);
	ZeroMemory(msg, sizeof(sdlmsg_keyboard_t));
	msgk->msgsize = htons(sizeof(sdlmsg_keyboard_t));
	msgk->msgtype = SDL_EVENT_MSGTYPE_KEYBOARD;
	msgk->is_pressed = pressed;
#if 1	// only support SDL2
	msgk->scancode = htons(scancode);
	msgk->sdlkey = htonl(key);
	msgk->unicode = htonl(unicode);
#endif
	msgk->sdlmod = htons(mod);
	return msg;
}

sdlmsg_t *
sdlmsg_mousekey(sdlmsg_t *msg, unsigned char pressed, unsigned char button, unsigned short x, unsigned short y) {
	sdlmsg_mouse_t *msgm = (sdlmsg_mouse_t*)msg;
	//ga_error("sdl client: button event btn=%u pressed=%u\n", button, pressed);
	ZeroMemory(msg, sizeof(sdlmsg_mouse_t));
	msgm->msgsize = htons(sizeof(sdlmsg_mouse_t));
	msgm->msgtype = SDL_EVENT_MSGTYPE_MOUSEKEY;
	msgm->is_pressed = pressed;
	msgm->mousex = htons(x);
	msgm->mousey = htons(y);
	msgm->mousebutton = button;
	return msg;
}

#if 1	// only support SDL2
sdlmsg_t *
sdlmsg_mousewheel(sdlmsg_t *msg, unsigned short mousex, unsigned short mousey) {
	sdlmsg_mouse_t *msgm = (sdlmsg_mouse_t*)msg;
	//ga_error("sdl client: motion event x=%u y=%u\n", mousex, mousey);
	ZeroMemory(msg, sizeof(sdlmsg_mouse_t));
	msgm->msgsize = htons(sizeof(sdlmsg_mouse_t));
	msgm->msgtype = SDL_EVENT_MSGTYPE_MOUSEWHEEL;
	msgm->mousex = htons(mousex);
	msgm->mousey = htons(mousey);
	return msg;
}
#endif

sdlmsg_t *
sdlmsg_mousemotion(sdlmsg_t *msg, unsigned short mousex, unsigned short mousey, unsigned short relx, unsigned short rely, unsigned char state, int relativeMouseMode) {
	sdlmsg_mouse_t *msgm = (sdlmsg_mouse_t*)msg;
	//ga_error("sdl client: motion event x=%u y=%u\n", mousex, mousey);
	ZeroMemory(msg, sizeof(sdlmsg_mouse_t));
	msgm->msgsize = htons(sizeof(sdlmsg_mouse_t));
	msgm->msgtype = SDL_EVENT_MSGTYPE_MOUSEMOTION;
	msgm->mousestate = state;
	msgm->relativeMouseMode = (relativeMouseMode != 0) ? 1 : 0;
	msgm->mousex = htons(mousex);
	msgm->mousey = htons(mousey);
	msgm->mouseRelX = htons(relx);
	msgm->mouseRelY = htons(rely);
	return msg;
}

//static void
//create_overlay(struct RTSPThreadParam *rtspParam, int ch) {
//	int w, h;
//	PixelFormat format;
//#if 1	// only support SDL2
//	unsigned int renderer_flags = SDL_RENDERER_SOFTWARE;
//	SDL_Window *surface = NULL;
//	SDL_Renderer *renderer = NULL;
//	SDL_Texture *overlay = NULL;
//#endif
//	struct SwsContext *swsctx = NULL;
//	dpipe_t *pipe = NULL;
//	dpipe_buffer_t *data = NULL;
//	char windowTitle[64];
//	char pipename[64];
//	//
//	pthread_mutex_lock(&rtspParam->surfaceMutex[ch]);
//	if (rtspParam->surface[ch] != NULL) {
//		pthread_mutex_unlock(&rtspParam->surfaceMutex[ch]);
//		rtsperror("ga-client: duplicated create window request - image comes too fast?\n");
//		return;
//	}
//	w = rtspParam->width[ch];
//	h = rtspParam->height[ch];
//	format = rtspParam->format[ch];
//	pthread_mutex_unlock(&rtspParam->surfaceMutex[ch]);
//	// swsctx
//	if ((swsctx = sws_getContext(w, h, format, w, h, PIX_FMT_YUV420P,
//		SWS_BICUBIC, NULL, NULL, NULL)) == NULL) {
//		rtsperror("ga-client: cannot create swsscale context.\n");
//		exit(-1);
//	}
//	// pipeline
//	snprintf(pipename, sizeof(pipename), "channel-%d", ch);
//	if ((pipe = dpipe_create(ch, pipename, POOLSIZE, sizeof(AVPicture))) == NULL) {
//		rtsperror("ga-client: cannot create pipeline.\n");
//		exit(-1);
//	}
//	for (data = pipe->in; data != NULL; data = data->next) {
//		bzero(data->pointer, sizeof(AVPicture));
//		if (avpicture_alloc((AVPicture*)data->pointer, PIX_FMT_YUV420P, w, h) != 0) {
//			rtsperror("ga-client: per frame initialization failed.\n");
//			exit(-1);
//		}
//	}
//	// sdl
//	int wflag = 0;
//#if 1	// only support SDL2
//#ifdef	ANDROID
//	wflag = SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_BORDERLESS;
//#else
//	wflag |= SDL_WINDOW_RESIZABLE;
//	if (ga_conf_readbool("fullscreen", 0) != 0) {
//		wflag |= SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_BORDERLESS;
//	}
//#endif
//	if (relativeMouseMode != 0) {
//		wflag |= SDL_WINDOW_INPUT_GRABBED;
//	}
//	snprintf(windowTitle, sizeof(windowTitle), WINDOW_TITLE, ch, w, h);
//	surface = SDL_CreateWindow(windowTitle,
//		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
//		w, h, wflag);
//#endif
//	if (surface == NULL) {
//		rtsperror("ga-client: set video mode (create window) failed.\n");
//		exit(-1);
//	}
//	//SDL_SetWindowMaximumSize(surface, w, h);
//	SDL_SetWindowMinimumSize(surface, w >> 2, h >> 2);
//	nativeSizeX[ch] = windowSizeX[ch] = w;
//	nativeSizeY[ch] = windowSizeY[ch] = h;
//	windowId2ch[SDL_GetWindowID(surface)] = ch;
//	// move mouse to center
//#if 1	// only support SDL2
//	SDL_WarpMouseInWindow(surface, w / 2, h / 2);
//#endif
//	if (relativeMouseMode != 0) {
//		SDL_SetRelativeMouseMode(SDL_TRUE);
//		showCursor = 0;
//		//SDL_ShowCursor(0);
//#if 0		//// XXX: EXPERIMENTAL - switch twice to make it normal?
//		switch_grab_input(NULL);
//		SDL_SetRelativeMouseMode(SDL_FALSE);
//		switch_grab_input(NULL);
//		SDL_SetRelativeMouseMode(SDL_TRUE);
//#endif		////
//		ga_error("ga-client: relative mouse mode enabled.\n");
//	}
//	//
//#if 1	// only support SDL2
//	do {	// choose SW or HW renderer?
//		// XXX: Windows crashed if there is not a HW renderer!
//		int i, n = SDL_GetNumRenderDrivers();
//		SDL_RendererInfo info;
//		for (i = 0; i < n; i++) {
//			if (SDL_GetRenderDriverInfo(i, &info) < 0)
//				continue;
//			rtsperror("ga-client: renderer#%d - %s (%s%s%s%s)\n",
//				i, info.name,
//				info.flags & SDL_RENDERER_SOFTWARE ? "SW" : "",
//				info.flags & SDL_RENDERER_ACCELERATED ? "HW" : "",
//				info.flags & SDL_RENDERER_PRESENTVSYNC ? ",vsync" : "",
//				info.flags & SDL_RENDERER_TARGETTEXTURE ? ",texture" : "");
//			if (info.flags & SDL_RENDERER_ACCELERATED)
//				renderer_flags = SDL_RENDERER_ACCELERATED;
//		}
//	} while (0);
//	//
//	renderer = SDL_CreateRenderer(surface, -1,
//		rtspconf->video_renderer_software ?
//		SDL_RENDERER_SOFTWARE : renderer_flags);
//	if (renderer == NULL) {
//		rtsperror("ga-client: create renderer failed.\n");
//		exit(-1);
//	}
//	//
//	overlay = SDL_CreateTexture(renderer,
//		SDL_PIXELFORMAT_YV12,
//		SDL_TEXTUREACCESS_STREAMING,
//		w, h);
//#endif
//	if (overlay == NULL) {
//		rtsperror("ga-client: create overlay (textuer) failed.\n");
//		exit(-1);
//	}
//	//
//	pthread_mutex_lock(&rtspParam->surfaceMutex[ch]);
//	rtspParam->pipe[ch] = pipe;
//	rtspParam->swsctx[ch] = swsctx;
//	rtspParam->overlay[ch] = overlay;
//#if 1	// only support SDL2
//	rtspParam->renderer[ch] = renderer;
//	rtspParam->windowId[ch] = SDL_GetWindowID(surface);
//#endif
//	rtspParam->surface[ch] = surface;
//	pthread_mutex_unlock(&rtspParam->surfaceMutex[ch]);
//	//
//	rtsperror("ga-client: window created successfully (%dx%d).\n", w, h);
//	// initialize watchdog
//	pthread_mutex_lock(&watchdogMutex);
//	gettimeofday(&watchdogTimer, NULL);
//	pthread_mutex_unlock(&watchdogMutex);
//	//
//	return;
//}

void
ProcessEvent(SDL_Event *event) {
	sdlmsg_t m;
	//map<unsigned int, int>::iterator mi;
	int ch;
	struct timeval tv;
	//
	switch (event->type) {
	case SDL_KEYUP:
//		if (event->key.keysym.sym == SDLK_BACKQUOTE && relativeMouseMode != 0) {
//			showCursor = 1 - showCursor;
//			//SDL_ShowCursor(showCursor);
//			switch_grab_input(NULL);
//#if 1
//			if (showCursor)
//				SDL_SetRelativeMouseMode(SDL_FALSE);
//			else
//				SDL_SetRelativeMouseMode(SDL_TRUE);
//#endif
//		}
//		// switch between fullscreen?
//		if ((event->key.keysym.sym == SDLK_RETURN)
//			&& (event->key.keysym.mod & KMOD_ALT)) {
//			// do nothing
//		}
//		else
//			//
//			if (rtspconf->ctrlenable) {
//				sdlmsg_keyboard(&m, 0,
//					event->key.keysym.scancode,
//					event->key.keysym.sym,
//					event->key.keysym.mod,
//					0/*event->key.keysym.unicode*/);
//				ctrl_client_sendmsg(&m, sizeof(sdlmsg_keyboard_t));
//			}

		sdlmsg_keyboard(&m, 0,
			event->key.keysym.scancode,
			event->key.keysym.sym,
			event->key.keysym.mod,
			0/*event->key.keysym.unicode*/);

		if (savefp_keyts != NULL) {
			gettimeofday(&tv, NULL);
			ga_save_printf(savefp_keyts, "KEY-UP: %u.%06u scan 0x%04x sym 0x%04x mod 0x%04x\n",
				tv.tv_sec, tv.tv_usec,
				event->key.keysym.scancode,
				event->key.keysym.sym,
				event->key.keysym.mod);
		}
		printf("key up\n");
		break;
	case SDL_KEYDOWN:

		if (keyboard_state_array[SDL_SCANCODE_LCTRL] && keyboard_state_array[SDL_SCANCODE_LSHIFT] && keyboard_state_array[SDL_SCANCODE_F12]) {

			printf("window resize\n");
			printf("resize\n");

			SDL_ShowWindow(window);
			SDL_MaximizeWindow(window);
			SDL_SetWindowResizable(window, SDL_TRUE);
			SDL_SetWindowFullscreen(window, SDL_WINDOW_MAXIMIZED);
		}
		if (keyboard_state_array[SDL_SCANCODE_LCTRL] && keyboard_state_array[SDL_SCANCODE_LSHIFT] && keyboard_state_array[SDL_SCANCODE_F11]) {
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
			printf("window change\n");
		}
		//if ((event->key.keysym.sym == SDLK_RETURN)
		//	&& (event->key.keysym.mod & KMOD_ALT)) {
		//	switch_fullscreen();
		//}
		//else
		//	//

		sdlmsg_keyboard(&m, 1,
				event->key.keysym.scancode,
				event->key.keysym.sym,
				event->key.keysym.mod,
				0/*event->key.keysym.unicode*/);
			
		if (savefp_keyts != NULL) {
			gettimeofday(&tv, NULL);
			ga_save_printf(savefp_keyts, "KEY-DOWN: %u.%06u scan 0x%04x sym 0x%04x mod 0x%04x\n",
				tv.tv_sec, tv.tv_usec,
				event->key.keysym.scancode,
				event->key.keysym.sym,
				event->key.keysym.mod);
		}
		printf("key down\n");
		break;
	case SDL_MOUSEBUTTONUP:

		sdlmsg_mousekey(&m, 0, event->button.button,
			event->button.x,
			event->button.y);

		printf("mouse up\n");
		break;
	case SDL_MOUSEBUTTONDOWN:

		sdlmsg_mousekey(&m, 1, event->button.button,
			event->button.x,
			event->button.y);

		printf("mouse down\n");
		break;
	case SDL_MOUSEMOTION:

		sdlmsg_mousemotion(&m,
			event->motion.x,
			event->motion.y,
			event->motion.xrel,
			event->motion.yrel,
			event->motion.state,
			0/*relativeMouseMode == 0 ? 0 : 1*/);

		printf("mouse motion\n");
		break;
#if 1	// only support SDL2
	case SDL_MOUSEWHEEL:

		sdlmsg_mousewheel(&m, event->motion.x, event->motion.y);

		
		printf("mouse wheel\n");
		break;

	case SDL_WINDOWEVENT:
		/*if (event->window.event == SDL_WINDOWEVENT_CLOSE) {
			rtspThreadParam.running = false;
			return;
		}
		else if (event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
			mi = windowId2ch.find(event->window.windowID);
			if (mi != windowId2ch.end()) {
				int w, h, ch = mi->second;
				char title[64];
				w = event->window.data1;
				h = event->window.data2;
				windowSizeX[ch] = w;
				windowSizeY[ch] = h;
				snprintf(title, sizeof(title), WINDOW_TITLE, ch, w, h);
				SDL_SetWindowTitle(rtspThreadParam.surface[ch], title);
				rtsperror("event window #%d(%x) resized: w=%d h=%d\n",
					ch, event->window.windowID, w, h);
			}
		}*/
		printf("window event\n");
		break;
	case SDL_USEREVENT:

		printf("user event in\n");

		if (event->user.code == SDL_USEREVENT_RENDER_IMAGE) {
			printf("\t user event render_image\n");
			/*long long ch = (long long)event->user.data2;
			render_image((struct RTSPThreadParam*) event->user.data1, (int)ch & 0x0ffffffff);*/
			break;
		}
		if (event->user.code == SDL_USEREVENT_CREATE_OVERLAY) {
			printf("\t user event create overlay\n");
			/*long long ch = (long long)event->user.data2;
			create_overlay((struct RTSPThreadParam*) event->user.data1, (int)ch & 0x0ffffffff);*/
			break;
		}
		if (event->user.code == SDL_USEREVENT_OPEN_AUDIO) {
			printf("\t user event open audio\n");
			/*open_audio(
				(struct RTSPThreadParam*) event->user.data1,
				(AVCodecContext*)event->user.data2);*/
			break;
		}
		if (event->user.code == SDL_USEREVENT_RENDER_TEXT) {
			printf("\t user event render text\n");
			////SDL_SetAlpha()
			//SDL_SetRenderDrawColor(rtspThreadParam.renderer[0], 0, 0, 0, 192/*SDL_ALPHA_OPAQUE/2*/);
			////SDL_RenderFillRect(rtspThreadParam.renderer[0], NULL);
			//render_text(rtspThreadParam.renderer[0],
			//	rtspThreadParam.surface[0],
			//	-1, -1, 0, (const char *)event->user.data1);
			//SDL_RenderPresent(rtspThreadParam.renderer[0]);
			break;
		}
		break;
#endif /* SDL_VERSION_ATLEAST(2,0,0) */
	case SDL_QUIT:
		/*rtspThreadParam.running = false;*/
		printf("sdl quit");
		return;
	default:
		// do nothing
		break;
	}
	return;
}

int main()
{
	SDL_Event event;
	char savefile_keyts[128] = "savefp_keyfile.txt";

	savefp_keyts = ga_save_init_txt(savefile_keyts);

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		printf("SDL init failed: %s\n", SDL_GetError());
		return -1;
	}

	window = SDL_CreateWindow("MSLM RX", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1920, 1080, 0/*SDL_WINDOW_RESIZABLE*/);
	renderer = SDL_CreateRenderer(window, -1, 0);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STREAMING, 1920, 1080);


	while (true)
	{
		if (SDL_WaitEvent(&event)) {
			ProcessEvent(&event);
		}
	}

	if (savefp_keyts != NULL) {
		ga_save_close(savefp_keyts);
		savefp_keyts = NULL;
	}

	SDL_Quit();

	return 0;
}