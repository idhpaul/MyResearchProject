#include <stdio.h>

#include <Windows.h>

HHOOK hHook;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	//if (nCode < 0 || nCode != HC_ACTION)
	//	return CallNextHookEx(hHook, nCode, wParam, lParam);

	//PKBDLLHOOKSTRUCT hookstruct;

	//hookstruct = (PKBDLLHOOKSTRUCT)lParam;

	//bool blnEat = false;
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
	//

	//if (blnEat)
	//{
	//	printf("blnEat is true\n");
	//	return 1;
	//}
	//else
	//{
	//	printf("----------------------\n");
	//	printf("vkcode : %x\n", hookstruct->vkCode);
	//	//printf("scancode : %x\n", hookstruct->scanCode);
	//	printf("flag : %x\n", hookstruct->flags);
	//	printf("----------------------\n\n");

	//	//return CallNextHookEx(hHook, nCode, wParam, lParam);

	//}

	PKBDLLHOOKSTRUCT hookstruct;

	hookstruct = (PKBDLLHOOKSTRUCT)lParam;

	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
	{
		if (GetAsyncKeyState(VK_LWIN) & 0x8000 || GetAsyncKeyState(VK_RWIN) & 0x8000)
		{
			if (hookstruct->vkCode == 0x51)
			{
				printf("win+q\n");
				return 1;
			}
			else
			{
				printf("win pressed\n");
		
			}

		}

		//return 1;
		//if(GetAsyncKeyState(VK_LSHIFT) & 0x8000 || GetAsyncKeyState(VK_RSHIFT) & 0x8000)
	}

	return CallNextHookEx(hHook, nCode, wParam, lParam);
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