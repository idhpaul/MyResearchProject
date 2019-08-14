//
//#include "Timer.h"
//
//using namespace std;
//
//
//
//int main(void)
//{
//	Timer t = Timer();
//
//
//	t.setInterval([&]() {
//		cout << "Hey.. After each 1s..." << endl;
//	}, 1000);
//
//
//	t.setTimeout([&]() {
//		cout << "Hey.. After 5.2s. But I will stop the timer!" << endl;
//		t.stop();
//	}, 5200);
//
//	return 0;
//}
#include <iostream>
#include <future>
#include <chrono>
#include <thread>
#include <functional>

template<class Fun, class Duration, class...Args>
auto timer(Fun fun, Duration d, Duration timeout, Args... args)
{
	return std::async(std::launch::async, [=]
	{
		auto start = std::chrono::system_clock::now();
		while (std::chrono::system_clock::now() - start < timeout)
		{
			std::this_thread::sleep_for(d);
			std::invoke(fun, args...);
		}
	});
}

int main()
{
	using namespace std::chrono_literals;
	timer([] { std::cout << "."; }, 1000ms, 5000ms);
	std::cout << "\ndone\n";
}