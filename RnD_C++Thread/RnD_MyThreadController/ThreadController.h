#pragma once

#include <winsock2.h>

#include <atomic>
#include <thread>
#include <iostream>

#pragma comment (lib, "Ws2_32.lib")

class ThreadController
{
public:
	ThreadController(const int iThreadNum);

	virtual ~ThreadController();

	bool WinSocketInit();

	bool ControllerSocketStart(const int port);
	void ControllerSocketStop();

	bool ThreadListStart();
	void ThreadListStop();

	bool RunCheck(const int timeout);
	
private:

	std::thread mControlThread;
	const int mThreadNum;
	std::thread* mThreadList;

	SOCKET mControllerListenSocket;
	SOCKET mControllerAcceptSocket;

	std::atomic<bool> mControlStop;
	std::atomic<bool> mThreadListStop;

	void mControlThreadFunc();
	void mThread1();
	void mThread2();
	void mThread3();

	void CloseSockets();
	void JoinThreads();

};

