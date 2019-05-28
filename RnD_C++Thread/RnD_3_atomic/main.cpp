#include <iostream>
#include <thread>
#include <atomic>
using namespace std;

atomic<int> intAtomic = 1;

void increase_value()
{
	//thread::id this_id = this_thread::get_id();

	intAtomic.fetch_add(1);		//값 1 증가
	cout << intAtomic << endl;
	//cout << "[Thread ID] "<< this_id <<" value : " << intAtomic << endl;

	//intAtomic.fetch_sub(2);		//값 2 감소
	//cout << intAtomic << endl;
	
}

int main()
{
	thread t[10];

	for (auto i = 0; i < 10; i++)
	{
		// 스레드 생성
		t[i] = thread(increase_value);
		//cout << "create thread id : " << t[i].get_id() << endl;
	}

	for (int i = 0; i < 10; i++)
	{
		// 스레드 대기 종료
		t[i].join();
	}
}