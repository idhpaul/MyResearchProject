/*
	스레드1(mainTask) 역할
		1) 서버와 핸드쉐이킹을 수행
		2) 스레드2에 의해 XML 파일 데이터를 로드 될 때까지 대기
		3) XML에서 로드된 데이터 처리

	스레드2(loadData) 역할
		1) XML에서 데이터 로드
		2) 메시지를 기다리는 다른 스레드에게 알림
*/


#include <iostream>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
using namespace std::placeholders;
class Application
{
	std::mutex m_mutex;
	std::condition_variable m_condVar;
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
		// Notify the condition variable
		m_condVar.notify_one();
	}
	bool isDataLoaded()
	{
		return m_bDataLoaded;
	}
	void mainTask()
	{
		std::cout << "Do Some Handshaking" << std::endl;
		// Acquire the lock
		std::unique_lock<std::mutex> mlock(m_mutex);
		// Start waiting for the Condition Variable to get signaled
		// Wait() will internally release the lock and make the thread to block
		// As soon as condition variable get signaled, resume the thread and
		// again acquire the lock. Then check if condition is met or not
		// If condition is met then continue else again go in wait.
		m_condVar.wait(mlock, std::bind(&Application::isDataLoaded, this));
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