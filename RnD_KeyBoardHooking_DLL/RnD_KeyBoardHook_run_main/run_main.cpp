#include "stdio.h"
#include "conio.h"
#include "windows.h"

#define DEF_DLL_NAME  "hook_test_sjpark2.dll"
#define DEF_HOOKSTART  "HookStart"
#define DEF_HOOKSTOP  "HookStop"

typedef void(*PFN_HOOKSTART)();
typedef void(*PFN_HOOKSTOP)();

void main()
{
	HMODULE   hDll = NULL;
	PFN_HOOKSTART HookStart = NULL;
	PFN_HOOKSTOP HookStop = NULL;
	char   ch = 0;

	// KeyHook.dll 로드
	hDll = LoadLibrary("hook_test_sjpark2.dll");

	// HookStrat, HookStop 함수의 주소를 얻어온다.
	HookStart = (PFN_HOOKSTART)GetProcAddress(hDll, DEF_HOOKSTART);
	HookStop = (PFN_HOOKSTOP)GetProcAddress(hDll, DEF_HOOKSTOP);

	// 후킹 시작
	HookStart();

	// q가 입력될때 까지 로더는 아무런 기능을 하지않고 대기
	printf("press 'q' to quit!\n");
	while (_getch() != 'q');

	// 후킹 종료
	HookStop();

	// KeyHook.dll 언로드
	FreeLibrary(hDll);
}


