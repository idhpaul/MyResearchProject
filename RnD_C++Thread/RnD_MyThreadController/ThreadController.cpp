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

	int opt = 1;
	int iResult;

	struct sockaddr_in sockinfo;
	struct sockaddr_in clientsockinfo;

	struct linger optval;
	optval.l_onoff = 1;
	optval.l_linger = 0;

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

	setsockopt(mControllerListenSocket, SOL_SOCKET, SO_LINGER, (char*)&optval, sizeof(optval));
	setsockopt(mControllerListenSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(opt));
	setsockopt(mControllerListenSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));


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
		/*std::cout << "RUN State "
			<< "ThreadListStop :" << std::boolalpha << mThreadList
			<< "ControlStop" << std::boolalpha << m_ThreadControlStop
			<< std::endl;*/
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
			ThreadListJoin();
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

void ThreadController::ThreadListJoin()
{
	for (int i = 0; i < mThreadNum; i++)
	{
		if (true == mThreadList[i].joinable())
		{
			mThreadList[i].join();
			std::cout << "mThreadList[" << i << "] join done" << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(10));

		}
	}
}

void ThreadController::mThread1()
{
	std::cout << "Thread1 Start" << std::endl;

	int i = 1;
	bool bResult;
	int iSendResult;

	SOCKET mThread1_Listen_Socket;
	SOCKET mThread1_Socket;
	bResult = MakeThreadSocket(mThread1_port, mThread1_Listen_Socket, mThread1_Socket);
	if (false == bResult)
	{
		std::cout << "mThread1 make socket failed " << std::endl;

		ThreadListStop();
	}

	while (!m_ThreadListStop)
	{

		std::this_thread::sleep_for(std::chrono::milliseconds(10));


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

	DeleteThreadSocket(mThread1_Listen_Socket, mThread1_Socket, mThread1_port);

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

}

void ThreadController::mThread2()
{
	std::cout << "Thread2 Start" << std::endl;

	int i = 1;
	bool bResult;
	int iSendResult;

	SOCKET mThread2_Listen_Socket;
	SOCKET mThread2_Socket;
	bResult = MakeThreadSocket(mThread2_port, mThread2_Listen_Socket, mThread2_Socket);
	if (false == bResult)
	{
		std::cout << "mThread2 make socket failed " << std::endl;

		ThreadListStop();
	}

	while (!m_ThreadListStop)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

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

	DeleteThreadSocket(mThread2_Listen_Socket, mThread2_Socket, mThread2_port);

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

}

void ThreadController::mThread3()
{
	std::cout << "Thread3 Start" << std::endl;

	int i = 1;
	bool bResult;
	int iSendResult;

	SOCKET mThread3_Listen_Socket;
	SOCKET mThread3_Socket;

	bResult = MakeThreadSocket(mThread3_port, mThread3_Listen_Socket, mThread3_Socket);
	if (false == bResult)
	{
		std::cout << "mThread1 make socket failed " << std::endl;

		ThreadListStop();
	}

	while (!m_ThreadListStop)
	{

		std::this_thread::sleep_for(std::chrono::milliseconds(10));


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

	DeleteThreadSocket(mThread3_Listen_Socket, mThread3_Socket, mThread3_port);

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

}

bool ThreadController::MakeThreadSocket(int port, SOCKET& listen_socket, SOCKET& accept_socket)
{

	int opt = 1;
	int iResult;

	struct sockaddr_in sockinfo;
	struct sockaddr_in clientsockinfo;

	struct linger optval;

	optval.l_onoff = 1;
	optval.l_linger = 0;

	int iClientSize;
	iClientSize = sizeof(clientsockinfo);

	listen_socket = socket(PF_INET, SOCK_STREAM, 0);
	if (listen_socket == INVALID_SOCKET) {
		std::cout << "failed to create socket" << std::endl;
		return SOCKET_ERROR;
	}

	memset(&sockinfo, 0, sizeof(sockinfo));

	sockinfo.sin_family = AF_INET;
	sockinfo.sin_addr.s_addr = htonl(INADDR_ANY);
	sockinfo.sin_port = htons(port);

	

	setsockopt(listen_socket, SOL_SOCKET, SO_LINGER, (char*)&optval, sizeof(optval));
	setsockopt(accept_socket, SOL_SOCKET, SO_LINGER, (char*)&optval, sizeof(optval));

	setsockopt(listen_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(opt));
	//setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
	//setsockopt(accept_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
	


	if (bind(listen_socket, (SOCKADDR*)&sockinfo, sizeof(sockinfo)) == SOCKET_ERROR) {
		std::cout << "listen_socket bind() error" << std::endl;
		return SOCKET_ERROR;
	}

	if (listen(listen_socket, SOMAXCONN) == SOCKET_ERROR) {
		std::cout << "listen_socket listen() error" << std::endl;
		return SOCKET_ERROR;
	}

	std::cout << "Waiting for Client" << std::endl;

	accept_socket = accept(listen_socket, (SOCKADDR*)&clientsockinfo, &iClientSize);
	if (accept_socket == INVALID_SOCKET) {
		std::cout << "PARMS accept() error" << std::endl;
		return SOCKET_ERROR;
	}

	std::cout << "Accept OK Port : "<< port << ")" << std::endl;


	return true;

	
}

void ThreadController::DeleteThreadSocket(SOCKET& listen_socket, SOCKET & accept_socket, const int Thread_socket_port)
{
	std::cout << "Thread Socket Closing(Port : "<< Thread_socket_port << ")" << std::endl;

	int iResult;
	// shutdown the connection since no more data will be sent
	iResult = shutdown(accept_socket, SD_BOTH);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with accept_socket error: %d\n", WSAGetLastError());
		closesocket(accept_socket);
	}

	closesocket(accept_socket);
	//accept_socket = INVALID_SOCKET;

	// shutdown the connection since no more data will be sent
	iResult = shutdown(listen_socket, SD_BOTH);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with listen_socket error: %d\n", WSAGetLastError());
		closesocket(listen_socket);
	}

	closesocket(listen_socket);
	//listen_socket = INVALID_SOCKET;
}

void ThreadController::CloseSockets()
{
	//int iResult;
	//// shutdown the connection since no more data will be sent
	//iResult = shutdown(mControllerListenSocket, SD_BOTH);
	//if (iResult == SOCKET_ERROR) {
	//	printf("shutdown failed with mControllerListenSocket error: %d\n", WSAGetLastError());
	//	closesocket(mControllerListenSocket);
	//}
	closesocket(mControllerListenSocket);
	//mControllerListenSocket = INVALID_SOCKET;

	//// shutdown the connection since no more data will be sent
	//iResult = shutdown(mControllerAcceptSocket, SD_BOTH);
	//if (iResult == SOCKET_ERROR) {
	//	printf("shutdown failed with mControllerAcceptSocket error: %d\n", WSAGetLastError());
	//	closesocket(mControllerAcceptSocket);
	//}
	closesocket(mControllerAcceptSocket);
	//mControllerAcceptSocket = INVALID_SOCKET;

}

void ThreadController::JoinThreads()
{

	std::cout << "JoinThreads Start" << std::endl;


	if (true == mControlThread.joinable())
	{
		mControlThread.join();
		std::cout << "mControlThread join done" << std::endl;

	}
}


