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