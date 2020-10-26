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
	// 생성자에서 여러가지들을 초기화 해줘야 한다.
	// service는 독립적으로 초기화가 가능하고,
	// work는 io 스레드를 블락시켜줄 도구이다. 이 도구를 초기화 할땐 service를 넣는다.
	// room에서는 strand를 사용하므로 service를 넣어줘야한다. 동적할당으로 생성을 미뤄도 된다.
	CManager() : service(), work(service), room1(service, 1), room2(service, 2)
	{
		// io 스레드를 미리 돌려 준다.
		for (int i = 0; i < 4; i++)
		{
			io_threads.create_thread(boost::bind(&boost::asio::io_service::run, &service));
		}
	}

	~CManager()
	{
		// boost io_service 종료를 보장하는 방법은 다음과 같다.
		service.stop();
		io_threads.join_all();
	}

	// boost bind를 통해서 함수를 맵핑하고 strand로 비동기 수행을 요청하는 방법이다.
	void PostPrintRoom1(int n) {
		service.post(room1.m_strand.wrap(boost::bind(&CRoom::PrintRoom, room1, n)));
	}

	// boost bind를 사용하지 않고, 람다를 사용하여 직접 함수를 만든 뒤, room2의 strand로 비동기 수행을 요청한다.
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

		// 1을 입력 받으면 Room1에서 PrintRoom(5)를 명령한다.
		else if (opt == '1')
		{
			manager.PostPrintRoom1(5);
		}

		// 2를 입력 받으면 Room1에서 PrintRoom(5)를 명령한다.
		else if (opt == '2')
		{
			manager.PostPrintRoom2(5);
		}
	}

	return 0;
}