#include "ThreadController.h"

ThreadController::ThreadController(const int iThreadNum) : mThreadNum(iThreadNum), mControllerListenSocket(INVALID_SOCKET), mControllerAcceptSocket(INVALID_SOCKET)
{
	std::cout << "[ThreadController Constructor Call]" << std::endl;
}

ThreadController::~ThreadController()
{
	std::cout << "[ThreadController Desturtor Call]" << std::endl;

	JoinThreads();

	delete[] mThreadList;

	CloseSockets();

	WSACleanup();
}

bool ThreadController::WinSocketInit()
{

	WSADATA wsaData;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		std::cout << "WSAStartup failed result : " << iResult << std::endl;
		return false;
	}

	return true;
}

bool ThreadController::ControllerSocketStart(const int port)
{

	int opt;
	int iResult;

	struct sockaddr_in sockinfo;
	struct sockaddr_in clientsockinfo;

	int iClientSize;
	iClientSize = sizeof(clientsockinfo);

	mControllerListenSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (mControllerListenSocket == INVALID_SOCKET) {
		std::cout << "failed to create socket" << std::endl;
		return false;
	}

	memset(&sockinfo, 0, sizeof(sockinfo));

	sockinfo.sin_family = AF_INET;
	sockinfo.sin_addr.s_addr = htonl(INADDR_ANY);
	sockinfo.sin_port = htons(port);

	setsockopt(mControllerListenSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(opt));

	if (bind(mControllerListenSocket, (SOCKADDR*)&sockinfo, sizeof(sockinfo)) == SOCKET_ERROR) {
		std::cout << "mControllerListenSocket bind() error" << std::endl;
		return false;
	}

	if (listen(mControllerListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		std::cout << "mControllerListenSocket listen() error" << std::endl;
		return false;
	}

	std::cout << "Waiting for Client" << std::endl;

	mControllerAcceptSocket= accept(mControllerListenSocket, (SOCKADDR*)&clientsockinfo, &iClientSize);
	if (mControllerAcceptSocket == INVALID_SOCKET) {
		std::cout << "PARMS accept() error" << std::endl;
		return false;
	}

	
	// shutdown the connection since no more data will be sent
	iResult = shutdown(mControllerListenSocket, SD_BOTH);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(mControllerListenSocket);
	}
	closesocket(mControllerListenSocket);

	mControlThread = std::thread(&ThreadController::mControlThreadFunc, this);

	return true;
}

void ThreadController::ControllerSocketStop()
{
	mControlStop = true;
}


bool ThreadController::ThreadListStart()
{

	mThreadListStop = false;

	mThreadList = new std::thread[mThreadNum];

	mThreadList[0] = std::thread(&ThreadController::mThread1,this);
	mThreadList[1] = std::thread(&ThreadController::mThread2,this);
	mThreadList[2] = std::thread(&ThreadController::mThread3,this);

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	return true;
}

void ThreadController::ThreadListStop()
{
	mThreadListStop = true;
}

bool ThreadController::RunCheck(const int timeout)
{
	if (!mControlStop && !mThreadListStop)
	{
		std::cout << "Whole Thread is Running" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
		return true;
	}
	else
	{
		std::cout << "Whole Thread is Not Running" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
		return false;
	}
}

void ThreadController::mControlThreadFunc()
{

	std::cout << "ControlThreadFunc Start" << std::endl;


	int iResult;

	int iSendResult;
	char recvbuf[512];
	int recvbuflen = 512;

	char sendbuf[64] = "recv OK!";

	while (!mControlStop)
	{
		ZeroMemory(recvbuf, sizeof(recvbuf));

		iResult = recv(mControllerAcceptSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {

			recvbuf[iResult] = '\0';

			printf("Bytes received size: %d \n\t\tdata: %s\n", iResult, recvbuf);

			// Echo the buffer back to the sender
			iSendResult = send(mControllerAcceptSocket, sendbuf, (int)strlen(sendbuf), 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(mControllerAcceptSocket);
				WSACleanup();
				break;
			}
			printf("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(mControllerAcceptSocket);
			WSACleanup();
			break;
		}

		if (!strcmp(recvbuf, "ctr_start"))
		{
			ThreadListStart();
		}

		if (!strcmp(recvbuf, "ctr_stop"))
		{
			ControllerSocketStop();
			ThreadListStop();
		}
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

}

void ThreadController::mThread1()
{
	std::cout << "Thread1 Start" << std::endl;

	int i = 1;

	while (!mThreadListStop)
	{
		i++;
	}

	std::cout << "mThread1 value : " << i << std::endl;

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

}

void ThreadController::mThread2()
{
	std::cout << "Thread2 Start" << std::endl;

	int i = 1;


	while (!mThreadListStop)
	{
		i--;
	}

	std::cout << "mThread2 value : " << i << std::endl;


	std::this_thread::sleep_for(std::chrono::milliseconds(10));

}

void ThreadController::mThread3()
{
	std::cout << "Thread3 Start" << std::endl;

	int i = 1;

	while (!mThreadListStop)
	{
		i++;
	}

	std::cout << "mThread3 value : " << i << std::endl;


	std::this_thread::sleep_for(std::chrono::milliseconds(10));


}

void ThreadController::CloseSockets()
{
	int iResult;
	// shutdown the connection since no more data will be sent
	iResult = shutdown(mControllerListenSocket, SD_BOTH);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with mControllerListenSocket error: %d\n", WSAGetLastError());
		closesocket(mControllerListenSocket);
	}
	closesocket(mControllerListenSocket);
	mControllerListenSocket = INVALID_SOCKET;

	// shutdown the connection since no more data will be sent
	iResult = shutdown(mControllerAcceptSocket, SD_BOTH);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with mControllerAcceptSocket error: %d\n", WSAGetLastError());
		closesocket(mControllerAcceptSocket);
	}
	closesocket(mControllerAcceptSocket);
	mControllerAcceptSocket = INVALID_SOCKET;

}

void ThreadController::JoinThreads()
{

	std::cout << "JoinThreads Start" << std::endl;


	for (int i = 0; i < mThreadNum; i++)
	{
		if (true == mThreadList[i].joinable())
		{
			mThreadList[i].join();
			std::cout << "mThreadList[" << i << "] join done" << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(10));

		}
	}

	if (true == mControlThread.joinable())
	{
		mControlThread.join();
		std::cout << "mControlThread join done" << std::endl;

	}
}


