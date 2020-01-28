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
	ThreadController_Client(const int iControllerPort , const int iThreadNum, const char* ip, const int iThread1_port, const int iThread2_port, const int iThread3_port);

	virtual ~ThreadController_Client();

	bool WinSocketInit();

	bool ControllerSocketStart();
	void ControllerSocketStop();

	bool ThreadListStart();
	void ThreadListStop();
	void ThreadJoin();

	bool RunCheck(const int timeout);
	
private:

	const int mThreadNum;
	std::thread mControlThread;
	std::thread* mThreadList;
	const int mController_port;
	const int mThread1_port;
	const int mThread2_port;
	const int mThread3_port;

	const char* ServerIPAddr;

	SOCKET mConnectSocket;

	std::atomic<bool> mControlStop;
	std::atomic<bool> mThreadListStop;

	void mControlThreadFunc();
	void mThread1();
	void mThread2();
	void mThread3();

	bool MakeThreadSocket(int port, SOCKET& accept_socket);
	void CloseThreadSocket(SOCKET& accept_socket);

	void CloseControllerSocket();
	void JoinThreads();

};

