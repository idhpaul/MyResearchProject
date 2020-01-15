#include "ThreadController.h"

ThreadController::ThreadController(const int iThreadNum) : mThreadNum(iThreadNum)
{
	std::cout << "[ThreadController Constructor Call]" << std::endl;
}

ThreadController::~ThreadController()
{
	std::cout << "[ThreadController Desturtor Call]" << std::endl;

	JoinThreads();
	delete[] mThreadList;
}


bool ThreadController::Start()
{
	mThreadList = new std::thread[mThreadNum];


	mThreadList[0] = std::thread(&ThreadController::mThread1,this);
	mThreadList[1] = std::thread(&ThreadController::mThread2,this);
	mThreadList[2] = std::thread(&ThreadController::mThread3,this);

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	return true;
}

void ThreadController::Stop()
{
	mStop = true;
}

void ThreadController::mThread1()
{
	std::cout << "Thread1 Start" << std::endl;

	int i = 1;

	while (!mStop)
	{
		i++;
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

}

void ThreadController::mThread2()
{
	std::cout << "Thread2 Start" << std::endl;

	int i = 1;


	while (!mStop)
	{
		i--;
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

}

void ThreadController::mThread3()
{
	std::cout << "Thread3 Start" << std::endl;

	int i = 1;

	while (!mStop)
	{
		i++;
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(10));


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
}


