#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/chrono.hpp>
#include <boost/thread.hpp>

#include <iostream>

class CRoom {
public:
	CRoom(boost::asio::io_service& service, const int& number) : m_strand(service), roomNumber(number) {}
	boost::asio::io_service::strand m_strand;

	void PrintRoom(int n) {
		for (int i = 0; i < n; i++)
		{
			std::printf("Room:%d - %d\n", roomNumber, i);
			boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
		}
	}

	int roomNumber;
};

class CManager
{
public:
	// �����ڿ��� ������������ �ʱ�ȭ ����� �Ѵ�.
	// service�� ���������� �ʱ�ȭ�� �����ϰ�,
	// work�� io �����带 ��������� �����̴�. �� ������ �ʱ�ȭ �Ҷ� service�� �ִ´�.
	// room������ strand�� ����ϹǷ� service�� �־�����Ѵ�. �����Ҵ����� ������ �̷ﵵ �ȴ�.
	CManager() : service(), work(service), room1(service, 1), room2(service, 2)
	{
		// io �����带 �̸� ���� �ش�.
		for (int i = 0; i < 4; i++)
		{
			io_threads.create_thread(boost::bind(&boost::asio::io_service::run, &service));
		}
	}

	~CManager()
	{
		// boost io_service ���Ḧ �����ϴ� ����� ������ ����.
		service.stop();
		io_threads.join_all();
	}

	// boost bind�� ���ؼ� �Լ��� �����ϰ� strand�� �񵿱� ������ ��û�ϴ� ����̴�.
	void PostPrintRoom1(int n) {
		service.post(room1.m_strand.wrap(boost::bind(&CRoom::PrintRoom, room1, n)));
	}

	// boost bind�� ������� �ʰ�, ���ٸ� ����Ͽ� ���� �Լ��� ���� ��, room2�� strand�� �񵿱� ������ ��û�Ѵ�.
	void PostPrintRoom2(int n) {
		service.post(room2.m_strand.wrap([this, n]() {
			for (int i = 0; i < n; i++)
			{
				std::printf("Room:%d - %d\n", room2.roomNumber, i);
				boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
			}
			}
		));
	}

private:
	boost::asio::io_service service;
	boost::thread_group io_threads;
	boost::asio::io_service::work work;

	CRoom room1, room2;
};



int main(int argc, char* argv[])
{
	CManager manager;

	char opt;
	while (1)
	{
		std::cin >> opt;

		if (opt == 'x')
		{
			break;
		}

		// 1�� �Է� ������ Room1���� PrintRoom(5)�� ����Ѵ�.
		else if (opt == '1')
		{
			manager.PostPrintRoom1(5);
		}

		// 2�� �Է� ������ Room1���� PrintRoom(5)�� ����Ѵ�.
		else if (opt == '2')
		{
			manager.PostPrintRoom2(5);
		}
	}

	return 0;
}