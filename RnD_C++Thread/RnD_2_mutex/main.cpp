#include <iostream>
#include <thread>
#include <mutex>
using namespace std;

int value;
mutex value_mutex;

void increase_value()
{
	// 뮤텍스를 이용하여 동기화
	value_mutex.lock();
	value++;
	cout << value << endl;
	value_mutex.unlock();
}

int main()
{
	value = 0;
	thread t[10];

	for (auto i = 0; i < 10; i++)
	{
		// 스레드 생성
		t[i] = thread(increase_value);
	}

	for (int i = 0; i < 10; i++)
	{
		// 스레드 대기 종료
		t[i].join();
	}
}