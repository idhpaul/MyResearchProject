#include "ThreadController_Client.h"

ThreadController_Client::ThreadController_Client(const int iControllerPort, const int iThreadNum, const char* ip, const int iThread1_port, const int iThread2_port, const int iThread3_port)
	: mController_port(iControllerPort),
	mThreadNum(iThreadNum), 
	ServerIPAddr(ip), 
	mThread1_port(iThread1_port),
	mThread2_port(iThread2_port),
	mThread3_port(iThread3_port),
	mConnectSocket(INVALID_SOCKET)
{
	std::cout << "[ThreadController Constructor Call]" << std::endl;
}

ThreadController_Client::~ThreadController_Client()
{
	std::cout << "[ThreadController Desturtor Call]" << std::endl;

	JoinThreads();

	delete[] mThreadList;

	CloseControllerSocket();

	WSACleanup();
}

bool ThreadController_Client::WinSocketInit()
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

bool ThreadController_Client::ControllerSocketStart()
{

	int opt = 1;
	int iResult;
	struct sockaddr_in clientsockinfo;

	int iClientSize;
	iClientSize = sizeof(clientsockinfo);

	struct linger optval;
	optval.l_onoff = 1;
	optval.l_linger = 0;

	mConnectSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (mConnectSocket == INVALID_SOCKET) {
		std::cout << "Failed to create socket" << std::endl;
		return false;
	}

	memset(&clientsockinfo, 0, sizeof(clientsockinfo));

	clientsockinfo.sin_family = AF_INET;
	inet_pton(AF_INET, ServerIPAddr, &(clientsockinfo.sin_addr));
	clientsockinfo.sin_port = htons(mController_port);

	setsockopt(mConnectSocket, SOL_SOCKET, SO_LINGER, (char*)&optval, sizeof(optval));


	setsockopt(mConnectSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(opt));
	setsockopt(mConnectSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

	while (true)
	{
		
		iResult = connect(mConnectSocket, (SOCKADDR*)&clientsockinfo, sizeof(clientsockinfo));
		if (iResult == SOCKET_ERROR)
		{
			std::cout << "Failed to connect" << std::endl;
		}
		else
		{
			std::cout << "Sucess to connect" << std::endl;
			break;
		}
	}
	

	mControlThread = std::thread(&ThreadController_Client::mControlThreadFunc, this);

	return true;
}

void ThreadController_Client::ControllerSocketStop()
{
	mControlStop = true;
}


bool ThreadController_Client::ThreadListStart()
{
	mThreadList = new std::thread[mThreadNum];

	mThreadList[0] = std::thread(&ThreadController_Client::mThread1,this);
	mThreadList[1] = std::thread(&ThreadController_Client::mThread2,this);
	mThreadList[2] = std::thread(&ThreadController_Client::mThread3,this);

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	return true;
}

void ThreadController_Client::ThreadListStop()
{
	mThreadListStop = true;
}

void ThreadController_Client::ThreadJoin()
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

bool ThreadController_Client::RunCheck(const int timeout)
{
	if (!mControlStop || !mThreadListStop)
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

void ThreadController_Client::mControlThreadFunc()
{
	std::cout << "ControlThreadFunc Start" << std::endl;

	int iResult;

	char recvbuf[512];
	int recvbuflen = 512;

	char sendbuf[64] = "Communcation START\n";

	// Send an initial buffer
	iResult = send(mConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult > 0)
	{
		std::cout << "Bytes First Send : " << iResult << std::endl;

	}
	else if (iResult == 0)
	{
		std::cout << "Connection is Closed code: " << WSAGetLastError() << std::endl;
		ControllerSocketStop();
	}
	else
	{
		std::cout << "Recv Failed code: " << WSAGetLastError() << std::endl;
		ControllerSocketStop();
	}

	while (!mControlStop)
	{
	
		char inputData[512];
		char *p;


		iResult = recv(mConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);

			ZeroMemory(inputData, sizeof(inputData));

			printf("Input String : ");
			fgets(inputData, sizeof(inputData) - 1, stdin);
			if ((p = strchr(inputData, '\n')) != NULL)
				*p = '\0';

			printf("[%s]\n", inputData);

			// Send an initial buffer
			iResult = send(mConnectSocket, inputData, (int)strlen(inputData), 0);
			if (iResult > 0)
			{
				//std::cout << "Bytes First Send : " << iResult << std::endl;
				if (!strcmp(inputData, "all_stop"))
				{
					ThreadListStop();

					ControllerSocketStop();
				}
				else if (!strcmp(inputData, "thread_start"))
				{
					ThreadListStart();
				}
				else if (!strcmp(inputData, "thread_stop"))
				{
					ThreadListStop();
					ThreadJoin();
				}
			}
			else if (iResult == 0)
			{
				std::cout << "Connection is Closed code: " << WSAGetLastError() << std::endl;
				ThreadListStop();

				ControllerSocketStop();
			}
			else
			{
				std::cout << "Recv Failed code: " << WSAGetLastError() << std::endl;
				ThreadListStop();

				ControllerSocketStop();
			}

		}
		else if (iResult == 0)
		{
			printf("Connection closed\n");
			ThreadListStop();

			ControllerSocketStop();
		}
		else
		{
			printf("recv failed with error: %d\n", WSAGetLastError());
			ThreadListStop();

			ControllerSocketStop();
		}

	}

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

}

void ThreadController_Client::mThread1()
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

	while (!mThreadListStop)
	{

		iSendResult = recv(mThread1_Socket, (char*)&i, sizeof(int), 0);
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

	std::cout << "Recved mThread1 value : " << i << std::endl;

	CloseThreadSocket(mThread1_Socket);

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

}

void ThreadController_Client::mThread2()
{
	int i = 1;
	bool bResult;
	int iSendResult;

	SOCKET mThread2_Socket;
	bResult = MakeThreadSocket(mThread2_port, mThread2_Socket);
	if (false == bResult)
	{
		std::cout << "mThread1 make socket failed " << std::endl;
		ThreadListStop();
	}

	while (!mThreadListStop)
	{

		iSendResult = recv(mThread2_Socket, (char*)&i, sizeof(int), 0);
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

	std::cout << "Recved mThread2 value : " << i << std::endl;

	CloseThreadSocket(mThread2_Socket);

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

}

void ThreadController_Client::mThread3()
{
	int i = 1;
	bool bResult;
	int iSendResult;

	SOCKET mThread3_Socket;
	bResult = MakeThreadSocket(mThread3_port, mThread3_Socket);
	if (false == bResult)
	{
		std::cout << "mThread3 make socket failed " << std::endl;
		ControllerSocketStop();
		ThreadListStop();
	}

	while (!mThreadListStop)
	{

		iSendResult = recv(mThread3_Socket, (char*)&i, sizeof(int), 0);
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

	std::cout << "Recved mThread3 value : " << i << std::endl;

	CloseThreadSocket(mThread3_Socket);

	std::this_thread::sleep_for(std::chrono::milliseconds(10));


}

bool ThreadController_Client::MakeThreadSocket(int port, SOCKET & accept_socket)
{
	int opt = 1;
	int iResult;
	struct sockaddr_in clientsockinfo;

	int iClientSize;
	iClientSize = sizeof(clientsockinfo);

	struct linger optval;
	optval.l_onoff = 1;
	optval.l_linger = 0;


	accept_socket = socket(PF_INET, SOCK_STREAM, 0);
	if (accept_socket == INVALID_SOCKET) {
		std::cout << "Failed to create socket" << std::endl;
		return false;
	}

	memset(&clientsockinfo, 0, sizeof(clientsockinfo));

	clientsockinfo.sin_family = AF_INET;
	inet_pton(AF_INET, ServerIPAddr, &(clientsockinfo.sin_addr));
	clientsockinfo.sin_port = htons(port);

	setsockopt(accept_socket, SOL_SOCKET, SO_LINGER, (char*)&optval, sizeof(optval));
	setsockopt(accept_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(opt));
	//setsockopt(accept_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));



	std::cout << "Thread Socket connect try.. Port : " << port << std::endl;

	while (true)
	{

		iResult = connect(accept_socket, (SOCKADDR*)&clientsockinfo, sizeof(clientsockinfo));
		if (iResult == SOCKET_ERROR)
		{
			std::cout << "Failed to connect" << std::endl;
		}
		else
		{
			std::cout << "Sucess to connect" << std::endl;
			break;
		}
	}

	return true;
}

void ThreadController_Client::CloseThreadSocket(SOCKET & accept_socket)
{
	int iResult;
	// shutdown the connection since no more data will be sent
	iResult = shutdown(accept_socket, SD_BOTH);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with accept_socket error: %d\n", WSAGetLastError());
		closesocket(accept_socket);
	}
	closesocket(accept_socket);
	//accept_socket = INVALID_SOCKET;

	std::cout << "Thread Socket Close" << std::endl;
}

void ThreadController_Client::CloseControllerSocket()
{

	//int iResult;
	//// shutdown the connection since no more data will be sent
	//iResult = shutdown(mConnectSocket, SD_BOTH);
	//if (iResult == SOCKET_ERROR) {
	//	printf("shutdown failed with error: %d\n", WSAGetLastError());
	//	closesocket(mConnectSocket);
	//}

	closesocket(mConnectSocket);
	//mConnectSocket = INVALID_SOCKET;


}

void ThreadController_Client::JoinThreads()
{

	std::cout << "JoinThreads Start" << std::endl;


	if (true == mControlThread.joinable())
	{
		mControlThread.join();
		std::cout << "mControlThread join done" << std::endl;

	}
}


