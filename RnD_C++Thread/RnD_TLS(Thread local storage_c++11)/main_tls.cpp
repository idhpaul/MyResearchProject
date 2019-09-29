#include <iostream>
#include <thread>
#include <mutex>

thread_local unsigned int i = 0;
std::mutex mtx;

void ThreadFunc(int nID)
{
	++i;
	std::unique_lock<std::mutex> lock(mtx);
	std::cout << nID << "-thread : " << i << std::endl;
}

int main()
{
	std::thread th1(ThreadFunc, 0), th2(ThreadFunc, 1);

	std::unique_lock<std::mutex> lock(mtx);
	std::cout << "Main thread : " << i << std::endl;
	lock.unlock();

	th1.join();
	th2.join();

	return 0;
}