#include <Windows.h>
#include <tchar.h>

#include <iostream>

DWORD WINAPI ThreadProc(PVOID pParam) {
	DWORD dwDelay = (DWORD)pParam;

	std::cout << ">>>>>>>>> Thread : " << GetCurrentThreadId() << " enter " << std::endl;
	Sleep(dwDelay);
	std::cout << ">>>>>>>>> Thread : " << GetCurrentThreadId() << " leave " << std::endl;

	//스레드 종료 코드를 5000으로 돌려준다. 메인 함수에서 확인했던 것처럼, GetExitCodeThread 함수를 통해 이 값을 획득할 수 있다. 사실 대부분의 경우 0을 리턴할 것이다.
	return dwDelay;
}

void _tmain() {
	std::cout << "Main thread creating sub thread.." << std::endl;

	DWORD dwThreadID = 0;
	//스레드를 생성한다. 스레드 엔트리 함수는 아래에 정의한다.
	HANDLE hThread = CreateThread(NULL, 0, ThreadProc, (PVOID)5000, 0, &dwThreadID);

	if (hThread == NULL) {
		//CreateThread 함수 호출 결과가 실패인 경우에는 NULL이며, GetLastError를 통해 에러 코드를 출력한다.
		std::cout << " ~~ CreateThread failed, error code : " << GetLastError() << std::endl;
		return;
	}

	//스레드가 종료될 때까지 충분히 대기한다. 스레드가 5초 대기하므로 메인 함수에서는 10초 대기하도록 한다. 2장에서는 실제로 스레드가 종료되는 시점까지 제대로 대기하는 방법에 대해 알아볼 것이다.
	Sleep(10000);

	DWORD dwExitCode = 0;
	//스레드의 종료 코드, 즉 엔트리 함수의 리턴값을 획득한다.
	GetExitCodeThread(hThread, &dwExitCode);

	std::cout << "Sub thread " << dwThreadID << "terminated with ExitCode " << dwExitCode << std::endl;

	//스레드 역시 커널 개겣이므로, CloseHandle을 호출해 닫아줘야 커널에 의해 삭제가 가능하다.
	CloseHandle(hThread);
}