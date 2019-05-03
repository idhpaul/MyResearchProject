#include "SDL.h"

#ifdef main
#undef main
#endif

#define	INVALID_KEY	0xffff
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

sdlmsg_t *
sdlmsg_keyboard(sdlmsg_t *msg, unsigned char pressed, unsigned short scancode, SDL_Keycode key, unsigned short mod, unsigned int unicode)
{
	sdlmsg_keyboard_t *msgk = (sdlmsg_keyboard_t*)msg;
	//printf("sdl client: key event code=%x key=%x mod=%x pressed=%u\n", scancode, key, mod, pressed);
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

	//printf("%d\n", key);

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
