#include <windows.h>

#include <conio.h>

#include <stdio.h>

#define DEF_HOOKSTART  "HookStart"
#define DEF_HOOKSTOP  "HookStop"

typedef void(*PFN_HOOKSTART)(DWORD threadId);
typedef void(*PFN_HOOKSTOP)();

int main(int argc, char *argv[])
{
	MSG messages;
	HMODULE   hDll = NULL;
	PFN_HOOKSTART HookStart = NULL;
	PFN_HOOKSTOP HookStop = NULL;

	DWORD ThreadId;

	char   ch = 0;

	// KeyHook.dll 로드
	hDll = LoadLibrary(L"RnD_KeyboardHook_console_dll.dll");

	// HookStrat, HookStop 함수의 주소를 얻어온다.
	HookStart = (PFN_HOOKSTART)GetProcAddress(hDll, DEF_HOOKSTART);
	HookStop = (PFN_HOOKSTOP)GetProcAddress(hDll, DEF_HOOKSTOP);

	ThreadId = GetCurrentThreadId();

	printf("main_ThreadId : %d\n", ThreadId);

	// 후킹 시작
	HookStart(ThreadId);

	// q가 입력될때 까지 로더는 아무런 기능을 하지않고 대기
	while (GetMessage(&messages, NULL, 0, 0))
	{
		TranslateMessage(&messages);
		DispatchMessage(&messages);
	}
	// 후킹 종료
	HookStop();

	// KeyHook.dll 언로드
	FreeLibrary(hDll);

	return 0;


//출처: https://www.crocus.co.kr/402 [Crocus]
}