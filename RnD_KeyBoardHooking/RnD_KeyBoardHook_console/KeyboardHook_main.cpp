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

	// KeyHook.dll �ε�
	hDll = LoadLibrary(L"RnD_KeyboardHook_console_dll.dll");

	// HookStrat, HookStop �Լ��� �ּҸ� ���´�.
	HookStart = (PFN_HOOKSTART)GetProcAddress(hDll, DEF_HOOKSTART);
	HookStop = (PFN_HOOKSTOP)GetProcAddress(hDll, DEF_HOOKSTOP);

	ThreadId = GetCurrentThreadId();

	printf("main_ThreadId : %d\n", ThreadId);

	// ��ŷ ����
	HookStart(ThreadId);

	// q�� �Էµɶ� ���� �δ��� �ƹ��� ����� �����ʰ� ���
	while (GetMessage(&messages, NULL, 0, 0))
	{
		TranslateMessage(&messages);
		DispatchMessage(&messages);
	}
	// ��ŷ ����
	HookStop();

	// KeyHook.dll ��ε�
	FreeLibrary(hDll);

	return 0;


//��ó: https://www.crocus.co.kr/402 [Crocus]
}