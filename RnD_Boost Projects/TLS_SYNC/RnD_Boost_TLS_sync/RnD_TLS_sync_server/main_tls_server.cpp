#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include <thread>
#include <atomic>
#include <iostream>

using namespace boost;

class Service {
public:
	Service() {}

	void handle_client(asio::ssl::stream<asio::ip::tcp::socket>& ssl_stream)
	{
		try {
			// Blocks until the handshake completes.
			ssl_stream.handshake(asio::ssl::stream_base::server);

			asio::streambuf request;
			asio::read_until(ssl_stream, request, '\n');

			// Emulate request processing.
			int i = 0;
			while (i != 1000000)
				i++;
			std::this_thread::sleep_for(
				std::chrono::milliseconds(500));

			// Sending response.
			std::string response = "Response\n";
			asio::write(ssl_stream, asio::buffer(response));

			std::cout << "Send Data to client" << std::endl;
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
			asio::ip::tcp::endpoint(
				asio::ip::address_v4::from_string("192.168.81.1"),
				port_num)),
		m_tls_context(asio::ssl::context::sslv3)
	{
		// Setting up the context.
		m_tls_context.set_options(
			boost::asio::ssl::context::default_workarounds
			//| boost::asio::ssl::context::no_sslv2
			| boost::asio::ssl::context::single_dh_use);


		m_tls_context.use_certificate_chain_file("root.pem");
		m_tls_context.use_private_key_file("root.key", boost::asio::ssl::context::pem);
		m_tls_context.use_tmp_dh_file("dh2048.pem");

		// Start listening for incoming connection requests.
		m_acceptor.listen();
	}

	~Acceptor()
	{
		delete m_tls_stream;
	}

	void accept(bool* connectstate) {

		try {
			m_tls_stream = new asio::ssl::stream<asio::ip::tcp::socket>(m_ios, m_tls_context);

			m_acceptor.accept(m_tls_stream->lowest_layer());

			m_tls_stream->handshake(asio::ssl::stream_base::server);

			// Accept and Handshaking ok
			*connectstate = true;

		}
		catch (system::system_error &e) {
			std::cout << "Error occured! Error code = "
				<< e.code() << ". Message: "
				<< e.what();

		}

	}

private:
	asio::io_service& m_ios;
	asio::ip::tcp::acceptor m_acceptor;

	asio::ssl::context m_tls_context;

public:
	asio::ssl::stream<asio::ip::tcp::socket> *m_tls_stream;

};

class Server {
public:
	Server() : m_stop(false) {}

	void start(unsigned short port_num) {
		run(port_num);

		/*m_thread.reset(new std::thread([this, port_num]() {
			run(port_num);
		}));*/
	}

	void stop() {
		m_stop.store(true);
		//m_thread->join();
	}

	asio::ssl::stream<asio::ip::tcp::socket>* get_tls_stream()
	{
		return acc->m_tls_stream;
	}

	~Server()
	{
		delete acc;
	}

private:
	void run(unsigned short port_num) {
		acc = new Acceptor(m_ios, port_num);

		acc->accept(&bConnectState);


		/*while (!m_stop.load()) {
			acc->accept();
		}*/
	}


	//std::unique_ptr<std::thread> m_thread;
	std::atomic<bool> m_stop;
	asio::io_service m_ios;

public:

	bool bConnectState = false;

	Acceptor *acc;

};

int main()
{
	unsigned short port_num = 9090;

	int i = 0;

	try {
		Server srv;
		srv.start(port_num);

		while (!srv.bConnectState)
		{
			std::cout << "Acceptor wait" << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(3));
		}

		asio::ssl::stream<asio::ip::tcp::socket> *test_tls_stream = srv.get_tls_stream();

		while (true)
		{
			try {

				if (i > 10)
				{
					break;
				}

				std::string response = "Write Response" + std::to_string(i) + "\n";
				asio::write(*test_tls_stream, asio::buffer(response));

				std::cout << "Send Data to client" << std::endl;
				i++;

				std::this_thread::sleep_for(std::chrono::seconds(3));

			}
			catch (system::system_error &e) {
				std::cout << "Error occured! Error code = "
					<< e.code() << ". Message: "
					<< e.what();
			}
		}

		srv.stop();
	}
	catch (system::system_error &e) {
		std::cout << "Error occured! Error code = "
			<< e.code() << ". Message: "
			<< e.what();
	}

	return 0;
}


//This is Original SRC
//#include <boost/asio.hpp>
//#include <boost/asio/ssl.hpp>
//
//#include <thread>
//#include <atomic>
//#include <iostream>
//
//using namespace boost;
//
//class Service {
//public:
//	Service() {}
//
//	void handle_client(asio::ssl::stream<asio::ip::tcp::socket>& ssl_stream)
//	{
//		try {
//			// Blocks until the handshake completes.
//			ssl_stream.handshake(asio::ssl::stream_base::server);
//
//			asio::streambuf request;
//			asio::read_until(ssl_stream, request, '\n');
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
//			asio::write(ssl_stream, asio::buffer(response));
//
//			std::cout << "Send Data to client" << std::endl;
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
//			asio::ip::tcp::endpoint(
//				asio::ip::address_v4::any(),
//				port_num)),
//		m_tls_context(asio::ssl::context::tls_server)
//	{
//		// Setting up the context.
//		m_tls_context.set_options(
//			boost::asio::ssl::context::default_workarounds
//			| boost::asio::ssl::context::no_sslv2
//			| boost::asio::ssl::context::single_dh_use);
//
//
//		m_tls_context.use_certificate_chain_file("root.pem");
//		m_tls_context.use_private_key_file("root.key", boost::asio::ssl::context::pem);
//		m_tls_context.use_tmp_dh_file("dh2048.pem");
//
//		// Start listening for incoming connection requests.
//		m_acceptor.listen();
//	}
//
//	~Acceptor()
//	{
//		delete m_tls_stream;
//	}
//
//	void accept(bool* connectstate) {
//
//		try {
//			m_tls_stream = new asio::ssl::stream<asio::ip::tcp::socket>(m_ios, m_tls_context);
//
//			m_acceptor.accept(m_tls_stream->lowest_layer());
//
//			m_tls_stream->handshake(asio::ssl::stream_base::server);
//
//			// Accept and Handshaking ok
//			*connectstate = true;
//
//		}
//		catch (system::system_error &e) {
//			std::cout << "Error occured! Error code = "
//				<< e.code() << ". Message: "
//				<< e.what();
//
//		}
//
//	}
//
//private:
//	asio::io_service& m_ios;
//	asio::ip::tcp::acceptor m_acceptor;
//
//	asio::ssl::context m_tls_context;
//
//public:
//	asio::ssl::stream<asio::ip::tcp::socket> *m_tls_stream;
//
//};
//
//class Server {
//public:
//	Server() : m_stop(false) {}
//
//	void start(unsigned short port_num) {
//		run(port_num);
//
//		/*m_thread.reset(new std::thread([this, port_num]() {
//			run(port_num);
//		}));*/
//	}
//
//	void stop() {
//		m_stop.store(true);
//		//m_thread->join();
//	}
//
//	asio::ssl::stream<asio::ip::tcp::socket>* get_tls_stream()
//	{
//		return acc->m_tls_stream;
//	}
//
//	~Server()
//	{
//		delete acc;
//	}
//
//private:
//	void run(unsigned short port_num) {
//		acc = new Acceptor(m_ios, port_num);
//
//		acc->accept(&bConnectState);
//
//
//		/*while (!m_stop.load()) {
//			acc->accept();
//		}*/
//	}
//
//
//	//std::unique_ptr<std::thread> m_thread;
//	std::atomic<bool> m_stop;
//	asio::io_service m_ios;
//
//public:
//
//	bool bConnectState = false;
//
//	Acceptor *acc;
//
//};
//
//int main()
//{
//	unsigned short port_num = 9090;
//
//	int i = 0;
//
//	try {
//		Server srv;
//		srv.start(port_num);
//
//		while (!srv.bConnectState)
//		{
//			std::cout << "Acceptor wait" << std::endl;
//			std::this_thread::sleep_for(std::chrono::seconds(3));
//		}
//
//		asio::ssl::stream<asio::ip::tcp::socket> *test_tls_stream = srv.get_tls_stream();
//
//		while (true)
//		{
//			try {
//
//				if (i > 10)
//				{
//					break;
//				}
//
//				std::string response = "Write Response" + std::to_string(i) + "\n";
//				asio::write(*test_tls_stream, asio::buffer(response));
//
//				std::cout << "Send Data to client" << std::endl;
//				i++;
//
//				std::this_thread::sleep_for(std::chrono::seconds(3));
//
//			}
//			catch (system::system_error &e) {
//				std::cout << "Error occured! Error code = "
//					<< e.code() << ". Message: "
//					<< e.what();
//			}
//		}
//
//		srv.stop();
//	}
//	catch (system::system_error &e) {
//		std::cout << "Error occured! Error code = "
//			<< e.code() << ". Message: "
//			<< e.what();
//	}
//
//	return 0;
//}
