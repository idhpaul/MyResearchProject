#pragma once

#include <atomic>
#include <thread>
#include <iostream>


class ThreadController
{
public:
	ThreadController(const int iThreadNum);

	~ThreadController();

	bool Start();
	void Stop();
	
private:
	const int mThreadNum;
	std::thread* mThreadList;

	std::atomic<bool> mStop;

	void mThread1();
	void mThread2();
	void mThread3();

	void JoinThreads();

};

