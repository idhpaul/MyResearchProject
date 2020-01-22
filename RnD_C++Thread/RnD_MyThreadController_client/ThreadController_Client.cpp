#include "ThreadController_Client.h"

ThreadController_Client::ThreadController_Client(const int iThreadNum) : mThreadNum(iThreadNum), mConnectSocket(INVALID_SOCKET)
{
	std::cout << "[ThreadController Constructor Call]" << std::endl;
}

ThreadController_Client::~ThreadController_Client()
{
	std::cout << "[ThreadController Desturtor Call]" << std::endl;

	

	JoinThreads();

	//delete[] mThreadList;

	CloseSockets();

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

bool ThreadController_Client::ControllerSocketStart(const char * ip, const int port)
{

	int opt;
	int iResult;
	struct sockaddr_in clientsockinfo;

	int iClientSize;
	iClientSize = sizeof(clientsockinfo);

	mConnectSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (mConnectSocket == INVALID_SOCKET) {
		std::cout << "Failed to create socket" << std::endl;
		return false;
	}

	memset(&clientsockinfo, 0, sizeof(clientsockinfo));

	clientsockinfo.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &(clientsockinfo.sin_addr));
	clientsockinfo.sin_port = htons(port);

	setsockopt(mConnectSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(opt));

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

bool ThreadController_Client::RunCheck(const int timeout)
{
	if (!mControlStop /*&& !mThreadStop*/)
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
			iResult = send(mConnectSocket, inputData, (int)strlen(sendbuf), 0);
			if (iResult > 0)
			{
				//std::cout << "Bytes First Send : " << iResult << std::endl;

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

		}
		else if (iResult == 0)
		{
			printf("Connection closed\n");
			ControllerSocketStop();
		}
		else
		{
			printf("recv failed with error: %d\n", WSAGetLastError());
			ControllerSocketStop();
		}

	}

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

}

void ThreadController_Client::mThread1()
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

void ThreadController_Client::mThread2()
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

void ThreadController_Client::mThread3()
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

void ThreadController_Client::CloseSockets()
{

	int iResult;
	// shutdown the connection since no more data will be sent
	iResult = shutdown(mConnectSocket, SD_BOTH);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(mConnectSocket);
	}

	closesocket(mConnectSocket);

}

void ThreadController_Client::JoinThreads()
{

	std::cout << "JoinThreads Start" << std::endl;


	/*for (int i = 0; i < mThreadNum; i++)
	{
		if (true == mThreadList[i].joinable())
		{
			mThreadList[i].join();
			std::cout << "mThreadList[" << i << "] join done" << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(10));

		}
	}*/

	if (true == mControlThread.joinable())
	{
		mControlThread.join();
		std::cout << "mControlThread join done" << std::endl;

	}
}


