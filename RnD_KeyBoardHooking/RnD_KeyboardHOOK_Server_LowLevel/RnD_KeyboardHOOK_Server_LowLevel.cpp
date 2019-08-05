
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

#include <processthreadsapi.h>

#pragma comment(lib, "Ws2_32.lib")

#include <stdio.h>
#include <iostream>

HHOOK hHook;

#define __SOCKET 1
#define __BARRIER 1
#define __TEST_SEND 1

#if __SOCKET

SOCKET sock;
SOCKET clientsock;

struct sockaddr_in sockinfo;
struct sockaddr_in clientsockinfo;

typedef struct HOOK {
	DWORD   vkCode;
	DWORD   scanCode;
	DWORD   flags;
	DWORD   time;
	ULONG_PTR dwExtraInfo;
	bool	keyPress;
	bool	controlPress;
	bool	shiftPress;
	bool	altPress;
	bool	winPress;
} MYHOOK;

#endif

#if __BARRIER

#include "key_types.h"

enum EHookMode {
	kHOOK_DISABLE,
	kHOOK_WATCH_JUMP_ZONE,
	kHOOK_RELAY_EVENTS
};

static EHookMode        g_mode = kHOOK_DISABLE;

static WPARAM            g_deadVirtKey = 0;
static WPARAM            g_deadRelease = 0;
static LPARAM            g_deadLParam = 0;

static BYTE                g_deadKeyState[256] = { 0 };
static BYTE                g_keyState[256] = { 0 };
#endif

#if __BARRIER

#if __TEST_SEND

typedef struct TESTHOOK {
	DWORD   vkCode;
	BOOL	key_up;
} TESTHOOK;

#endif



//IKeyState* getKeyState() const
//{
//	return m_keyState;
//}
//
//bool isKeyDown(KeyButton button)
//{
//	return getKeyState()->isKeyDown(button);
//}
//
//bool onKey(WPARAM wParam, LPARAM lParam)
//{
//	static const KeyModifierMask s_ctrlAlt =
//		KeyModifierControl | KeyModifierAlt;
//
//	printf("event: Key char=%d, vk=0x%02x, nagr=%d, lParam=0x%08x", (wParam & 0xff00u) >> 8, wParam & 0xffu, (wParam & 0x10000u) ? 1 : 0, lParam);
//
//	// get event info
//	KeyButton button = (KeyButton)((lParam & 0x01ff0000) >> 16);
//	bool down = ((lParam & 0x80000000u) == 0x00000000u);
//	bool wasDown = isKeyDown(button);
//	KeyModifierMask oldState = pollActiveModifiers();
//
//	// check for autorepeat
//	if (m_keyState->testAutoRepeat(down, (lParam & 0x40000000u), button)) {
//		lParam |= 0x40000000u;
//	}
//
//	// if the button is zero then guess what the button should be.
//	// these are badly synthesized key events and logitech software
//	// that maps mouse buttons to keys is known to do this.
//	// alternatively, we could just throw these events out.
//	if (button == 0) {
//		button = m_keyState->virtualKeyToButton(wParam & 0xffu);
//		if (button == 0) {
//			return true;
//		}
//		wasDown = isKeyDown(button);
//	}
//
//	// record keyboard state
//	m_keyState->onKey(button, down, oldState);
//
//	if (!down && m_isPrimary && !m_isOnScreen) {
//		PrimaryKeyDownList::iterator find = std::find(m_primaryKeyDownList.begin(), m_primaryKeyDownList.end(), button);
//		if (find != m_primaryKeyDownList.end()) {
//			LOG((CLOG_DEBUG1 "release key button %d on primary", *find));
//			m_hook.setMode(kHOOK_WATCH_JUMP_ZONE);
//			fakeLocalKey(*find, false);
//			m_primaryKeyDownList.erase(find);
//			m_hook.setMode(kHOOK_RELAY_EVENTS);
//			return true;
//		}
//	}
//
//	// windows doesn't tell us the modifier key state on mouse or key
//	// events so we have to figure it out.  most apps would use
//	// GetKeyState() or even GetAsyncKeyState() for that but we can't
//	// because our hook doesn't pass on key events for several modifiers.
//	// it can't otherwise the system would interpret them normally on
//	// the primary screen even when on a secondary screen.  so tapping
//	// alt would activate menus and tapping the windows key would open
//	// the start menu.  if you don't pass those events on in the hook
//	// then GetKeyState() understandably doesn't reflect the effect of
//	// the event.  curiously, neither does GetAsyncKeyState(), which is
//	// surprising.
//	//
//	// so anyway, we have to track the modifier state ourselves for
//	// at least those modifiers we don't pass on.  pollActiveModifiers()
//	// does that but we have to update the keyboard state before calling
//	// pollActiveModifiers() to get the right answer.  but the only way
//	// to set the modifier state or to set the up/down state of a key
//	// is via onKey().  so we have to call onKey() twice.
//	KeyModifierMask state = pollActiveModifiers();
//	m_keyState->onKey(button, down, state);
//
//	// check for hot keys
//	if (oldState != state) {
//		// modifier key was pressed/released
//		if (onHotKey(0, lParam)) {
//			return true;
//		}
//	}
//	else {
//		// non-modifier was pressed/released
//		if (onHotKey(wParam, lParam)) {
//			return true;
//		}
//	}
//
//	// stop sending modifier keys over and over again
//	if (isModifierRepeat(oldState, state, wParam)) {
//		return true;
//	}
//
//	// ignore message if posted prior to last mark change
//	if (!ignore()) {
//		// check for ctrl+alt+del.  we do not want to pass that to the
//		// client.  the user can use ctrl+alt+pause to emulate it.
//		UINT virtKey = (wParam & 0xffu);
//		if (virtKey == VK_DELETE && (state & s_ctrlAlt) == s_ctrlAlt) {
//			LOG((CLOG_DEBUG "discard ctrl+alt+del"));
//			return true;
//		}
//
//		// check for ctrl+alt+del emulation
//		if ((virtKey == VK_PAUSE || virtKey == VK_CANCEL) &&
//			(state & s_ctrlAlt) == s_ctrlAlt) {
//			LOG((CLOG_DEBUG "emulate ctrl+alt+del"));
//			// switch wParam and lParam to be as if VK_DELETE was
//			// pressed or released.  when mapping the key we require that
//			// we not use AltGr (the 0x10000 flag in wParam) and we not
//			// use the keypad delete key (the 0x01000000 flag in lParam).
//			wParam = VK_DELETE | 0x00010000u;
//			lParam &= 0xfe000000;
//			lParam |= m_keyState->virtualKeyToButton(wParam & 0xffu) << 16;
//			lParam |= 0x01000001;
//		}
//
//		// process key
//		KeyModifierMask mask;
//		KeyID key = m_keyState->mapKeyFromEvent(wParam, lParam, &mask);
//		button = static_cast<KeyButton>((lParam & 0x01ff0000u) >> 16);
//		if (key != kKeyNone) {
//			// do it
//			m_keyState->sendKeyEvent(getEventTarget(),
//				((lParam & 0x80000000u) == 0),
//				((lParam & 0x40000000u) != 0),
//				key, mask, (SInt32)(lParam & 0xffff), button);
//		}
//		else {
//			LOG((CLOG_DEBUG1 "cannot map key"));
//		}
//	}
//
//	return true;
//}
//
//bool onHotKey(WPARAM wParam, LPARAM lParam)
//{
//	// get the key info
//	KeyModifierMask state = getActiveModifiers();
//	UINT virtKey = (wParam & 0xffu);
//	UINT modifiers = 0;
//	if ((state & KeyModifierShift) != 0) {
//		modifiers |= MOD_SHIFT;
//	}
//	if ((state & KeyModifierControl) != 0) {
//		modifiers |= MOD_CONTROL;
//	}
//	if ((state & KeyModifierAlt) != 0) {
//		modifiers |= MOD_ALT;
//	}
//	if ((state & KeyModifierSuper) != 0) {
//		modifiers |= MOD_WIN;
//	}
//
//	// find the hot key id
//	HotKeyToIDMap::const_iterator i =
//		m_hotKeyToIDMap.find(HotKeyItem(virtKey, modifiers));
//	if (i == m_hotKeyToIDMap.end()) {
//		return false;
//	}
//
//	// find what kind of event
//	Event::Type type;
//	if ((lParam & 0x80000000u) == 0u) {
//		if ((lParam & 0x40000000u) != 0u) {
//			// ignore key repeats but it counts as a hot key
//			return true;
//		}
//		type = m_events->forIPrimaryScreen().hotKeyDown();
//	}
//	else {
//		type = m_events->forIPrimaryScreen().hotKeyUp();
//	}
//
//	// generate event
//	m_events->addEvent(Event(type, getEventTarget(),
//		HotKeyInfo::alloc(i->second)));
//
//	return true;
//}


static
WPARAM
makeKeyMsg(UINT virtKey, char c, bool noAltGr)
{
	return MAKEWPARAM(MAKEWORD(virtKey & 0xff, (BYTE)c), noAltGr ? 1 : 0);
}

static
void
keyboardGetState(BYTE keys[256], DWORD vkCode, bool kf_up)
{
	// we have to use GetAsyncKeyState() rather than GetKeyState() because
	// we don't pass through most keys so the event synchronous state
	// doesn't get updated.  we do that because certain modifier keys have
	// side effects, like alt and the windows key.
	// 우리는 대부분의 키를 통과하지 않기 때문에 GetKeyState() 
	// 대신 GetAsyncKeyState()를 사용해야하므로 이벤트 동기 상태가 
	// 업데이트되지 않습니다.특정 수정 자 키가 alt와 windows 키와 같은 
	// 부작용을 가지고 있기 때문에 그렇게합니다.
	if (vkCode < 0 || vkCode >= 256) {
		return;
	}

	// Keep track of key state on our own in case GetAsyncKeyState() fails
	// GetAsyncKeyState ()가 실패 할 경우를 대비하여 키 상태를 추적하십시오.
	g_keyState[vkCode] = kf_up ? 0 : 0x80;
	g_keyState[VK_SHIFT] = g_keyState[VK_LSHIFT] | g_keyState[VK_RSHIFT];

	SHORT key;
	// Test whether GetAsyncKeyState() is being honest with us
	// GetAsyncKeyState ()가 우리에게 정직한 지 테스트하십시오.
	key = GetAsyncKeyState(vkCode);

	if (key & 0x80) {
		// The only time we know for sure that GetAsyncKeyState() is working
		// is when it tells us that the current key is down.
		// In this case, update g_keyState to reflect what GetAsyncKeyState()
		// is telling us, just in case we have gotten out of sync
		// 우리가 GetAsyncKeyState()가 작동하는지 알 수있는 유일한 시간은 
		// 현재 키가 작동 중지되었음을 알려주는 것입니다.
		// 이 경우 g_keyState를 업데이트하여 GetAsyncKeyState()가 
		// 알려주는 내용을 반영하십시오.

		for (int i = 0; i < 256; ++i) {
			key = GetAsyncKeyState(i);
			g_keyState[i] = (BYTE)((key < 0) ? 0x80u : 0);
		}
	}

	// copy g_keyState to keys
	// g_key 키로 복사
	for (int i = 0; i < 256; ++i) {
		keys[i] = g_keyState[i];
	}

	key = GetKeyState(VK_CAPITAL);
	keys[VK_CAPITAL] = (BYTE)(((key < 0) ? 0x80 : 0) | (key & 1));
	/*key = GetKeyState(VK_HANJA);
	keys[VK_HANJA] = (BYTE)(((key < 0) ? 0x80 : 0) | (key & 1));
	key = GetKeyState(VK_HANGUL);
	keys[VK_HANGUL] = (BYTE)(((key < 0) ? 0x80 : 0) | (key & 1));*/
}
static
bool
keyboardHookHandler(WPARAM wParam, LPARAM lParam)
{
	DWORD vkCode = static_cast<DWORD>(wParam);
	bool kf_up = (lParam & (KF_UP << 16)) != 0;

	// check for special events indicating if we should start or stop
	// passing events through and not report them to the server.  this
	// is used to allow the server to synthesize events locally but
	// not pick them up as user events.

	// 이벤트를 시작하거나 중지해야하는지 여부를 나타내는 특수 이벤트를 확인하고 
	// 이벤트를 서버에보고하지 마십시오.이는 서버가 이벤트를 로컬로 합성하지만
	//	사용자 이벤트로 가져 오지 못하게하는 데 사용됩니다.
	//if (wParam == BARRIER_HOOK_FAKE_INPUT_VIRTUAL_KEY &&
	//	((lParam >> 16) & 0xffu) == BARRIER_HOOK_FAKE_INPUT_SCANCODE) {
	//	// update flag
	//	g_fakeServerInput = ((lParam & 0x80000000u) == 0);
	//	PostThreadMessage(g_threadID, BARRIER_MSG_DEBUG,
	//		0xff000000u | wParam, lParam);

	//	// discard event
	//	return true;
	//}


	// VK_RSHIFT may be sent with an extended scan code but right shift
	// is not an extended key so we reset that bit.
	//VK_RSHIFT는 확장 스캔 코드와 함께 전송 될 수 있지만 
	//오른쪽 시프트는 확장 키가 아니므로 해당 비트를 재설정합니다.

	if (wParam == VK_RSHIFT) {
		lParam &= ~0x01000000u;
	}

	// tell server about event
	//PostThreadMessage(g_threadID, BARRIER_MSG_DEBUG, wParam, lParam);
	//printf("[%d]hook: 0x%08x 0x%08x\n", __LINE__,wParam, lParam);

#if 0
	//! 한국어 키보드에는 데드키가 없음
	// ignore dead key release
	if ((g_deadVirtKey == wParam || g_deadRelease == wParam) &&
		(lParam & 0x80000000u) != 0) {
		g_deadRelease = 0;

		printf("[ignore dead key release]");
		printf("[%d]hook: 0x%08x 0x%08x\n", __LINE__, wParam | 0x04000000, lParam);

		return false;
	}
#endif

	// we need the keyboard state for ToAscii()
	BYTE keys[256];
	keyboardGetState(keys, vkCode, kf_up);

	/*for (int i = 0; i < 256; ++i)
	{
		printf("%d ", g_keyState[i]);
	}
	printf("\n↑g_keyState\n");

	for (int i = 0; i < 256; ++i)
	{
		printf("%d ", keys[i]);
	}
	printf("\n↑keys\n");

	return true;*/

	// ToAscii() maps ctrl+letter to the corresponding control code
	// and ctrl+backspace to delete.  we don't want those translations
	// so clear the control modifier state.  however, if we want to
	// simulate AltGr (which is ctrl+alt) then we must not clear it.
	// ToAscii()는 Ctrl + 문자를 해당 제어 코드에 매핑하고 
	// Ctrl + Backspace를 삭제합니다.
	// 우리는 이러한 변환이 컨트롤 수정 자 상태를 너무 명확하게하기를 
	// 원하지 않습니다.그러나 AltGr(ctrl + alt)을 시뮬레이트하려면
	// 이를 지워서는 안됩니다.
	UINT control = keys[VK_CONTROL] | keys[VK_LCONTROL] | keys[VK_RCONTROL];
	UINT menu = keys[VK_MENU] | keys[VK_LMENU] | keys[VK_RMENU];
	if ((control & 0x80) == 0 || (menu & 0x80) == 0) {
		keys[VK_LCONTROL] = 0;
		keys[VK_RCONTROL] = 0;
		keys[VK_CONTROL] = 0;
		printf("Control up\n");
	}
	else {
		keys[VK_LCONTROL] = 0x80;
		keys[VK_RCONTROL] = 0x80;
		keys[VK_CONTROL] = 0x80;
		keys[VK_LMENU] = 0x80;
		keys[VK_RMENU] = 0x80;
		keys[VK_MENU] = 0x80;
		printf("Control & Alt down\n");
	}



	// ToAscii() needs to know if a menu is active for some reason.
	// we don't know and there doesn't appear to be any way to find
	// out.  so we'll just assume a menu is active if the menu key
	// is down.
	// ToAscii()는 어떤 이유로 메뉴가 활성화되어 있는지 알아야합니다.
	// 우리는 몰라요.알아내는 방법이없는 것처럼 보입니다.
	// 메뉴 키가 눌러져 있으면 메뉴가 활성 상태라고 가정합니다.
	// FIXME -- figure out some way to check if a menu is active
	// FIXME - 메뉴가 활성화되어 있는지 확인하는 방법 알아보기
	UINT flags = 0;
	if ((menu & 0x80) != 0)
		flags |= 1;

	// if we're on the server screen then just pass numpad keys with alt
	// key down as-is.  we won't pick up the resulting character but the
	// local app will.  if on a client screen then grab keys as usual;
	// if the client is a windows system it'll synthesize the expected
	// character.  if not then it'll probably just do nothing.
	// 우리가 서버 화면에 있다면 Alt 키를 누른 상태로 숫자 패드 키를 
	// 그대로 전달하면됩니다.우리는 결과 캐릭터를 선택하지는 않지만 
	// 로컬 앱은 가져올 것입니다.클라이언트 화면에 있다면 
	// 평소처럼 키를 잡으십시오.클라이언트가 Windows 시스템 인 경우 
	// 예상되는 문자를 합성합니다.그렇지 않다면 아마 아무 것도하지 않을 것입니다.
	if (g_mode != kHOOK_RELAY_EVENTS) {
		// we don't use virtual keys because we don't know what the
		// state of the numlock key is.  we'll hard code the scan codes
		// instead.  hopefully this works across all keyboards.
		// 우리는 numlock 키의 상태를 모르기 때문에 가상 키를 사용하지 않습니다.
		// 대신에 스캔 코드를 하드 코딩 할 것입니다.
		// 잘하면이 모든 키보드에서 작동합니다.
		UINT sc = (lParam & 0x01ff0000u) >> 16;
		if (menu &&
			(sc >= 0x47u && sc <= 0x52u && sc != 0x4au && sc != 0x4eu)) {
			return false;
		}
	}

	WORD c = 0;
#if 0
	//! 한국어 키보드에는 데드키가 없음
	// map the key event to a character.  we have to put the dead
	// key back first and this has the side effect of removing it.
	// 키 이벤트를 문자에 매핑합니다.우리는 먼저 죽은 키를 다시 
	// 넣어야하고이 키를 제거하는 부작용이 있습니다.
	if (g_deadVirtKey != 0) {
		if (ToAscii((UINT)g_deadVirtKey, (g_deadLParam & 0x10ff0000u) >> 16,
			g_deadKeyState, &c, flags) == 2) {
			// If ToAscii returned 2, it means that we accidentally removed
			// a double dead key instead of restoring it. Thus, we call
			// ToAscii again with the same parameters to restore the
			// internal dead key state.
			// ToAscii가 2를 반환하면 실수로 이중 고정 키를 복원하는 
			// 대신 실수로 제거한 것입니다. 따라서 동일한 매개 변수로 
			// ToAscii를 다시 호출하여 내부 데드 키 상태를 복원합니다.
			ToAscii((UINT)g_deadVirtKey, (g_deadLParam & 0x10ff0000u) >> 16,
				g_deadKeyState, &c, flags);

			// We need to keep track of this because g_deadVirtKey will be
			// cleared later on; this would cause the dead key release to
			// incorrectly restore the dead key state.
			// g_deadVirtKey가 나중에 지워지므로이를 추적해야합니다. 
			// 이로 인해 데드 키 릴리스가 데드 키 상태를 잘못 복원하게됩니다.
			g_deadRelease = g_deadVirtKey;
		}
	}
#endif

	UINT scanCode = ((lParam & 0x10ff0000u) >> 16);
	int n = ToAscii((UINT)wParam, scanCode, keys, &c, flags);


	// if mapping failed and ctrl and alt are pressed then try again
	// with both not pressed.  this handles the case where ctrl and
	// alt are being used as individual modifiers rather than AltGr.
	// we note that's the case in the message sent back to barrier
	// because there's no simple way to deduce it after the fact.
	// we have to put the dead key back first, if there was one.
	// 매핑이 실패하고 Ctrl과 Alt 키를 누른 경우 다시 누르지 않은 상태에서 
	// 다시 시도하십시오. 이것은 ctrl과 alt가 AltGr보다는 개별 수식어로 
	// 사용되는 경우를 처리합니다. 우리는 그것이 사실 이후에 그것을 
	// 추론 할 수있는 간단한 방법이 없기 때문에 장벽으로 되돌려 
	// 보내진 메시지의 경우에 주목합니다. 우리는 죽은 열쇠가 있다면 
	// 먼저 돌려 놓아야합니다.
	bool noAltGr = false;
	if (n == 0 && (control & 0x80) != 0 && (menu & 0x80) != 0) {
		noAltGr = true;

		//printf("[%d]hook: 0x%08x 0x%08x\n",__LINE__, wParam | 0x05000000, lParam);

#if 0
		//! 한국어 키보드에는 데드키가 없음
		if (g_deadVirtKey != 0) {
			if (ToAscii((UINT)g_deadVirtKey, (g_deadLParam & 0x10ff0000u) >> 16,
				g_deadKeyState, &c, flags) == 2) {
				ToAscii((UINT)g_deadVirtKey, (g_deadLParam & 0x10ff0000u) >> 16,
					g_deadKeyState, &c, flags);
				g_deadRelease = g_deadVirtKey;
			}
		}
#endif

		BYTE keys2[256];
		for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i) {
			keys2[i] = keys[i];
		}
		keys2[VK_LCONTROL] = 0;
		keys2[VK_RCONTROL] = 0;
		keys2[VK_CONTROL] = 0;
		keys2[VK_LMENU] = 0;
		keys2[VK_RMENU] = 0;
		keys2[VK_MENU] = 0;
		n = ToAscii((UINT)wParam, scanCode, keys2, &c, flags);
	}


	//printf("[%d]hook: 0x%08x 0x%08x\n", __LINE__,wParam | ((c & 0xff) << 8) | ((n & 0xff) << 16) | 0x06000000, lParam);

	WPARAM charAndVirtKey = 0;
	bool clearDeadKey = false;
	switch (n) {
	default:
		// key is a dead key

		if (lParam & 0x80000000u)
			// This handles the obscure situation where a key has been
			// pressed which is both a dead key and a normal character
			// depending on which modifiers have been pressed. We
			// break here to prevent it from being considered a dead
			// key.
			// 이것은 눌려진 수정 자에 따라 데드 키와 일반 문자 둘다 키가 
			// 눌러 진 모호한 상황을 처리합니다.우리는 그것이 죽은 열쇠로 
			// 간주되지 않도록 여기에서 휴식을 취합니다.
			break;

		g_deadVirtKey = wParam;
		g_deadLParam = lParam;
		for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i) {
			g_deadKeyState[i] = keys[i];
		}
		break;

	case 0:
		// key doesn't map to a character.  this can happen if
		// non-character keys are pressed after a dead key.
		printf("case 0\n");
		charAndVirtKey = makeKeyMsg((UINT)wParam, (char)0, noAltGr);
		break;

	case 1:
		printf("case 1\n");
		// key maps to a character composed with dead key
		charAndVirtKey = makeKeyMsg((UINT)wParam, (char)LOBYTE(c), noAltGr);
		clearDeadKey = true;
		break;

	case 2: {
		printf("case 2\n");
		// previous dead key not composed.  send a fake key press
		// and release for the dead key to our window.
		WPARAM deadCharAndVirtKey =
			makeKeyMsg((UINT)g_deadVirtKey, (char)LOBYTE(c), noAltGr);

		// use uncomposed character
		charAndVirtKey = makeKeyMsg((UINT)wParam, (char)HIBYTE(c), noAltGr);
		clearDeadKey = true;
		break;
	}
	}

#if 0
	//! 한국어 키보드에는 데드키가 없음
	// put back the dead key, if any, for the application to use
	if (g_deadVirtKey != 0) {
		ToAscii((UINT)g_deadVirtKey, (g_deadLParam & 0x10ff0000u) >> 16,
			g_deadKeyState, &c, flags);
	}

	// clear out old dead key state
	if (clearDeadKey) {
		g_deadVirtKey = 0;
		g_deadLParam = 0;
	}
#endif

	printf("wParam : %x\n", wParam);
	printf("c : %c\n", (char)LOBYTE(c));
	printf("noAltGr : %d\n", noAltGr);
	printf("charAndVirtKey 0x%x\n", charAndVirtKey);

#if __TEST_SEND 1

	TESTHOOK hook = { 0, };
	hook.vkCode = static_cast<DWORD>(wParam);
	hook.key_up = kf_up;

	printf("hook.vkCode : %x\n", hook.vkCode);
	printf("hook.key_up = %d\n", hook.key_up);


#if __SOCKET
	if (send(clientsock, (char*)&hook, sizeof(TESTHOOK), 0))
	{
		std::cout << "SEND OK" << std::endl;
		return 1;
	}
	else
	{
		std::cout << "ERR Send  : " << WSAGetLastError() << std::endl;
	}
#endif

#endif

	// forward message to our window.  do this whether or not we're
	// forwarding events to clients because this'll keep our thread's
	// key state table up to date.  that's important for querying
	// the scroll lock toggle state.
	// XXX -- with hot keys for actions we may only need to do this when
	// forwarding.
	if (charAndVirtKey != 0) {
		//PostThreadMessage(g_threadID, BARRIER_MSG_DEBUG,
		//	charAndVirtKey | 0x07000000, lParam);
		printf("hook: 0x%08x 0x%08x\n",
			charAndVirtKey | 0x07000000, lParam);

		//PostThreadMessage(g_threadID, BARRIER_MSG_KEY, charAndVirtKey, lParam);
		//onKey(charAndVirtKey, lParam);

		return true;


	}

	if (g_mode == kHOOK_RELAY_EVENTS) {
		// let certain keys pass through
		switch (wParam) {
		case VK_CAPITAL:
		case VK_NUMLOCK:
		case VK_SCROLL:
			// pass event on.  we want to let these through to
			// the window proc because otherwise the keyboard
			// lights may not stay synchronized.
		case VK_HANGUL:
			// pass event on because we're using a low level hook

			break;

		default:
			// discard
			return true;
		}
	}

	return false;

}
#endif

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
#if __BARRIER
	// decode the message
	KBDLLHOOKSTRUCT* info = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

	// do not filter non-action events nor immune keys
	if (nCode == HC_ACTION) {


		WPARAM wParam = info->vkCode;
		LPARAM lParam = 1;								// repeat code
		lParam |= (info->scanCode << 16);				// scan code
		if (info->flags & LLKHF_EXTENDED) {
			lParam |= (1lu << 24);						// extended key
			printf("[확장키 입력]\n");
		}
		if (info->flags & LLKHF_ALTDOWN) {
			lParam |= (1lu << 29);						// context code
			printf("[Alt 눌림(SYSKEY)]\n");
		}
		if (info->flags & LLKHF_UP) {
			lParam |= (1lu << 31);						// transition
			printf("[KEY UP]\n");
		}
		else
		{
			printf("[KEY DOWN]\n");
		}


		// FIXME -- bit 30 should be set if key was already down but
		// we don't know that info.  as a result we'll never generate
		// key repeat events.

		// handle the message
		if (keyboardHookHandler(wParam, lParam)) {
			return 1;
		}
	}

	return CallNextHookEx(hHook, nCode, wParam, lParam);

#endif

	//if (nCode < 0 || nCode != HC_ACTION)
	//	return CallNextHookEx(hHook, nCode, wParam, lParam);

	//PKBDLLHOOKSTRUCT hookstruct;

	//hookstruct = (PKBDLLHOOKSTRUCT)lParam;

	//MYHOOK myhook = { 0 ,};
	//
	//bool blnEat = false;


	//if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
	//{
	//	myhook.keyPress = true;

	//	//return 1;
	//	//if(GetAsyncKeyState(VK_LSHIFT) & 0x8000 || GetAsyncKeyState(VK_RSHIFT) & 0x8000)
	//}
	//else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
	//{
	//	myhook.keyPress = false;
	//}

	//if (GetAsyncKeyState(VK_LCONTROL) & 0x8000 || GetAsyncKeyState(VK_RCONTROL) & 0x8000)
	//{
	//	//WM_KEYDOWN
	//	std::cout << "@Control Press" << std::endl;
	//	myhook.controlPress = true;
	//}
	//if (GetAsyncKeyState(VK_SHIFT) & 0x8000 || GetAsyncKeyState(VK_RSHIFT) & 0x8000)
	//{
	//	//WM_KEYDOWN
	//	std::cout << "@Shift Press" << std::endl;

	//	myhook.shiftPress = true;
	//}
	//if (GetAsyncKeyState(VK_LWIN) & 0x8000 || GetAsyncKeyState(VK_RWIN) & 0x8000)
	//{
	//	//WM_KEYDOWN
	//	std::cout << "@Win Press" << std::endl;

	//	myhook.winPress = true;
	//}
	//if (GetAsyncKeyState(VK_LMENU) & 0x8000 || GetAsyncKeyState(VK_RMENU) & 0x8000)
	//{
	//	//WM_SYSKEYDOWN
	//	std::cout << "@Alt Press" << std::endl;

	//	myhook.altPress = true;
	//}

	//std::cout << "key State : " << myhook.keyPress
	//	<< "\t vkcode : " << hookstruct->vkCode 
	//	<< "\t flag : " << hookstruct->flags << std::endl;

	////printf("----------------------\n");
	////printf("vkcode : %x\n", hookstruct->vkCode);
	////printf("scancode : %x\n", hookstruct->scanCode);
	////printf("flag : %x\n", hookstruct->flags);
#if __SOCKET
	//memcpy(&myhook, (char*)hookstruct, sizeof(KBDLLHOOKSTRUCT));

	//if (send(clientsock, (char*)&myhook, sizeof(MYHOOK), 0))
	//{
	//	std::cout << "SEND OK" << std::endl;
	//	return 1;
	//}
	//else
	//{
	//	std::cout << "ERR Send  : " << WSAGetLastError() << std::endl;
	//}
#else
	//return 1;
#endif


	//switch (wParam)
	//{
	//case WM_KEYDOWN:
	//	//printf("WM_KEYDOWN\n");

	//case WM_KEYUP:
	//	//printf("WM_KEYUP\n");

	//case WM_SYSKEYDOWN:
	//	//printf("WM_SYSKEYDOWN\n");

	//case WM_SYSKEYUP:
	//	//printf("WM_SYSKEYUP\n");

	//	if ((hookstruct->vkCode == VK_TAB) && (hookstruct->flags == LLKHF_ALTDOWN))
	//	{
	//		printf("[hook]alt + tab\n");
	//		blnEat = true;
	//		break;
	//	}
	//	if ((hookstruct->vkCode == VK_ESCAPE) && (hookstruct->flags == LLKHF_ALTDOWN))
	//	{
	//		printf("[hook]alt + esc\n");
	//		blnEat = true;
	//		break;
	//	}
	//	if ((hookstruct->vkCode == VK_ESCAPE) && (hookstruct->flags == 0))
	//	{
	//		printf("[hook]ctrl + esc\n");
	//		blnEat = true;
	//		break;
	//	}
	//	if ((hookstruct->vkCode == VK_LWIN) && (hookstruct->flags == LLKHF_EXTENDED))
	//	{
	//		printf("[hook]Lwin\n");
	//		blnEat = true;
	//		break;
	//	}
	//	if ((hookstruct->vkCode == VK_RWIN) && (hookstruct->flags == LLKHF_EXTENDED))
	//	{
	//		printf("[hook]Rwin\n");
	//		blnEat = true;
	//		break;
	//	}
	//	if ((true) && (hookstruct->flags == LLKHF_ALTDOWN))
	//	{
	//		printf("[hook]alt + true\n");
	//		blnEat = true;
	//		break;
	//	}
	//}


	//if (blnEat)
	//{
	//	printf("blnEat is true\n");
	//	return 1;
	//}
	//else
	//{
	//	printf("----------------------\n");
	//	printf("vkcode : %x\n", hookstruct->vkCode);
	//	printf("scancode : %x\n", hookstruct->scanCode);
	//	printf("flag : %x\n", hookstruct->flags);
	//	printf("----------------------\n\n");

	//	
	//	memcpy(&myhook, (char*)hookstruct, sizeof(MYHOOK));

	//	send(clientsock, (char*)&myhook, sizeof(MYHOOK), 0);

	//	//return CallNextHookEx(hHook, nCode, wParam, lParam);

	//}

/////////////////////////////////////////////////////////////////////////////

	//PKBDLLHOOKSTRUCT hookstruct;

	//hookstruct = (PKBDLLHOOKSTRUCT)lParam;

	//if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
	//{

	//	SHORT nPressCrtl = GetAsyncKeyState(VK_CONTROL);
	//	SHORT nPressShift = GetAsyncKeyState(VK_SHIFT);
	//	SHORT nPressAlt = GetAsyncKeyState(VK_MENU);

	//	printf("nPressCrtl %d\n", nPressCrtl);
	//	printf("nPressShift %d\n", nPressShift);
	//	printf("nPressAlt %d\n", nPressAlt);

	//	if (hookstruct->vkCode == VK_HANGUL)
	//	{
	//		printf("input hangul\n");
	//		return 1;
	//	}
	//	else if (hookstruct->vkCode == VK_HANJA)
	//	{
	//		printf("input hanja\n");
	//		return 1;
	//	}
	//	else if (hookstruct->vkCode == VK_LWIN) /* pesky Windows button */
	//	{
	//		printf("input left win\n");
	//		return 1;
	//	}
	//	else if (hookstruct->vkCode == VK_RWIN) /* pesky Windows button */
	//	{
	//		printf("input right win\n");
	//	}
	//	else
	//	{
	//		printf("another key prees : %d\n", hookstruct->vkCode);
	//	}
	//}

	//CallNextHookEx(hHook, nCode, wParam, lParam);
}

int main(void)
{
#if __SOCKET
	WSADATA wsadata;
	int opt = 0;

	int clientsize;

	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
		printf("windows socket initialize failed\n");
		return -1;
	}

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		printf("failed to create socket");
		return -1;
	}


	memset(&sockinfo, 0, sizeof(sockinfo));

	sockinfo.sin_family = AF_INET;
	sockinfo.sin_addr.s_addr = htonl(INADDR_ANY);
	sockinfo.sin_port = htons(27015);

	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(opt));

	if (bind(sock, (SOCKADDR*)&sockinfo, sizeof(sockinfo)) == SOCKET_ERROR) {
		printf("PARMS bind() error");
		return -1;
	}

	if (listen(sock, SOMAXCONN) == SOCKET_ERROR) {
		printf("PARMS listen() error");
		return -1;
	}

	printf("Waiting for Client\n");

	clientsize = sizeof(clientsockinfo);

	clientsock = accept(sock, (SOCKADDR*)&clientsockinfo, &clientsize);
	if (clientsock == INVALID_SOCKET) {
		printf("PARMS accept() error");
		return -1;
	}
	else
	{
		printf("accept OK\n");
	}
#endif


	MSG messages;
	HMODULE hInstance = GetModuleHandle(NULL);

	//what is Set WindowsHookEx : https://yokang90.tistory.com/48
	//Keyboard_LowLevel : https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms644985(v=vs.85)
	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInstance, NULL);

	HRESULT r;
	r = SetThreadDescription(GetCurrentThread(), L"ThisIsMyThreadName");

	while (GetMessage(&messages, NULL, 0, 0))
	{
		TranslateMessage(&messages);
		DispatchMessage(&messages);
	}

#if __SOCKET
	closesocket(sock);
	closesocket(clientsock);

	WSACleanup();
#endif

	return 0;
}


//PKBDLLHOOKSTRUCT pKey = (PKBDLLHOOKSTRUCT)lParam;

//if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
//{

//	//Now just check pKey->vkCode etc for whatever you want
//	//for instance, a basic printout of the value and a check for pgup
//	if (pKey->vkCode == VK_HANGUL)
//	{
//		event.type = Hangul;
//		event.user.code = 1;
//		event.user.data1 = (int*)0x15;
//		event.user.data2 = 0;
//		SDL_PushEvent(&event);
//	}
//	else if (pKey->vkCode == VK_HANJA)
//	{
//		event.type = Hangul;
//		event.user.code = 2;
//		event.user.data1 = (int*)0x19;
//		event.user.data2 = 0;
//		SDL_PushEvent(&event);
//	}

//}
//CallNextHookEx(hHook, nCode, wParam, lParam);
//return 0;