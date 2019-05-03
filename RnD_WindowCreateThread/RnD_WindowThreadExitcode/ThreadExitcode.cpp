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
	//쓰레드가 정상적으로 종료(return이 있을때)될때까지 기다린다.
	WaitForMultipleObjects(1, &hThread, TRUE, INFINITE);
#else

	/*
		쓰레드가 정상적으로 종료가 안되었으므로
		`GetExitCodeThread`로 return 값을 받아도 STILL_ACTIVE(259)가 반환되므로
		내부적으로는 Thread가 종료되지 않음을 의미한다

		쓰레드가 정상적으로 루틴을 마무리하여 return까지 기다리기 위해서 `WaitForMultipleObjects()`함수를 이용한다.

		[실습방법]
		#1 __exitcase 1로 두고 한 뒤 반환값을 보면 ThreadCallbackFunction 에서 지정한 리턴값을 정상적으로 받음을 확인한다
		#2 __exitcase 0로 두고 하게 되면  CloaseHandle을 하여도 STILL_ACTIVE(259)가 반환되므로 쓰레드가 아직 살아있으므로 정상적으로 종료하기 위해서는
		    쓰레드 함수 내에서 ExitThread를 하거나 메인 쓰레드에서 `WaitForMultipleObjects`함수를 이용한다.

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