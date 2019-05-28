#include <iostream>
#include <thread>
#include <mutex>
using namespace std;

int value;
mutex value_mutex;

void increase_value()
{
	// ���ؽ��� �̿��Ͽ� ����ȭ
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
		// ������ ����
		t[i] = thread(increase_value);
	}

	for (int i = 0; i < 10; i++)
	{
		// ������ ��� ����
		t[i].join();
	}
}