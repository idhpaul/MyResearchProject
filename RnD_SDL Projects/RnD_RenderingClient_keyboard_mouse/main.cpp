#include <stdio.h>

#include "SDL.h"

#ifdef main
#undef main
#endif

#define	SDL_USEREVENT_CREATE_OVERLAY	0x0001
#define	SDL_USEREVENT_OPEN_AUDIO		0x0002
#define	SDL_USEREVENT_RENDER_IMAGE		0x0004
#define	SDL_USEREVENT_RENDER_TEXT		0x0008

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
	/*sdlmsg_t m;
	map<unsigned int, int>::iterator mi;
	int ch;
	struct timeval tv;*/
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
//		if (savefp_keyts != NULL) {
//			gettimeofday(&tv, NULL);
//			ga_save_printf(savefp_keyts, "KEY-UP: %u.%06u scan 0x%04x sym 0x%04x mod 0x%04x\n",
//				tv.tv_sec, tv.tv_usec,
//				event->key.keysym.scancode,
//				event->key.keysym.sym,
//				event->key.keysym.mod);
//		}
		printf("key up\n");
		break;
	case SDL_KEYDOWN:
		//// switch between fullscreen?
		//if ((event->key.keysym.sym == SDLK_RETURN)
		//	&& (event->key.keysym.mod & KMOD_ALT)) {
		//	switch_fullscreen();
		//}
		//else
		//	//
		//	if (rtspconf->ctrlenable) {
		//		sdlmsg_keyboard(&m, 1,
		//			event->key.keysym.scancode,
		//			event->key.keysym.sym,
		//			event->key.keysym.mod,
		//			0/*event->key.keysym.unicode*/);
		//		ctrl_client_sendmsg(&m, sizeof(sdlmsg_keyboard_t));
		//	}
		//if (savefp_keyts != NULL) {
		//	gettimeofday(&tv, NULL);
		//	ga_save_printf(savefp_keyts, "KEY-DN: %u.%06u scan 0x%04x sym 0x%04x mod 0x%04x\n",
		//		tv.tv_sec, tv.tv_usec,
		//		event->key.keysym.scancode,
		//		event->key.keysym.sym,
		//		event->key.keysym.mod);
		//}
		printf("key down\n");
		break;
	case SDL_MOUSEBUTTONUP:
		/*mi = windowId2ch.find(event->button.windowID);
		if (mi != windowId2ch.end() && rtspconf->ctrlenable) {
			ch = mi->second;
			sdlmsg_mousekey(&m, 0, event->button.button,
				xlat_mouseX(ch, event->button.x),
				xlat_mouseY(ch, event->button.y));
			ctrl_client_sendmsg(&m, sizeof(sdlmsg_mouse_t));
		}*/
		printf("mouse up\n");
		break;
	case SDL_MOUSEBUTTONDOWN:
		/*mi = windowId2ch.find(event->button.windowID);
		if (mi != windowId2ch.end() && rtspconf->ctrlenable) {
			ch = mi->second;
			sdlmsg_mousekey(&m, 1, event->button.button,
				xlat_mouseX(ch, event->button.x),
				xlat_mouseY(ch, event->button.y));
			ctrl_client_sendmsg(&m, sizeof(sdlmsg_mouse_t));
		}*/
		printf("mouse down\n");
		break;
	case SDL_MOUSEMOTION:
		/*mi = windowId2ch.find(event->motion.windowID);
		if (mi != windowId2ch.end() && rtspconf->ctrlenable && rtspconf->sendmousemotion) {
			ch = mi->second;
			sdlmsg_mousemotion(&m,
				xlat_mouseX(ch, event->motion.x),
				xlat_mouseY(ch, event->motion.y),
				xlat_mouseX(ch, event->motion.xrel),
				xlat_mouseY(ch, event->motion.yrel),
				event->motion.state,
				relativeMouseMode == 0 ? 0 : 1);
			ctrl_client_sendmsg(&m, sizeof(sdlmsg_mouse_t));
		}*/
		printf("mouse motion\n");
		break;
#if 1	// only support SDL2
	case SDL_MOUSEWHEEL:
		/*if (rtspconf->ctrlenable && rtspconf->sendmousemotion) {
			sdlmsg_mousewheel(&m, event->motion.x, event->motion.y);
			ctrl_client_sendmsg(&m, sizeof(sdlmsg_mouse_t));
		}*/
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

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		printf("SDL init failed: %s\n", SDL_GetError());
		return -1;
	}

	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Texture *texture = NULL;

	window = SDL_CreateWindow("SDL Window Title", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, 0/*SDL_WINDOW_RESIZEABLE*/);
	renderer = SDL_CreateRenderer(window, -1, 0);

	while (true)
	{
		if (SDL_WaitEvent(&event)) {
			ProcessEvent(&event);
		}
	}

	SDL_Quit();

	return 0;
}