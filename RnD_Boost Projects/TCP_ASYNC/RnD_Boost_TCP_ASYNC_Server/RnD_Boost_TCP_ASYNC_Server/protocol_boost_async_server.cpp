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



char sendbuffer[6] = "hello";
char recvbuffer[19];

std::string make_string(boost::asio::streambuf& streambuf)
{
	return { buffers_begin(streambuf.data()),
			buffers_end(streambuf.data()) };
}

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

		boost::asio::async_read_until(_socket,
			buffer_, "\r\n",
			[this](boost::system::error_code ec, std::size_t length)
			{

					if (!ec)
					{
						#if _DEBUG
						std::cout << "[DEBUG] Read Data from Client " << __FUNCTION__ << __LINE__ << std::endl;
						std::cout << "[DEBUG] Read recv : " << length << std::endl;
						#endif

						std::string recvStr = make_string(buffer_);
						recvStr = recvStr.substr(0, recvStr.size() - 2);

						My_Net::Session mySessionRead;
						mySessionRead.ParseFromString(recvStr);

						std::cout << mySessionRead.host() << std::endl;
						std::cout << mySessionRead.user_agent() << std::endl;
						std::cout << mySessionRead.content_type() << std::endl;
						std::cout << mySessionRead.date() << std::endl;
						std::cout << mySessionRead.content_length() << std::endl;

						readSize = length;

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

						buffer_.consume(readSize);

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
	boost::asio::streambuf buffer_;
	size_t readSize;

};

class ServerAcceptor
{
public:
	ServerAcceptor(const boost::asio::ip::tcp::endpoint& endpoint)
		: _work(new boost::asio::io_service::work(_ioCtx)),
		_acceptor(_ioCtx, endpoint)
	{
		_worker = std::thread([&]() {
			_ioCtx.run();
			});

		do_accept();
	}


	~ServerAcceptor()
	{
#if _DEBUG
		std::cout << "[DEBUG]  Destructor call" << __FUNCTION__ << __LINE__ << std::endl;
#endif
		_ioCtx.stop();
		_worker.join();
		_work.reset();
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
	std::thread _worker;
	std::unique_ptr<Session> _mySession;


	boost::asio::io_context _ioCtx;
	boost::asio::ip::tcp::acceptor	_acceptor;
	std::shared_ptr<boost::asio::io_service::work> _work;
};

int main()
{

	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), 8090);

	ServerAcceptor Server(endpoint);

	while (true)
	{

	}

	return 0;
}