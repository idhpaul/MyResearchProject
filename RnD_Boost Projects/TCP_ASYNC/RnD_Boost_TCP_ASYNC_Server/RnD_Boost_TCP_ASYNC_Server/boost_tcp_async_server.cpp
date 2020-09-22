#include <cstring>
#include <iostream>
#include <memory>
#include <thread>
#include <string>
#include <queue>
#include <boost/asio.hpp>

class MySession
{
public:
	MySession(boost::asio::ip::tcp::socket socket);
	~MySession();

	void start();
	void stop();
private:

	void read();
	void write();

	boost::asio::ip::tcp::socket _socket;
	std::unique_ptr<char> _buffer;
	std::string tmp_string = "RECV";
};

MySession::MySession(boost::asio::ip::tcp::socket socket)
	: _socket(std::move(socket))
	, _buffer(std::make_unique<char>(3))
{

};
MySession::~MySession()
{
#if _DEBUG
	std::cout << "[DEBUG]  Destructor call " << __FUNCTION__ << __LINE__ << std::endl;
#endif
};

void MySession::start()
{
	read();
};

void MySession::stop()
{
#if _DEBUG
	std::cout << "[DEBUG]  mySession stop call " << __FUNCTION__ << __LINE__ << std::endl;
#endif
	_socket.close();
};

void MySession::read()
{

	boost::asio::async_read(_socket, 
		boost::asio::buffer(_buffer.get(), 3),
		[this](boost::system::error_code ec, std::size_t /*length*/)
		{
			if (!ec)
			{
#if _DEBUG
				std::cout << "[DEBUG] Read Data from Client " << __FUNCTION__ << __LINE__ << std::endl;
				std::cout << "[DEBUG] recv : " << _buffer.get() << std::endl;
#endif

				write();
			}
			else
			{
				stop();
			}
		});

};

void MySession::write()
{

	boost::asio::async_write(_socket,
		boost::asio::buffer(tmp_string.c_str(), tmp_string.size()+1),
		[this](boost::system::error_code ec, std::size_t length)
		{
			if (!ec)
			{
#if _DEBUG
				std::cout << "[DEBUG] Write Data to Client " << __FUNCTION__ << __LINE__ << std::endl;
				std::cout << "[DEBUG] send : " << length << std::endl;
#endif

				read();
			}
			else
			{
				stop();
			}
		});

};



/// //////////////////////////////////////////////////////////


class SessionGateway
{
public:
	SessionGateway(boost::asio::io_context& io_context, const boost::asio::ip::tcp::endpoint& endpoint);
	~SessionGateway();

private:

	void do_accept();

	std::unique_ptr<MySession>		_session;
	boost::asio::ip::tcp::acceptor	_acceptor;
}; 

SessionGateway::SessionGateway(boost::asio::io_context& io_context, const boost::asio::ip::tcp::endpoint& endpoint)
	: _acceptor(io_context, endpoint)
{
	do_accept();
};

SessionGateway::~SessionGateway()
{
#if _DEBUG
	std::cout << "[DEBUG]  Destructor call" << __FUNCTION__ << __LINE__ << std::endl;
#endif
}

void SessionGateway::do_accept()
{
	_acceptor.async_accept(
		[this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
		{
			if (!ec)
			{
				std::cout << "Accept ok" << std::endl;
				_session = std::make_unique<MySession>(std::move(socket));
				_session->start();
			}

			do_accept();
		});
}

int main()
{
	boost::asio::io_context::count_type ioRes;
	boost::asio::io_context io_context;

	// Session Server Socket info
	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), 8090);

	// Session Server class
	std::unique_ptr<SessionGateway> sessionSrv = std::make_unique<SessionGateway>(io_context, endpoint);

	ioRes = io_context.run();
	std::cout << ioRes << std::endl;

	return 0;
}