#include <boost/asio.hpp>

#include <thread>
#include <atomic>
#include <memory>
#include <iostream>

using namespace boost;

class Service {
public:
	Service() {}

	void HandleClient(asio::ip::tcp::socket& sock) {
		try {
			asio::streambuf request;
			asio::read_until(sock, request, '\n');

			// Emulate request processing.
			int i = 0;
			while (i != 1000000)
				i++;
			std::this_thread::sleep_for(
				std::chrono::milliseconds(500));

			// Sending response.
			std::string response = "Response\n";
			asio::write(sock, asio::buffer(response));
		}
		catch (system::system_error &e) {
			std::cout << "Error occured! Error code = "
				<< e.code() << ". Message: "
				<< e.what();
		}
	}
};

class Acceptor {
public:
	Acceptor(asio::io_service& ios, unsigned short port_num) :
		m_ios(ios),
		m_acceptor(m_ios,
			asio::ip::tcp::endpoint(asio::ip::address_v4::from_string("192.168.81.1"), port_num))
	{
		m_acceptor.listen();
	}

	~Acceptor()
	{
		std::cout << "delete asio::ip::tcp::socket class" << std::endl;
		delete m_socket;
	}

	void Accept(bool* connectstate) {

		m_socket = new asio::ip::tcp::socket(m_ios);

		m_acceptor.accept(*m_socket);

		// Accept and Handshaking ok
		*connectstate = true;

		/*Service svc;
		svc.HandleClient(sock);*/
	}

private:
	asio::io_service& m_ios;
	asio::ip::tcp::acceptor m_acceptor;

public:
	asio::ip::tcp::socket *m_socket;
};

class Server {
public:
	Server() : m_stop(false) {}

	~Server()
	{
		std::cout << "delete Acceptor class" << std::endl;
		delete acc;
	}

	void Start(unsigned short port_num) {
		m_thread.reset(new std::thread([this, port_num]() {
			Run(port_num);
		}));
	}

	void Stop() {
		m_stop.store(true);
		m_thread->join();
	}

	asio::ip::tcp::socket* get_socket()
	{
		return acc->m_socket;
	}

private:
	void Run(unsigned short port_num) {

		acc = new Acceptor(m_ios, port_num);

		acc->Accept(&m_bConnectState);

		/*while (!m_stop.load()) {
			acc.Accept();
		}*/
	}

	std::unique_ptr<std::thread> m_thread;
	std::atomic<bool> m_stop;
	asio::io_service m_ios;

public:
	bool m_bConnectState = false;

	Acceptor *acc;
};

int main()
{
	unsigned short port_num = 8090;

	try {

		int i = 0;

		Server srv;
		srv.Start(port_num);

		while (!srv.m_bConnectState)
		{
			std::cout << "Acceptor wait" << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(3));
		}

		asio::ip::tcp::socket *test_socket = srv.get_socket();

		while (true)
		{

			if (i > 10)
			{
				break;
			}

			std::string response = "Write Response" + std::to_string(i) + "\n";
			asio::write(*test_socket, asio::buffer(response));

			std::cout << "Send Data to client" << std::endl;
			i++;

			std::this_thread::sleep_for(std::chrono::seconds(3));

		}

		std::this_thread::sleep_for(std::chrono::seconds(60));

		srv.Stop();
	}
	catch (system::system_error &e) {
		std::cout << "Error occured! Error code = "
			<< e.code() << ". Message: "
			<< e.what();
	}

	return 0;
}

// This is Original SRC
//#include <boost/asio.hpp>
//
//#include <thread>
//#include <atomic>
//#include <memory>
//#include <iostream>
//
//using namespace boost;
//
//class Service {
//public:
//	Service() {}
//
//	void HandleClient(asio::ip::tcp::socket& sock) {
//		try {
//			asio::streambuf request;
//			asio::read_until(sock, request, '\n');
//
//			// Emulate request processing.
//			int i = 0;
//			while (i != 1000000)
//				i++;
//			std::this_thread::sleep_for(
//				std::chrono::milliseconds(500));
//
//			// Sending response.
//			std::string response = "Response\n";
//			asio::write(sock, asio::buffer(response));
//		}
//		catch (system::system_error &e) {
//			std::cout << "Error occured! Error code = "
//				<< e.code() << ". Message: "
//				<< e.what();
//		}
//	}
//};
//
//class Acceptor {
//public:
//	Acceptor(asio::io_service& ios, unsigned short port_num) :
//		m_ios(ios),
//		m_acceptor(m_ios,
//			asio::ip::tcp::endpoint(asio::ip::address_v4::any(), port_num))
//	{
//		m_acceptor.listen();
//	}
//
//	void Accept() {
//		asio::ip::tcp::socket sock(m_ios);
//
//		m_acceptor.accept(sock);
//
//		Service svc;
//		svc.HandleClient(sock);
//	}
//
//private:
//	asio::io_service& m_ios;
//	asio::ip::tcp::acceptor m_acceptor;
//};
//
//class Server {
//public:
//	Server() : m_stop(false) {}
//
//	void Start(unsigned short port_num) {
//		m_thread.reset(new std::thread([this, port_num]() {
//			Run(port_num);
//		}));
//	}
//
//	void Stop() {
//		m_stop.store(true);
//		m_thread->join();
//	}
//
//private:
//	void Run(unsigned short port_num) {
//		Acceptor acc(m_ios, port_num);
//
//		while (!m_stop.load()) {
//			acc.Accept();
//		}
//	}
//
//	std::unique_ptr<std::thread> m_thread;
//	std::atomic<bool> m_stop;
//	asio::io_service m_ios;
//};
//
//int main()
//{
//	unsigned short port_num = 8090;
//
//	try {
//		Server srv;
//		srv.Start(port_num);
//
//		std::this_thread::sleep_for(std::chrono::seconds(60));
//
//		srv.Stop();
//	}
//	catch (system::system_error &e) {
//		std::cout << "Error occured! Error code = "
//			<< e.code() << ". Message: "
//			<< e.what();
//	}
//
//	return 0;
//}