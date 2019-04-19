#include <Windows.h>
#include <tchar.h>

#include <iostream>

DWORD WINAPI ThreadProc(PVOID pParam) {
	DWORD dwDelay = (DWORD)pParam;

	std::cout << ">>>>>>>>> Thread : " << GetCurrentThreadId() << " enter " << std::endl;
	Sleep(dwDelay);
	std::cout << ">>>>>>>>> Thread : " << GetCurrentThreadId() << " leave " << std::endl;

	//������ ���� �ڵ带 5000���� �����ش�. ���� �Լ����� Ȯ���ߴ� ��ó��, GetExitCodeThread �Լ��� ���� �� ���� ȹ���� �� �ִ�. ��� ��κ��� ��� 0�� ������ ���̴�.
	return dwDelay;
}

void _tmain() {
	std::cout << "Main thread creating sub thread.." << std::endl;

	DWORD dwThreadID = 0;
	//�����带 �����Ѵ�. ������ ��Ʈ�� �Լ��� �Ʒ��� �����Ѵ�.
	HANDLE hThread = CreateThread(NULL, 0, ThreadProc, (PVOID)5000, 0, &dwThreadID);

	if (hThread == NULL) {
		//CreateThread �Լ� ȣ�� ����� ������ ��쿡�� NULL�̸�, GetLastError�� ���� ���� �ڵ带 ����Ѵ�.
		std::cout << " ~~ CreateThread failed, error code : " << GetLastError() << std::endl;
		return;
	}

	//�����尡 ����� ������ ����� ����Ѵ�. �����尡 5�� ����ϹǷ� ���� �Լ������� 10�� ����ϵ��� �Ѵ�. 2�忡���� ������ �����尡 ����Ǵ� �������� ����� ����ϴ� ����� ���� �˾ƺ� ���̴�.
	Sleep(10000);

	DWORD dwExitCode = 0;
	//�������� ���� �ڵ�, �� ��Ʈ�� �Լ��� ���ϰ��� ȹ���Ѵ�.
	GetExitCodeThread(hThread, &dwExitCode);

	std::cout << "Sub thread " << dwThreadID << "terminated with ExitCode " << dwExitCode << std::endl;

	//������ ���� Ŀ�� �����̹Ƿ�, CloseHandle�� ȣ���� �ݾ���� Ŀ�ο� ���� ������ �����ϴ�.
	CloseHandle(hThread);
}