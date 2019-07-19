//ref : https://thispointer.com//c-11-multithreading-part-1-three-different-ways-to-create-threads/

#define version 1

#if (version ==  1)

//함수 포인터를 사용하여 스레드 만들기

#include <iostream>
#include <thread>

void thread_function()
{
	for (int i = 0; i < 10000; i++);
	std::cout << "thread function Executing" << std::endl;
}

int main()
{

	std::thread threadObj(thread_function);

	for (int i = 0; i < 10000; i++);

	std::cout << "Display From MainThread" << std::endl;
	threadObj.join();
	std::cout << "Exit of Main function" << std::endl;

	//use Thread id
	std::thread threadObj1(thread_function);
	std::thread threadObj2(thread_function);

	if (threadObj1.get_id() != threadObj2.get_id())
		std::cout << "Both Threads have different IDs" << std::endl;

	std::cout << "From Main Thread :: ID of Thread 1 = " << threadObj1.get_id() << std::endl;
	std::cout << "From Main Thread :: ID of Thread 2 = " << threadObj2.get_id() << std::endl;

	threadObj1.join();
	threadObj2.join();

	return 0;
}

#elif (version == 2)

// 함수 객체를 사용하여 스레드 만들기

#include <iostream>
#include <thread>
class DisplayThread
{
public:
	void operator()()
	{
		for (int i = 0; i < 10000; i++)
			std::cout << "Display Thread Executing" << std::endl;
	}
};

int main()
{
	std::thread threadObj((DisplayThread()));

	for (int i = 0; i < 10000; i++)
	{
		std::cout << "Display From Main Thread " << std::endl;
	}

	std::cout << "Waiting For Thread to complete" << std::endl;
	threadObj.join();
	std::cout << "Exiting from Main Thread" << std::endl;

	return 0;
}

#elif (version == 3)

//람다 함수를 사용하여 스레드 만들기

#include <iostream>
#include <thread>
int main()
{
	int x = 9;
	std::thread threadObj([] {
		for (int i = 0; i < 10000; i++)
			std::cout << "Display Thread Executing" << std::endl;
	});

	for (int i = 0; i < 10000; i++)
		std::cout << "Display From Main Thread" << std::endl;

	threadObj.join();
	std::cout << "Exiting from Main Thread" << std::endl;
	return 0;
}

#endif

