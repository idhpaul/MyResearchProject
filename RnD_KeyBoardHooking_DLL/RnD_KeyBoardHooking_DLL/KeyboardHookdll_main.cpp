// KeyHook.dll
#include <stdio.h>
#include <windows.h>

#define DEF_PROCESS_NAME  "notepad.exe"

HINSTANCE g_hInstance = NULL;
HHOOK g_hHook = NULL;
HWND g_hWnd = NULL;
FILE *f1;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpvReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		g_hInstance = hinstDLL;
		break;

	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
extern "C" {
	LRESULT __declspec(dllexport) CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
	{

		PKBDLLHOOKSTRUCT hookstruct;

		if (nCode == HC_ACTION)
		{
			switch (wParam)
			{
			case WM_KEYDOWN: case WM_SYSKEYDOWN:
			case WM_KEYUP: case WM_SYSKEYUP:

				hookstruct = (PKBDLLHOOKSTRUCT)lParam;

				if (hookstruct->vkCode == 91) /* pesky Windows button */
				{
					printf("press window key\n");
					MessageBox(NULL, "windowkey", "테스트", MB_OK);

					return 1;
				}

				if ((hookstruct->vkCode == 0x09) && (hookstruct->flags == 0x20))
				{
					printf("press Alt + tab\n");
					MessageBox(NULL, " Alt + tab", "테스트", MB_OK);


					return 1;
				}

				if ((hookstruct->vkCode == 0x1B) && (hookstruct->flags == 0x20))
				{
					printf("press Alt + esc\n");
					MessageBox(NULL, "Alt + esc", "테스트", MB_OK);


					return 1;
				}
				if ((hookstruct->vkCode == 0x1B) && (hookstruct->flags == 0x00))
				{
					printf("press Ctrl+Esc\n");
					MessageBox(NULL, "Ctrl+Esc", "테스트", MB_OK);


					return 1;
				}
				if ((hookstruct->vkCode == 0x73) && (hookstruct->flags == 0x20))
				{
					printf("press Alt+F4\n");
					MessageBox(NULL, " Alt+F4wkey다.", "테스트", MB_OK);


					return 1;
				}
				if ((hookstruct->vkCode == 0x5B) && (hookstruct->flags == 0x01))
				{
					printf("press Left Windows Key\n");
					MessageBox(NULL, " Left Windows Key", "테스트", MB_OK);


					return 1;
				}
				if ((hookstruct->vkCode == 0x5C) && (hookstruct->flags == 0x01))
				{
					printf("press Right Windows Key\n");
					MessageBox(NULL, "Right Windows Key", "테스트", MB_OK);


					return 1;
				}

				else
					return CallNextHookEx(NULL, nCode, wParam, lParam);
			}
		}
		return CallNextHookEx(NULL, nCode, wParam, lParam);

	}
}

#ifdef __cplusplus
extern "C" {
#endif
	__declspec(dllexport) void HookStart()
	{
		g_hHook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, g_hInstance, 0); //후킹 시작
		printf("Hook Start\n");
	}
	__declspec(dllexport) void HookStop()
	{
		if (g_hHook)
		{
			UnhookWindowsHookEx(g_hHook); //후킹 종료
			g_hHook = NULL;
		}
	}
#ifdef __cplusplus
}
#endif
