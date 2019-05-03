#include <stdio.h>
#include <Windows.h>

#define __exitcase 1

struct test {
	int a;
	int b;
};

DWORD WINAPI threadfunc(LPVOID lpParam);

int main() {
	struct test testparm = { 1,2 };

	DWORD   dwThreadId;
	HANDLE  hThread;
	DWORD dwExitCode = 0;

	hThread = CreateThread(
		NULL,                   // default security attributes
		0,                      // use default stack size  
		threadfunc,       // thread function name
		&testparm,          // argument to thread function 
		0,                      // use default creation flags 
		&dwThreadId);   // returns the thread identifier 

#if __exitcase 1
	//�����尡 ���������� ����(return�� ������)�ɶ����� ��ٸ���.
	WaitForMultipleObjects(1, &hThread, TRUE, INFINITE);
#else

	/*
		�����尡 ���������� ���ᰡ �ȵǾ����Ƿ�
		`GetExitCodeThread`�� return ���� �޾Ƶ� STILL_ACTIVE(259)�� ��ȯ�ǹǷ�
		���������δ� Thread�� ������� ������ �ǹ��Ѵ�

		�����尡 ���������� ��ƾ�� �������Ͽ� return���� ��ٸ��� ���ؼ� `WaitForMultipleObjects()`�Լ��� �̿��Ѵ�.

		[�ǽ����]
		#1 __exitcase 1�� �ΰ� �� �� ��ȯ���� ���� ThreadCallbackFunction ���� ������ ���ϰ��� ���������� ������ Ȯ���Ѵ�
		#2 __exitcase 0�� �ΰ� �ϰ� �Ǹ�  CloaseHandle�� �Ͽ��� STILL_ACTIVE(259)�� ��ȯ�ǹǷ� �����尡 ���� ��������Ƿ� ���������� �����ϱ� ���ؼ���
		    ������ �Լ� ������ ExitThread�� �ϰų� ���� �����忡�� `WaitForMultipleObjects`�Լ��� �̿��Ѵ�.

			Important  The GetExitCodeThread function returns a valid error code defined by the application only after the thread terminates. 
			Therefore, an application should not use STILL_ACTIVE (259) as an error code. 
			If a thread returns STILL_ACTIVE (259) as an error code, applications that test for this value could interpret 
			it to mean that the thread is still running and continue to test for the completion of the thread 
			after the thread has terminated, which could put the application into an infinite loop. 
			To avoid this problem, callers should call the GetExitCodeThread function only after the thread has been confirmed to have exited. 
			Use the WaitForSingleObject function with a wait duration of zero to determine whether a thread has exited.


	*/

#endif

	GetExitCodeThread(hThread, &dwExitCode);
	printf("%d\n", dwExitCode);

	CloseHandle(hThread);

	GetExitCodeThread(hThread, &dwExitCode);
	printf("%d\n", dwExitCode);

	return 0;
}

DWORD WINAPI threadfunc(LPVOID lpParam) {
	struct test* threadparam;

	threadparam = (struct test*)lpParam;
	printf("\t%d %d\n", threadparam->a, threadparam->b);
	return 3;
}