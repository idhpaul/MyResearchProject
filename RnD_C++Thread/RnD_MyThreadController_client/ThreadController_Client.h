#pragma once

#include <WS2tcpip.h>
#include <winsock2.h>

#include <atomic>
#include <thread>
#include <iostream>

#pragma comment (lib, "Ws2_32.lib")

class ThreadController_Client
{
public:
	ThreadController_Client(const int iThreadNum);

	virtual ~ThreadController_Client();

	bool WinSocketInit();

	bool ControllerSocketStart(const char * ip, const int port);
	void ControllerSocketStop();

	bool ThreadListStart();
	void ThreadListStop();

	bool RunCheck(const int timeout);
	
private:

	std::thread mControlThread;
	const int mThreadNum;
	std::thread* mThreadList;

	SOCKET mConnectSocket;

	std::atomic<bool> mControlStop;
	std::atomic<bool> mThreadListStop;

	void mControlThreadFunc();
	void mThread1();
	void mThread2();
	void mThread3();

	void CloseSockets();
	void JoinThreads();

};

