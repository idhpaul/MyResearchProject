#include <iostream>
#include <thread>
#include <stdio.h>

#include <Windows.h>
#pragma comment(lib,"ws2_32.lib")



#include <map>
using namespace std;
typedef WORD KeySym;

#ifdef main
#undef main
#endif

#include "main.h"

SDL_Event event;
Uint32 Hangul = SDL_RegisterEvents(1);
Uint32 upHangul = SDL_RegisterEvents(2);

#define __SOCKET 0
#if __SOCKET
WSADATA wsadata;
SOCKET clientSocket;
struct sockaddr_in clientsockinfo;
#endif

#define	SDL_USEREVENT_CREATE_OVERLAY	0x0001
#define	SDL_USEREVENT_OPEN_AUDIO		0x0002
#define	SDL_USEREVENT_RENDER_IMAGE		0x0004
#define	SDL_USEREVENT_RENDER_TEXT		0x0008


SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;



#define	VIDEO_SOURCE_CHANNEL_MAX	2
static int windowSizeX[VIDEO_SOURCE_CHANNEL_MAX];
static int windowSizeY[VIDEO_SOURCE_CHANNEL_MAX];
// support resizable window
static int nativeSizeX[VIDEO_SOURCE_CHANNEL_MAX];
static int nativeSizeY[VIDEO_SOURCE_CHANNEL_MAX];

static map<int, KeySym> keymap;
static KeySym SDLKeyToKeySym(int sdlkey);

static void
SDLKeyToKeySym_init() {
	unsigned short i;
	//
	keymap[SDLK_BACKSPACE] = VK_BACK;		//		= 8,
	keymap[SDLK_TAB] = VK_TAB;		//		= 9,
	keymap[SDLK_CLEAR] = VK_CLEAR;		//		= 12,
	keymap[SDLK_RETURN] = VK_RETURN;		//		= 13,
	keymap[SDLK_PAUSE] = VK_PAUSE;		//		= 19,
	keymap[SDLK_ESCAPE] = VK_ESCAPE;		//		= 27,
	// Latin 1: starting from space (0x20)
	keymap[SDLK_SPACE] = VK_SPACE;		//		= 32,
	// (0x20) space, exclam, quotedbl, numbersign, dollar, percent, ampersand,
	// (0x27) quoteright, parentleft, parentright, asterisk, plus, comma,
	// (0x2d) minus, period, slash
	//SDLK_EXCLAIM		= 33,
	keymap[SDLK_QUOTEDBL] = VK_OEM_7;		//		= 34,
	//SDLK_HASH		= 35,
	//SDLK_DOLLAR		= 36,
	//SDLK_AMPERSAND		= 38,
	keymap[SDLK_QUOTE] = VK_OEM_7;		//		= 39,
	//SDLK_LEFTPAREN		= 40,
	//SDLK_RIGHTPAREN		= 41,
	//SDLK_ASTERISK		= 42,
	keymap[SDLK_PLUS] = VK_OEM_PLUS;		//		= 43,
	keymap[SDLK_COMMA] = VK_OEM_COMMA;		//		= 44,
	keymap[SDLK_MINUS] = VK_OEM_MINUS;		//		= 45,
	keymap[SDLK_PERIOD] = VK_OEM_PERIOD;	//		= 46,
	keymap[SDLK_SLASH] = VK_OEM_2;		//		= 47,
	keymap[SDLK_COLON] = VK_OEM_1;		//		= 58,
	keymap[SDLK_SEMICOLON] = VK_OEM_1;		//		= 59,
	keymap[SDLK_LESS] = VK_OEM_COMMA;		//		= 60,
	keymap[SDLK_EQUALS] = VK_OEM_PLUS;		//		= 61,
	keymap[SDLK_GREATER] = VK_OEM_PERIOD;	//		= 62,
	keymap[SDLK_QUESTION] = VK_OEM_2;		//		= 63,
	//SDLK_AT			= 64,
	/*
	   Skip uppercase letters
	 */
	keymap[SDLK_LEFTBRACKET] = VK_OEM_4;		//		= 91,
	keymap[SDLK_BACKSLASH] = VK_OEM_5;		//		= 92,
	keymap[SDLK_RIGHTBRACKET] = VK_OEM_6;		//		= 93,
	//SDLK_CARET		= 94,
	keymap[SDLK_UNDERSCORE] = VK_OEM_MINUS;		//		= 95,
	keymap[SDLK_BACKQUOTE] = VK_OEM_3;		//		= 96,
	// (0x30-0x39) 0-9
	for (i = 0x30; i <= 0x39; i++) {
		keymap[i] = i;
	}
	// (0x3a) colon, semicolon, less, equal, greater, question, at
	// (0x41-0x5a) A-Z
	// SDL: no upper cases, only lower cases
	// (0x5b) bracketleft, backslash, bracketright, asciicircum/caret,
	// (0x5f) underscore, grave
	// (0x61-7a) a-z
	for (i = 0x61; i <= 0x7a; i++) {
		keymap[i] = i & 0xdf;	// convert to uppercases
	}
	keymap[SDLK_DELETE] = VK_DELETE;		//		= 127,
	// SDLK_WORLD_0 (0xa0) - SDLK_WORLD_95 (0xff) are ignored
	/** @name Numeric keypad */
	keymap[SDLK_KP_0] = VK_NUMPAD0;	//		= 256,
	keymap[SDLK_KP_1] = VK_NUMPAD1;	//		= 257,
	keymap[SDLK_KP_2] = VK_NUMPAD2;	//		= 258,
	keymap[SDLK_KP_3] = VK_NUMPAD3;	//		= 259,
	keymap[SDLK_KP_4] = VK_NUMPAD4;	//		= 260,
	keymap[SDLK_KP_5] = VK_NUMPAD5;	//		= 261,
	keymap[SDLK_KP_6] = VK_NUMPAD6;	//		= 262,
	keymap[SDLK_KP_7] = VK_NUMPAD7;	//		= 263,
	keymap[SDLK_KP_8] = VK_NUMPAD8;	//		= 264,
	keymap[SDLK_KP_9] = VK_NUMPAD9;	//		= 265,
	keymap[SDLK_KP_PERIOD] = VK_DECIMAL;	//		= 266,
	keymap[SDLK_KP_DIVIDE] = VK_DIVIDE;	//		= 267,
	keymap[SDLK_KP_MULTIPLY] = VK_MULTIPLY;	//		= 268,
	keymap[SDLK_KP_MINUS] = VK_SUBTRACT;	//		= 269,
	keymap[SDLK_KP_PLUS] = VK_ADD;	//		= 270,
	//keymap[SDLK_KP_ENTER]	= XK_KP_Enter;	//		= 271,
	//keymap[SDLK_KP_EQUALS]	= XK_KP_Equal;	//		= 272,
	/** @name Arrows + Home/End pad */
	keymap[SDLK_UP] = VK_UP;	//		= 273,
	keymap[SDLK_DOWN] = VK_DOWN;	//		= 274,
	keymap[SDLK_RIGHT] = VK_RIGHT;	//		= 275,
	keymap[SDLK_LEFT] = VK_LEFT;	//		= 276,
	keymap[SDLK_INSERT] = VK_INSERT;	//		= 277,
	keymap[SDLK_HOME] = VK_HOME;	//		= 278,
	keymap[SDLK_END] = VK_END;	//		= 279,
	keymap[SDLK_PAGEUP] = VK_PRIOR;	//		= 280,
	keymap[SDLK_PAGEDOWN] = VK_NEXT;	//		= 281,
	/** @name Function keys */
	keymap[SDLK_F1] = VK_F1;	//		= 282,
	keymap[SDLK_F2] = VK_F2;	//		= 283,
	keymap[SDLK_F3] = VK_F3;	//		= 284,
	keymap[SDLK_F4] = VK_F4;	//		= 285,
	keymap[SDLK_F5] = VK_F5;	//		= 286,
	keymap[SDLK_F6] = VK_F6;	//		= 287,
	keymap[SDLK_F7] = VK_F7;	//		= 288,
	keymap[SDLK_F8] = VK_F8;	//		= 289,
	keymap[SDLK_F9] = VK_F9;	//		= 290,
	keymap[SDLK_F10] = VK_F10;	//		= 291,
	keymap[SDLK_F11] = VK_F11;	//		= 292,
	keymap[SDLK_F12] = VK_F12;	//		= 293,
	keymap[SDLK_F13] = VK_F13;	//		= 294,
	keymap[SDLK_F14] = VK_F14;	//		= 295,
	keymap[SDLK_F15] = VK_F15;	//		= 296,
	/** @name Key state modifier keys */
	keymap[SDLK_NUMLOCKCLEAR] = VK_NUMLOCK;	//		= 300,
	keymap[SDLK_CAPSLOCK] = VK_CAPITAL;	//		= 301,
	keymap[SDLK_SCROLLLOCK] = VK_SCROLL;	//		= 302,
	keymap[SDLK_RSHIFT] = VK_RSHIFT;	//		= 303,
	keymap[SDLK_LSHIFT] = VK_LSHIFT;	//		= 304,
	keymap[SDLK_RCTRL] = VK_RCONTROL;	//		= 305,
	keymap[SDLK_LCTRL] = VK_LCONTROL;	//		= 306,
	keymap[SDLK_RALT] = VK_RMENU;	//		= 307,
	keymap[SDLK_LALT] = VK_LMENU;	//		= 308,
	keymap[SDLK_RGUI] = VK_RWIN;	//		= 309,
	keymap[SDLK_LGUI] = VK_LWIN;	//		= 310,
	//keymap[SDLK_LSUPER]	= XK_Super_L;	//		= 311,		/**< Left "Windows" key */
	//keymap[SDLK_RSUPER]	= XK_Super_R;	//		= 312,		/**< Right "Windows" key */
	keymap[SDLK_MODE] = VK_MODECHANGE;//		= 313,		/**< "Alt Gr" key */
	//keymap[SDLK_COMPOSE]	= XK_Multi_key;	//		= 314,		/**< Multi-key compose key */
	/** @name Miscellaneous function keys */
	keymap[SDLK_HELP] = VK_HELP;	//		= 315,
	//keymap[SDLK_SYSREQ]	= XK_Sys_Req;	//		= 317,
	keymap[SDLK_CANCEL] = VK_CANCEL;	//		= 318,
	keymap[SDLK_MENU] = VK_MENU;	//		= 319,

	return;
}
static KeySym
SDLKeyToKeySym(int sdlkey) {
	map<int, KeySym>::iterator mi;

	SDLKeyToKeySym_init();

	if ((mi = keymap.find(sdlkey)) != keymap.end()) {
		return mi->second;
	}
	return INVALID_KEY;
}


#define	GEN_KB_ADD_FUNC_PROTO(type, field) \
	int sdlmsg_kb_add_##field(type v, int remove)
#define GEN_KB_ADD_FUNC(type, field, db)	\
	GEN_KB_ADD_FUNC_PROTO(type, field) { \
		if(remove) { db.erase(v); } \
		else       { db[v] = v;   } \
		return 0; \
	}
#define GEN_KB_MATCH_FUNC_PROTO(type, field) \
	int sdlmsg_kb_match_##field(type v)
#define	GEN_KB_MATCH_FUNC(type, field, db) \
	GEN_KB_MATCH_FUNC_PROTO(type, field) { \
		if(db.find(v) == db.end()) { return 0; } \
		return 1; \
	}

static bool keyblock_initialized = false;
static map<unsigned short, unsigned short> kbScancode;
static map<int, int> kbSdlkey;

GEN_KB_ADD_FUNC(unsigned short, scancode, kbScancode)
GEN_KB_ADD_FUNC(int, sdlkey, kbSdlkey)
GEN_KB_MATCH_FUNC(unsigned short, scancode, kbScancode)
GEN_KB_MATCH_FUNC(int, sdlkey, kbSdlkey)

int
sdlmsg_key_blocked(sdlmsg_t *msg) {
	sdlmsg_keyboard_t *msgk;
	if (msg->msgtype != SDL_EVENT_MSGTYPE_KEYBOARD) {
		return 0;
	}
	//
	msgk = (sdlmsg_keyboard_t*)msg;
	//
	if (sdlmsg_kb_match_scancode(msgk->scancode)) {
		return 1;
	}
	if (sdlmsg_kb_match_sdlkey(msgk->sdlkey)) {
		return 1;
	}
	return 0;
}


static void
sdlmsg_replay_native(sdlmsg_t *msg) {
	INPUT in;
	sdlmsg_keyboard_t *msgk = (sdlmsg_keyboard_t*)msg;
	sdlmsg_mouse_t *msgm = (sdlmsg_mouse_t*)msg;
	//
	switch (msg->msgtype) {
	case SDL_EVENT_MSGTYPE_KEYBOARD:
		ZeroMemory(&in, sizeof(in));
		in.type = INPUT_KEYBOARD;
		if ((in.ki.wVk = SDLKeyToKeySym(msgk->sdlkey)) != INVALID_KEY) {
			if (msgk->is_pressed == 0) {
				in.ki.dwFlags |= KEYEVENTF_KEYUP;
			}
			in.ki.wScan = MapVirtualKey(in.ki.wVk, MAPVK_VK_TO_VSC);
			//ga_error("sdl replayer: vk=%x scan=%x\n", in.ki.wVk, in.ki.wScan);
			printf("in");
			SendInput(1, &in, sizeof(in));
		}
		else {
			////////////////
			printf("sdl replayer: undefined key scan=%u(%04x) key=%u(%04x) mod=%u(%04x) pressed=%d\n",
				msgk->scancode, msgk->scancode,
				msgk->sdlkey, msgk->sdlkey, msgk->sdlmod, msgk->sdlmod,
				msgk->is_pressed);
			////////////////
		}
		break;
	case SDL_EVENT_MSGTYPE_MOUSEKEY:
		////ga_error("sdl replayer: button event btn=%u pressed=%d\n", msg->mousebutton, msg->is_pressed);
		//ZeroMemory(&in, sizeof(in));
		//in.type = INPUT_MOUSE;
		//if (msgm->mousebutton == 1 && msgm->is_pressed != 0) {
		//	in.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
		//}
		//else if (msgm->mousebutton == 1 && msgm->is_pressed == 0) {
		//	in.mi.dwFlags = MOUSEEVENTF_LEFTUP;
		//}
		//else if (msgm->mousebutton == 2 && msgm->is_pressed != 0) {
		//	in.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
		//}
		//else if (msgm->mousebutton == 2 && msgm->is_pressed == 0) {
		//	in.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
		//}
		//else if (msgm->mousebutton == 3 && msgm->is_pressed != 0) {
		//	in.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
		//}
		//else if (msgm->mousebutton == 3 && msgm->is_pressed == 0) {
		//	in.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
		//}
		//else if (msgm->mousebutton == 4 && msgm->is_pressed != 0) {
		//	// mouse wheel forward
		//	in.mi.dwFlags = MOUSEEVENTF_WHEEL;
		//	in.mi.mouseData = +WHEEL_DELTA;
		//}
		//else if (msgm->mousebutton == 5 && msgm->is_pressed != 0) {
		//	// mouse wheel backward
		//	in.mi.dwFlags = MOUSEEVENTF_WHEEL;
		//	in.mi.mouseData = -WHEEL_DELTA;
		//}
		//SendInput(1, &in, sizeof(in));
		break;
	case SDL_EVENT_MSGTYPE_MOUSEWHEEL:
		//if (msgm->mousex != 0) {
		//	ZeroMemory(&in, sizeof(in));
		//	in.type = INPUT_MOUSE;
		//	if (((short)msgm->mousex) > 0) {
		//		// mouse wheel forward
		//		in.mi.dwFlags = MOUSEEVENTF_WHEEL;
		//		in.mi.mouseData = +WHEEL_DELTA;
		//	}
		//	else if (((short)msgm->mousex) < 0) {
		//		// mouse wheel backward
		//		in.mi.dwFlags = MOUSEEVENTF_WHEEL;
		//		in.mi.mouseData = -WHEEL_DELTA;
		//	}
		//	SendInput(1, &in, sizeof(in));
		//}

		break;
	case SDL_EVENT_MSGTYPE_MOUSEMOTION:
		////ga_error("sdl replayer: motion event x=%u y=%d\n", msgm->mousex, msgm->mousey);
		//ZeroMemory(&in, sizeof(in));
		//in.type = INPUT_MOUSE;
		//// mouse x/y has to be mapped to (0,0)-(65535,65535)
		//if (msgm->relativeMouseMode == 0) {
		//	if (prect == NULL) {
		//		in.mi.dx = (DWORD)
		//			(65536.0 * scaleFactorX * msgm->mousex) / cxsize;
		//		in.mi.dy = (DWORD)
		//			(65536.0 * scaleFactorY * msgm->mousey) / cysize;
		//	}
		//	else {
		//		in.mi.dx = (DWORD)
		//			(65536.0 * (prect->left + scaleFactorX * msgm->mousex)) / cxsize;
		//		in.mi.dy = (DWORD)
		//			(65536.0 * (prect->top + scaleFactorY * msgm->mousey)) / cysize;
		//	}
		//	in.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
		//}
		//else {
		//	in.mi.dx = (short)(scaleFactorX * msgm->mouseRelX);
		//	in.mi.dy = (short)(scaleFactorY * msgm->mouseRelY);
		//	in.mi.dwFlags = MOUSEEVENTF_MOVE;
		//}
		//SendInput(1, &in, sizeof(in));
		break;
	default: // do nothing
		break;
	}
	return;
}

int
sdlmsg_replay(sdlmsg_t *msg) {
	// convert from network byte order to host byte order
	//sdlmsg_ntoh(msg);
	/*if (sdlmsg_key_blocked(msg)) {
		return 0;
	}*/
	sdlmsg_replay_native(msg);
	return 0;
}

void
ProcessEvent(SDL_Event *event) {
	sdlmsg_t m;
	//map<unsigned int, int>::iterator mi;
	int ch;
	struct timeval tv;

	//sdlmsg_replay(&m);
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

		printf("key up :%d\n", event->key.keysym.scancode);
		break;
	case SDL_KEYDOWN:

		if (event->key.keysym.sym == SDLK_F12) {
			//#idea1 모드 변경안하고 하는 방법
			SDL_SetWindowResizable(window, SDL_TRUE);
			SDL_SetWindowResizable(window, SDL_TRUE);
			SDL_SetWindowPosition(window, 100, 100);
			//

			//#idea2 미니멈사이즈 이용
			//SDL_SetWindowResizable(window, SDL_TRUE);
			//SDL_SetWindowPosition(window, 10, 10);




			//SDL_SetWindowFullscreen(window, SDL_WINDOW_MAXIMIZED);
			//SDL_SetWindowSize(window, 1280, 720);
			//SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
			//SDL_ShowWindow(window);
			//SDL_SetWindowFullscreen(window, SDL_WINDOW_SHOWN);


			/*SDL_ShowWindow(window);
			SDL_MaximizeWindow(window);
			SDL_SetWindowResizable(window, SDL_TRUE);
			SDL_SetWindowFullscreen(window, SDL_WINDOW_MAXIMIZED);*/
		}
		else if(event->key.keysym.sym == SDLK_F11) {
			//#모드 변경안하고 하는 방법
			SDL_SetWindowResizable(window, SDL_FALSE);
			SDL_SetWindowFullscreen(window, SDL_WINDOW_MAXIMIZED);
			SDL_SetWindowPosition(window, 0, 0);


			//SDL_SetWindowResizable(window, SDL_FALSE);
			//SDL_MaximizeWindow(window);
			//SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
			//SDL_SetWindowPosition(window, 0, 0);
			//SDL_SetWindowDisplayMode(window, NULL);

		}
			//


		//if (event->key.keysym.scancode != SDL_GetScancodeFromKey(event->key.keysym.sym))
		//	printf("GetScancodeName : %s \n GetKeyName %s\n", SDL_GetScancodeName(event->key.keysym.scancode), SDL_GetKeyName(event->key.keysym.sym));

		//SDL_Log("Physical %s key acting as %s key", SDL_GetScancodeName(event->key.keysym.scancode), SDL_GetKeyName(event->key.keysym.sym));


		sdlmsg_keyboard(&m, 1,
				event->key.keysym.scancode,
				event->key.keysym.sym,
				event->key.keysym.mod,
				0/*event->key.keysym.unicode*/);
		
		printf("key down\n");
		break;
	case SDL_MOUSEBUTTONUP:

		sdlmsg_mousekey(&m, 0, event->button.button,
			event->button.x,
			event->button.y);

		std::cout << "Mouse up" << std::endl;
		std::cout << "\t button : " << event->button.button << std::endl;
		std::cout << "\t button x : " << event->button.x << std::endl;
		std::cout << "\t button y : " << event->button.y << std::endl;
		std::cout << "\t button state : " << event->button.state << std::endl;
		std::cout << "\t button type : " << event->button.type << std::endl;


		break;
	case SDL_MOUSEBUTTONDOWN:

		sdlmsg_mousekey(&m, 1, event->button.button,
			event->button.x,
			event->button.y);

		printf("mouse down\n");
		std::cout << "\t button : " << event->button.button << std::endl;
		std::cout << "\t button x : " << event->button.x << std::endl;
		std::cout << "\t button y : " << event->button.y << std::endl;
		std::cout << "\t button state : " << event->button.state << std::endl;
		std::cout << "\t button type : " << event->button.type << std::endl;
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
		std::cout << "\t button : " << event->button.button << std::endl;
		std::cout << "\t button x : " << event->button.x << std::endl;
		std::cout << "\t button y : " << event->button.y << std::endl;
		std::cout << "\t button state : " << event->button.state << std::endl;
		std::cout << "\t button type : " << event->button.type << std::endl;
		break;
#if 1	// only support SDL2
	case SDL_MOUSEWHEEL:

		sdlmsg_mousewheel(&m, event->motion.x, event->motion.y);

		
		printf("mouse wheel\n");
		std::cout << "\t button : " << event->button.button << std::endl;
		std::cout << "\t button x : " << event->button.x << std::endl;
		std::cout << "\t button y : " << event->button.y << std::endl;
		std::cout << "\t button state : " << event->button.state << std::endl;
		std::cout << "\t button type : " << event->button.type << std::endl;
		break;

	case SDL_WINDOWEVENT:

		if (event->type == SDL_WINDOWEVENT) {
			switch (event->window.event) {
			case SDL_WINDOWEVENT_SHOWN:
				SDL_Log("Window %d shown", event->window.windowID);
				break;
			case SDL_WINDOWEVENT_HIDDEN:
				SDL_Log("Window %d hidden", event->window.windowID);
				break;
			case SDL_WINDOWEVENT_EXPOSED:
				SDL_Log("Window %d exposed", event->window.windowID);
				break;
			case SDL_WINDOWEVENT_MOVED:
				SDL_Log("Window %d moved to %d,%d",
					event->window.windowID, event->window.data1,
					event->window.data2);
				break;
			case SDL_WINDOWEVENT_RESIZED:
				SDL_Log("Window %d resized to %dx%d",
					event->window.windowID, event->window.data1,
					event->window.data2);
				break;
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				SDL_Log("Window %d size changed to %dx%d",
					event->window.windowID, event->window.data1,
					event->window.data2);
				break;
			case SDL_WINDOWEVENT_MINIMIZED:
				SDL_Log("Window %d minimized", event->window.windowID);
				break;
			case SDL_WINDOWEVENT_MAXIMIZED:
				SDL_Log("Window %d maximized", event->window.windowID);
				break;
			case SDL_WINDOWEVENT_RESTORED:
				SDL_Log("Window %d restored", event->window.windowID);
				break;
			case SDL_WINDOWEVENT_ENTER:
				SDL_Log("Mouse entered window %d",
					event->window.windowID);
				break;
			case SDL_WINDOWEVENT_LEAVE:
				SDL_Log("Mouse left window %d", event->window.windowID);
				break;
			case SDL_WINDOWEVENT_FOCUS_GAINED:
				SDL_Log("Window %d gained keyboard focus",
					event->window.windowID);
				break;
			case SDL_WINDOWEVENT_FOCUS_LOST:
				SDL_Log("Window %d lost keyboard focus",
					event->window.windowID);
				break;
			case SDL_WINDOWEVENT_CLOSE:
				SDL_Log("Window %d closed", event->window.windowID);
				break;
#if SDL_VERSION_ATLEAST(2, 0, 5)
			case SDL_WINDOWEVENT_TAKE_FOCUS:
				SDL_Log("Window %d is offered a focus", event->window.windowID);
				break;
			case SDL_WINDOWEVENT_HIT_TEST:
				SDL_Log("Window %d has a special hit test", event->window.windowID);
				break;
#endif
			default:
				SDL_Log("Window %d got unknown event %d",
					event->window.windowID, event->window.event);
				break;
			}
		}
		break;
	case SDL_USEREVENT:

		printf("user event in\n");
		if (event->user.code == 1) {
			printf("user data %d\n", event->user.data1);
			break;
		}
		if (event->user.code == 2) {
			printf("user message %s\n", event->user.data1);
			break;
		}
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
#if __SOCKET
	send(clientSocket, (char*)&m, sizeof(sdlmsg_t), 0);
#endif
	return;
}
#if __SOCKET
int socket_init(void) {

	int opt = 0;
	int recvlen = 0;

	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
		printf("windows socket initialize failed\n");
		return -1;
	}

	clientSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET) {
		printf("failed to create socket");
		return -1;
	}
	else {
		printf("socket ok\n");
	}

	memset(&clientsockinfo, 0, sizeof(clientsockinfo));

	clientsockinfo.sin_family = AF_INET;
	clientsockinfo.sin_addr.s_addr = inet_addr("192.168.0.3");
	clientsockinfo.sin_port = htons(27015);

	setsockopt(clientSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(opt));

	if (connect(clientSocket, (SOCKADDR*)&clientsockinfo, sizeof(clientsockinfo)) == SOCKET_ERROR) {
		printf("faile to connect");
		return -1;
	}
	else {
		printf("connect\n");
	}

	printf("Socket Connected\n");
}
#endif



int m_nHotKeyID;
HHOOK hHook;

//LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
//{
//	PKBDLLHOOKSTRUCT pKey = (PKBDLLHOOKSTRUCT)lParam;
//
//	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
//	{
//		
//		//Now just check pKey->vkCode etc for whatever you want
//		//for instance, a basic printout of the value and a check for pgup
//		if (pKey->vkCode == VK_HANGUL) {
//			printf("한영키 누름\n");
//			event.type = Hangul;
//			event.user.code = 1;
//			event.user.data1 = (int*)0x15;
//			event.user.data2 = 0;
//			SDL_PushEvent(&event);
//
//		}
//			
//		else if (pKey->vkCode == 25)
//			printf("한자키 누름\n");
//		else {
//			printf("%d\n", pKey->vkCode);
//		}
//	}
//
//	CallNextHookEx(hHook, nCode, wParam, lParam);
//	return 0;
//}

int main()
{
	

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		printf("SDL init failed: %s\n", SDL_GetError());
		return -1;
	}
#if __SOCKET
	socket_init();
#endif
	int w, h, ch = 0;
	w = 1280;
	h = 720;

	unsigned int renderer_flags = SDL_RENDERER_ACCELERATED;



	//

	int wflag = 0;
	wflag |= SDL_WINDOW_RESIZABLE;

	window = SDL_CreateWindow("MSLM RX", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, 0);
	//SDL_SetWindowMinimumSize(window, w >> 2, h >> 2);
	//SDL_WarpMouseInWindow(window, w / 2, h / 2);

	//do {	// choose SW or HW renderer?
	//	int i, n = SDL_GetNumRenderDrivers();
	//	SDL_RendererInfo info;
	//	for (i = 0; i < n; i++) {
	//		if (SDL_GetRenderDriverInfo(i, &info) < 0)
	//			continue;
	//		i, info.name,
	//			info.flags & SDL_RENDERER_SOFTWARE ? "SW" : "",
	//			info.flags & SDL_RENDERER_ACCELERATED ? "HW" : "",
	//			info.flags & SDL_RENDERER_PRESENTVSYNC ? ",vsync" : "",
	//			info.flags & SDL_RENDERER_TARGETTEXTURE ? ",texture" : "");
	//			if (info.flags & SDL_RENDERER_ACCELERATED)
	//				renderer_flags = SDL_RENDERER_ACCELERATED;
	//	}
	//} while (0);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STREAMING, w, h);

	//Uint32 myEventType = SDL_RegisterEvents(1);
	//if (myEventType != ((Uint32)-1)) {
	//	SDL_Event event;
	//	SDL_memset(&event, 0, sizeof(event)); /* or SDL_zero(event) */
	//	event.type = myEventType;
	//	event.user.code = 1;
	//	event.user.data1 = (char*)"data";
	//	event.user.data2 = 0;
	//	SDL_PushEvent(&event);
	//}


	SDL_EventState(SDL_RENDER_DEVICE_RESET, 1);
	SDL_EventState(SDL_RENDER_TARGETS_RESET, 1);

	while (true)
	{
		/*if (SDL_WaitEvent(&event)) {
			ProcessEvent(&event);

			if (event.type == SDL_QUIT)
				break;
		}*/
	}
	
	//RegisterHotKey(NULL, 26000, 0,VK_F5);
	//HMODULE hInstance = GetModuleHandle(NULL);
	//hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInstance, NULL);

	

	

	SDL_Quit();

	MSG msg;
	GetMessage(&msg, NULL, NULL, NULL);
	UnhookWindowsHookEx(hHook);

	return 0;
}