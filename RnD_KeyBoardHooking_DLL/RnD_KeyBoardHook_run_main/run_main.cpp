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

	// KeyHook.dll �ε�
	hDll = LoadLibrary("hook_test_sjpark2.dll");

	// HookStrat, HookStop �Լ��� �ּҸ� ���´�.
	HookStart = (PFN_HOOKSTART)GetProcAddress(hDll, DEF_HOOKSTART);
	HookStop = (PFN_HOOKSTOP)GetProcAddress(hDll, DEF_HOOKSTOP);

	// ��ŷ ����
	HookStart();

	// q�� �Էµɶ� ���� �δ��� �ƹ��� ����� �����ʰ� ���
	printf("press 'q' to quit!\n");
	while (_getch() != 'q');

	// ��ŷ ����
	HookStop();

	// KeyHook.dll ��ε�
	FreeLibrary(hDll);
}


