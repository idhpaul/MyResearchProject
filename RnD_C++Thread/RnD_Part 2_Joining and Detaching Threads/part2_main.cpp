#include <thread>
#include <iostream>

class ThreadRAII
{
	std::thread & m_thread;
public:
	ThreadRAII(std::thread  & threadObj) : m_thread(threadObj)
	{

	}
	~ThreadRAII()
	{
		// Check if thread is joinable then detach the thread
		// joinable ��ȯ��: ������ ��ü�� Ȱ�� ���� �����带 �ĺ��ϸ� true ,�׷��� ������ false
		/*
			std::thread t
			std::cout << "before starting, joinable: " << std::boolalpha << t.joinable() << '\n';

			t = std::thread(foo);
			std::cout << "after starting, joinable: " << t.joinable() << '\n'; => true

			t.join();
			std::cout << "after joining, joinable: " << t.joinable() << '\n'; => false
		*/
		if (m_thread.joinable())
		{
			m_thread.detach();
		}
	}
};
void thread_function()
{
	for (int i = 0; i < 10000; i++);
	std::cout << "thread_function Executing" << std::endl;
}

int main()
{
	std::thread threadObj(thread_function);

	// If we comment this Line, then program will crash
	ThreadRAII wrapperObj(threadObj);
	return 0;
}