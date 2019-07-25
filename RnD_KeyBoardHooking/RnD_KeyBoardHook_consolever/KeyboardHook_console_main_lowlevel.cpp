#include <stdio.h>

#include <Windows.h>

HHOOK hHook;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	PKBDLLHOOKSTRUCT hookstruct;

	hookstruct = (PKBDLLHOOKSTRUCT)lParam;

	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
	{

		SHORT nPressCrtl = GetAsyncKeyState(VK_CONTROL);
		SHORT nPressShift = GetAsyncKeyState(VK_SHIFT);
		SHORT nPressAlt = GetAsyncKeyState(VK_MENU);

		printf("nPressCrtl %d\n", nPressCrtl);
		printf("nPressShift %d\n", nPressShift);
		printf("nPressAlt %d\n", nPressAlt);

		if (hookstruct->vkCode == VK_HANGUL)
		{
			printf("input hangul\n");
			return 1;
		}
		else if (hookstruct->vkCode == VK_HANJA)
		{
			printf("input hanja\n");
			return 1;
		}
		else if (hookstruct->vkCode == VK_LWIN) /* pesky Windows button */
		{
			printf("input left win\n");
			return 1;
		}
		else if (hookstruct->vkCode == VK_RWIN) /* pesky Windows button */
		{
			printf("input right win\n");
		}
		else
		{
			printf("another key prees : %d\n", hookstruct->vkCode);
		}
	}

	CallNextHookEx(hHook, nCode, wParam, lParam);
}

int main(void)
{
	MSG messages;
	HMODULE hInstance = GetModuleHandle(NULL);

	//what is Set WindowsHookEx : https://yokang90.tistory.com/48
	//Keyboard_LowLevel : https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms644985(v=vs.85)
	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInstance, NULL);

	while (GetMessage(&messages, NULL, 0, 0))
	{
		TranslateMessage(&messages);
		DispatchMessage(&messages);
	}

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