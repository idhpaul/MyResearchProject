#pragma once

#include <winsock2.h>

#include <atomic>
#include <thread>
#include <iostream>

#pragma comment (lib, "Ws2_32.lib")

class ThreadController
{
public:
	ThreadController(const int iControllerPort ,const int iThreadNum, const int iThread1_Port,  const int iThread2_Port, const int iThread3_Port);

	virtual ~ThreadController();

	bool WinSocketInit();

	bool ThreadControllerStart();

	bool RunCheck(const int timeout);
	
private:

	std::thread mControlThread;

	const int mThreadNum;
	std::thread* mThreadList;
	const int mController_port;
	const int mThread1_port;
	const int mThread2_port;
	const int mThread3_port;

	SOCKET mControllerListenSocket;
	SOCKET mControllerAcceptSocket;

	std::atomic<bool> m_ThreadControlStop;
	std::atomic<bool> m_ThreadListStop;

	void mControlThreadFunc();

	void ThreadControlStop();

	bool ThreadListStart();
	void ThreadListStop();
	void ThreadListJoin();

	void mThread1();
	void mThread2();
	void mThread3();

	bool MakeThreadSocket(int port, SOCKET& listen_socket, SOCKET& accept_socket);
	void DeleteThreadSocket(SOCKET& listen_socket, SOCKET& accept_socket, const int Thread_socket_port);

	void CloseSockets();
	void JoinThreads();

};

