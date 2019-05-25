#include <iostream>
#include <thread>
using namespace std;

//스레드에 의해 호출되는 함수
void call_from_thread(int tid) {
	cout << "스레드 실행 " << tid << std::endl;
}

int main() {
	thread t[10];

	//10개의 스레드 시작
	for (int i = 0; i < 10; ++i) {
		t[i] = thread(call_from_thread, i);
	}

	std::cout << "메인 함수 시작" << endl;

	//스레드가 종료될 때 까지 대시
	for (int i = 0; i < 10; ++i) {
		t[i].join();
	}

	return 0;
}