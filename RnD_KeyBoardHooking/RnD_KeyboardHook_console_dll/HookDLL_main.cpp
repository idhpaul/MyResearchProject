// KeyHook.dll
#include "stdio.h"
#include "windows.h"

using namespace std;

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

		//MessageBox(NULL,L"Enter hook", L"WARNING", MB_OK);
		printf("Enter Hook\n");

		if (lParam & 0x80000000) // released
		{
			printf("press : %d\n", wParam);
			//if (wParam == VK_LWIN) // f7 pressed
			//{
			//	printf("press LWIN\n");
			//	MessageBox(NULL,L"F7 pressed", L"WARNING", MB_OK);
			//}
			return 1;
		}
		else
		{
			printf("up :%d\n",wParam);
		}
			return CallNextHookEx(g_hHook, nCode, wParam, lParam);

		//return CallNextHookEx(g_hHook, nCode, wParam, lParam);


		//char szPath[MAX_PATH] = { 0, };
		//char *p = NULL;
		//char ch;
		//HWND pWnd, pEdit;

		//printf("call keyboardprc\n");

		//if (nCode >= 0)
		//{
		//	// bit 31 : 0 => key press, 1 => key release
		//	if (!(lParam & 0x80000000))
		//	{
		//		if (wParam == VK_RETURN) { //엔터가 입력된 경우 메모장의 윈도우를 얻어와 "hooking"을 출력한다.

		//			printf("press enter\n");
		//			return 0; //0을 리턴하면 메시지는 정상 전달됨
		//		}
		//	}


		//}

		// 일반적인 경우에는 CallNextHookEx() 를 호출하여
		// 응용프로그램 (혹은 다음 훅) 으로 메시지를 전달함

	}
}

#ifdef __cplusplus
extern "C" {
#endif
	__declspec(dllexport) void HookStart(DWORD threadId)
	{

		printf("dll_ThreadId : %d\n", threadId);

		g_hHook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, g_hInstance, threadId); //후킹 시작
		printf("Hook Start....\n");
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
//출처: https://www.crocus.co.kr/402 [Crocus]