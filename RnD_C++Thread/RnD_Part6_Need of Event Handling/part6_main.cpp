/*
	해당 소스코드의 문제점
	
	스레드1은 값을 확인하기 위해서 CPU 사이클을 소비하고 bool 플래그를 업데이트하기 위해 점유권을 획득해야하기 때문에 속도가 느림

*/

#include<iostream>
#include<thread>
#include<mutex>
class Application
{
	std::mutex m_mutex;
	bool m_bDataLoaded;
public:
	Application()
	{
		m_bDataLoaded = false;
	}
	void loadData()
	{
		// Make This Thread sleep for 1 Second
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout << "Loading Data from XML" << std::endl;
		// Lock The Data structure
		std::lock_guard<std::mutex> guard(m_mutex);
		// Set the flag to true, means data is loaded
		m_bDataLoaded = true;
	}
	void mainTask()
	{
		std::cout << "Do Some Handshaking" << std::endl;
		// Acquire the Lock
		//m_mutex.lock();
		// Check if flag is set to true or not
		while (m_bDataLoaded != true)
		{
			// Release the lock
			//m_mutex.unlock();
			std::lock_guard<std::mutex> guard(m_mutex);
			//sleep for 100 milli seconds
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			// Acquire the lock
			//m_mutex.lock();
		}
		// Release the lock
		//m_mutex.unlock();
		//Doc processing on loaded Data
		std::cout << "Do Processing On loaded Data" << std::endl;
	}
};
int main()
{
	Application app;
	std::thread thread_1(&Application::mainTask, &app);
	std::thread thread_2(&Application::loadData, &app);
	thread_2.join();
	thread_1.join();
	return 0;
}