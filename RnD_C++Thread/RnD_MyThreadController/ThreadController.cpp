#include "ThreadController.h"

ThreadController::ThreadController(const int iControllerPort, const int iThreadNum, const int iThread1_Port, const int iThread2_Port, const int iThread3_Port)
	: mController_port(iControllerPort),
	mThreadNum(iThreadNum), 
	mThread1_port(iThread1_Port),
	mThread2_port(iThread2_Port),
	mThread3_port(iThread3_Port),
	mControllerListenSocket(INVALID_SOCKET), 
	mControllerAcceptSocket(INVALID_SOCKET)
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

bool ThreadController::ThreadControllerStart()
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
	sockinfo.sin_port = htons(mController_port);

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

	closesocket(mControllerListenSocket);
	mControllerListenSocket = INVALID_SOCKET;


mControlThread = std::thread(&ThreadController::mControlThreadFunc, this);

return true;
}


bool ThreadController::RunCheck(const int timeout)
{
	if (!m_ThreadListStop || !m_ThreadControlStop)
	{
		std::cout << "RUN State "
			<< "ThreadListStop :" << std::boolalpha << mThreadList
			<< "ControlStop" << std::boolalpha << m_ThreadControlStop
			<< std::endl;
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

	while (!m_ThreadControlStop)
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
				ThreadControlStop();
				ThreadListStop();
			}
			printf("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult == 0)
		{
			printf("Connection closing...\n");
			ThreadControlStop();
			ThreadListStop();
		}
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			ThreadControlStop();
			ThreadListStop();
		}

		if (!strcmp(recvbuf, "all_stop"))
		{
			ThreadControlStop();
			ThreadListStop();
		}

		if (!strcmp(recvbuf, "thread_start"))
		{
			ThreadListStart();
		}

		if (!strcmp(recvbuf, "thread_stop"))
		{
			ThreadListStop();
		}
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

}

void ThreadController::ThreadControlStop()
{
	m_ThreadControlStop = true;
}


bool ThreadController::ThreadListStart()
{

	m_ThreadListStop = false;

	mThreadList = new std::thread[mThreadNum];

	mThreadList[0] = std::thread(&ThreadController::mThread1, this);
	mThreadList[1] = std::thread(&ThreadController::mThread2, this);
	mThreadList[2] = std::thread(&ThreadController::mThread3, this);

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	return true;
}

void ThreadController::ThreadListStop()
{
	m_ThreadListStop = true;
}

void ThreadController::mThread1()
{
	std::cout << "Thread1 Start" << std::endl;

	int i = 1;
	bool bResult;
	int iSendResult;

	SOCKET mThread1_Socket;
	bResult = MakeThreadSocket(mThread1_port, mThread1_Socket);
	if (false == bResult)
	{
		std::cout << "mThread1 make socket failed " << std::endl;

		ThreadListStop();
	}

	while (!m_ThreadListStop)
	{
		i++;

		iSendResult = send(mThread1_Socket, (char*)&i, sizeof(int), 0);
		if (iSendResult > 0)
		{

		}
		else if (iSendResult == 0)
		{
			printf("Connection closing...\n");

			ThreadListStop();
		}
		else
		{
			// iSendResult == SOKETERROR(-1)
			printf("Send failed with error at mThread1: %d\n", WSAGetLastError());

			ThreadListStop();
		}
	}

	std::cout << "mThread1 value : " << i << std::endl;

	DeleteThreadSocket(mThread1_Socket);

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

}

void ThreadController::mThread2()
{
	std::cout << "Thread1 Start" << std::endl;

	int i = 1;
	bool bResult;
	int iSendResult;

	SOCKET mThread2_Socket;
	bResult = MakeThreadSocket(mThread2_port, mThread2_Socket);
	if (false == bResult)
	{
		std::cout << "mThread2 make socket failed " << std::endl;

		ThreadListStop();
	}

	while (!m_ThreadListStop)
	{
		i--;

		iSendResult = send(mThread2_Socket, (char*)&i, sizeof(int), 0);
		if (iSendResult > 0)
		{

		}
		else if (iSendResult == 0)
		{
			printf("Connection closing...\n");

			ThreadListStop();
		}
		else
		{
			// iSendResult == SOKETERROR(-1)
			printf("Send failed with error at mThread2: %d\n", WSAGetLastError());

			ThreadListStop();
		}
	}

	std::cout << "mThread2 value : " << i << std::endl;

	DeleteThreadSocket(mThread2_Socket);

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

}

void ThreadController::mThread3()
{
	std::cout << "Thread1 Start" << std::endl;

	int i = 1;
	bool bResult;
	int iSendResult;

	SOCKET mThread3_Socket;
	bResult = MakeThreadSocket(mThread3_port, mThread3_Socket);
	if (false == bResult)
	{
		std::cout << "mThread1 make socket failed " << std::endl;

		ThreadListStop();
	}

	while (!m_ThreadListStop)
	{
		i++;

		iSendResult = send(mThread3_Socket, (char*)&i, sizeof(int), 0);
		if (iSendResult > 0)
		{

		}
		else if (iSendResult == 0)
		{
			printf("Connection closing...\n");

			ThreadListStop();
		}
		else
		{
			// iSendResult == SOKETERROR(-1)
			printf("Send failed with error at mThread3: %d\n", WSAGetLastError());

			ThreadListStop();
		}
	}

	std::cout << "mThread3 value : " << i << std::endl;

	DeleteThreadSocket(mThread3_Socket);

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

}

bool ThreadController::MakeThreadSocket(int port, SOCKET& accept_socket)
{

	int opt;
	int iResult;

	struct sockaddr_in sockinfo;
	struct sockaddr_in clientsockinfo;

	SOCKET threadListenSocket;

	int iClientSize;
	iClientSize = sizeof(clientsockinfo);

	threadListenSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (threadListenSocket == INVALID_SOCKET) {
		std::cout << "failed to create socket" << std::endl;
		return SOCKET_ERROR;
	}

	memset(&sockinfo, 0, sizeof(sockinfo));

	sockinfo.sin_family = AF_INET;
	sockinfo.sin_addr.s_addr = htonl(INADDR_ANY);
	sockinfo.sin_port = htons(port);

	setsockopt(threadListenSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(opt));

	if (bind(threadListenSocket, (SOCKADDR*)&sockinfo, sizeof(sockinfo)) == SOCKET_ERROR) {
		std::cout << "threadSocket1 bind() error" << std::endl;
		return SOCKET_ERROR;
	}

	if (listen(threadListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		std::cout << "threadSocket1 listen() error" << std::endl;
		return SOCKET_ERROR;
	}

	std::cout << "Waiting for Client" << std::endl;

	accept_socket = accept(threadListenSocket, (SOCKADDR*)&clientsockinfo, &iClientSize);
	if (accept_socket == INVALID_SOCKET) {
		std::cout << "PARMS accept() error" << std::endl;
		return SOCKET_ERROR;
	}

	closesocket(threadListenSocket);
	threadListenSocket = INVALID_SOCKET;

	return true;

	
}

void ThreadController::DeleteThreadSocket(SOCKET & accept_socket)
{
	int iResult;
	// shutdown the connection since no more data will be sent
	iResult = shutdown(accept_socket, SD_BOTH);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with accept_socket error: %d\n", WSAGetLastError());
		closesocket(accept_socket);
	}
	closesocket(accept_socket);
	accept_socket = INVALID_SOCKET;

	std::cout << "Thread Socket Close" << std::endl;
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


