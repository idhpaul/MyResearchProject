#include <windows.h>
#include <tchar.h>
#include <crtdbg.h>
#include <stdio.h>

LRESULT CALLBACK LLKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{

	KBDLLHOOKSTRUCT* info = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

	// do not filter non-action events nor immune keys
	if (nCode == HC_ACTION) {
		WPARAM wParam = info->vkCode;
		LPARAM lParam = 1;								// repeat code
		lParam |= (info->scanCode << 16);				// scan code
		if (info->flags & LLKHF_EXTENDED) {
			lParam |= (1lu << 24);						// extended key
			printf("확장키 입력\n");
		}
		if (info->flags & LLKHF_ALTDOWN) {
			lParam |= (1lu << 29);						// context code
			printf("Alt 눌림(SYSKEY)\n");
		}
		if (info->flags & LLKHF_UP) {
			lParam |= (1lu << 31);						// transition
			printf("KEY UP\n");
		}
		else
		{
			printf("KEY DOWN\n");
		}
		
		// FIXME -- bit 30 should be set if key was already down but
		// we don't know that info.  as a result we'll never generate
		// key repeat events.

		// handle the message
		/*if (keyboardHookHandler(wParam, lParam)) {
			return 1;
		}*/
	}
	
	return CallNextHookEx(NULL, nCode, wParam, lParam);
	//PKBDLLHOOKSTRUCT hookstruct;

	//if (nCode == HC_ACTION)
	//{
	//	switch (wParam)
	//	{
	//	case WM_KEYDOWN: case WM_SYSKEYDOWN:
	//	case WM_KEYUP: case WM_SYSKEYUP:

	//		hookstruct = (PKBDLLHOOKSTRUCT)lParam;

	//		if (hookstruct->vkCode == 91) /* pesky Windows button */
	//		{
	//			printf("press window key\n");
	//			MessageBox(NULL, L"windowkey", L"테스트", MB_OK);

	//			return 1;
	//		}

	//		if ((hookstruct->vkCode == 0x09) && (hookstruct->flags == 0x20))
	//		{
	//			printf("press Alt + tab\n");
	//			MessageBox(NULL, L" Alt + tab", L"테스트", MB_OK);


	//			return 1;
	//		}

	//		if ((hookstruct->vkCode == 0x1B) && (hookstruct->flags == 0x20))
	//		{
	//			printf("press Alt + esc\n");
	//			MessageBox(NULL, L"Alt + esc", L"테스트", MB_OK);


	//			return 1;
	//		}
	//		if ((hookstruct->vkCode == 0x1B) && (hookstruct->flags == 0x00))
	//		{
	//			printf("press Ctrl+Esc\n");
	//			MessageBox(NULL, L"Ctrl+Esc", L"테스트", MB_OK);


	//			return 1;
	//		}
	//		if ((hookstruct->vkCode == 0x73) && (hookstruct->flags == 0x20))
	//		{
	//			printf("press Alt+F4\n");
	//			MessageBox(NULL, L" Alt+F4wkey다.", L"테스트", MB_OK);


	//			return 1;
	//		}
	//		if ((hookstruct->vkCode == 0x5B) && (hookstruct->flags == 0x01))
	//		{
	//			printf("press Left Windows Key\n");
	//			MessageBox(NULL, L" Left Windows Key", L"테스트", MB_OK);


	//			return 1;
	//		}
	//		if ((hookstruct->vkCode == 0x5C) && (hookstruct->flags == 0x01))
	//		{
	//			printf("press Right Windows Key\n");
	//			MessageBox(NULL, L"Right Windows Key", L"테스트", MB_OK);


	//			return 1;
	//		}

	//		else
	//			return CallNextHookEx(NULL, nCode, wParam, lParam);
	//	}
	//}
	//return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HHOOK hook_keys;
	int keycode;

	switch (uMsg)
	{
	case WM_CREATE:
		hook_keys = SetWindowsHookEx(WH_KEYBOARD_LL,
			LLKeyboardProc, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		return 0;

	case WM_DESTROY:
		UnhookWindowsHookEx(hook_keys);
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevIns, LPSTR lpszArgument, int iShow)
{
	TCHAR szClassName[] = _T("Template");
	TCHAR szWindowName[] = _T("Template");
	WNDCLASSEX wc = { 0 };
	MSG messages;
	HWND hWnd;

	wc.lpszClassName = szClassName;
	wc.lpfnWndProc = WindowProc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hbrBackground = (HBRUSH)COLOR_BTNSHADOW;
	wc.hInstance = hInstance;

	_ASSERTE(RegisterClassEx(&wc) != 0);

	hWnd = CreateWindowEx(0, szClassName, szWindowName, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		HWND_DESKTOP, 0, hInstance, 0);

	_ASSERTE(::IsWindow(hWnd));

	ShowWindow(hWnd, SW_SHOWMAXIMIZED);
	while (GetMessage(&messages, NULL, 0, 0))
	{
		TranslateMessage(&messages);
		DispatchMessage(&messages);
	}

	return static_cast<int>(messages.wParam);

}