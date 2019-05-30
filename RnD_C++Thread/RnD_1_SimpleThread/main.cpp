#define __THREAD_VER 0

#if __THREAD_VER

#include <iostream>
#include <thread>
using namespace std;

//�����忡 ���� ȣ��Ǵ� �Լ�
void call_from_thread(int tid) {
	cout << "������ ���� " << tid << std::endl;
}

int main() {
	thread t[10];

	//10���� ������ ����
	for (int i = 0; i < 10; ++i) {
		t[i] = thread(call_from_thread, i);
	}

	std::cout << "���� �Լ� ����" << endl;

	//�����尡 ����� �� ���� ���
	for (int i = 0; i < 10; ++i) {
		t[i].join();
	}

	return 0;
}

#else
#include <thread>
#include <iostream>
#include <vector>

int count = 0;

void doSomeWork() {

	std::cout << "The doSomeWork function is running on another thread." << std::endl;
	int data = count++;
	// Pause for a moment to provide a delay to make
	// threads more apparent.
	std::this_thread::sleep_for(std::chrono::seconds(3));
	std::cout << "The function called by the worker thread has ended." << std::endl;
}

int main() {
	std::vector<std::thread> threads;

	for (int i = 0; i < 10; ++i) {

		threads.push_back(std::thread(doSomeWork));
		std::cout << "The Main() thread calls this after starting the new thread" << std::endl;
	}

	for (auto& thread : threads) {
		thread.join();
	}

	return 0;
}
#endif