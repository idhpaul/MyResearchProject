#define PROTOBUF_LIB_NAME	"libprotobuf"

#ifdef _DEBUG
#define DEBUG_ALLOW			"d"
#else
#define DEBUG_ALLOW			""
#endif // _DEBUG

#if defined _MT && defined _DLL

//! IF you use Dynamic Linkig
//! Please include '#define PROTOBUF_USE_DLLS' you protocolbuffer header files

#define PROTOBUF_DIR ".\\ThirdParty\\protobuf_3_13_0_shared\\lib\\"
#else
#define PROTOBUF_DIR ".\\ThirdParty\\protobuf_3_13_0_static\\lib\\"
#endif

#pragma comment(lib, PROTOBUF_DIR PROTOBUF_LIB_NAME DEBUG_ALLOW)


#include "MySession.pb.h"
#include "google/protobuf/util/time_util.h"

#include <iostream>
#include <memory>
#include <boost/asio.hpp>

boost::asio::streambuf buffer_;

char sendbuffer[6] = "hello";
char recvbuffer[19];

class Session
{
public:
	Session(boost::asio::ip::tcp::socket socket)
		: _socket(std::move(socket))
	{
		Start();
	};

	~Session()
	{

	};

	void Start()
	{
		Read();
	};

	void Stop()
	{

	};

	void Read()
	{
		// TODO: 고정 문자열이 아닌 read_until 활용하기 _ 2020.09.22 IDH
		boost::asio::async_read(_socket,
			boost::asio::buffer(recvbuffer, 19),
			[this](boost::system::error_code ec, std::size_t length)
			{
					if (!ec)
					{
						#if _DEBUG
						std::cout << "[DEBUG] Read Data from Client " << __FUNCTION__ << __LINE__ << std::endl;
						std::cout << "[DEBUG] Read recv : " << length << std::endl;
						#endif


						My_Net::Session mySessionRead;
						mySessionRead.ParseFromString(recvbuffer);

						std::cout << mySessionRead.host() << std::endl;
						std::cout << mySessionRead.user_agent() << std::endl;
						std::cout << mySessionRead.content_type() << std::endl;
						std::cout << mySessionRead.date() << std::endl;
						std::cout << mySessionRead.content_length() << std::endl;

						Write();
					}
					else
					{
						// Error Handling
					}
			});
	};

	void Write()
	{
		boost::asio::async_write(_socket,
			boost::asio::buffer(sendbuffer, 6),
			[this](boost::system::error_code ec, std::size_t length)
			{
					if (!ec)
					{
						#if _DEBUG
						std::cout << "[DEBUG] Write Data from Client " << __FUNCTION__ << __LINE__ << std::endl;
						std::cout << "[DEBUG] Write length : " << length << std::endl;
						#endif

						Read();

					}
					else
					{
						// Error Handling
					}
			});
	};

private:
	boost::asio::ip::tcp::socket _socket;
};

class ServerAcceptor
{
public:
	ServerAcceptor(boost::asio::io_context& io_context, const boost::asio::ip::tcp::endpoint& endpoint)
		: _acceptor(io_context, endpoint)
	{
		do_accept();
	}

	~ServerAcceptor()
	{
#if _DEBUG
		std::cout << "[DEBUG]  Destructor call" << __FUNCTION__ << __LINE__ << std::endl;
#endif
	}

private:

	void do_accept()
	{
		_acceptor.async_accept(
			[this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
			{
				if (!ec)
				{
					std::cout << "Client Accept success" << std::endl;
					_mySession = std::make_unique<Session>(std::move(socket));
				}

				do_accept();
			});
	}

	std::unique_ptr<Session> _mySession;
	boost::asio::ip::tcp::acceptor	_acceptor;
};

int main()
{
	boost::asio::io_context io_ctx;

	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), 8090);

	ServerAcceptor Server(io_ctx, endpoint);

	io_ctx.run();

	return 0;
}