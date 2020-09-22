#include <boost/asio.hpp>

#include "MyProto.h"

#include <cstring>
#include <iostream>
#include <memory>
#include <string>

char recvbuffer[6];
char sendbuffer[3] = "hi";

class ClientSession
{
public:
	ClientSession(boost::asio::io_context& io_context,
		const boost::asio::ip::tcp::resolver::results_type& endpoints)
		: _io_context(io_context),
		_socket(io_context)
	{
		do_connect(endpoints);
	};

	~ClientSession()
	{

	};

	void do_connect(const boost::asio::ip::tcp::resolver::results_type& endpoints)
	{
		boost::asio::async_connect(_socket, endpoints,
			[this](boost::system::error_code ec, boost::asio::ip::tcp::endpoint)
			{
				std::cout << "do connect" << __FUNCTION__ << __LINE__ << std::endl;

				if (!ec)
				{
					Read();
				}
			});
	};


	void Read()
	{
		boost::asio::async_read(_socket,
			boost::asio::buffer(recvbuffer, 6),
			[this](boost::system::error_code ec, std::size_t length)
			{

					if (!ec)
					{
#if _DEBUG
						std::cout << "[DEBUG] Read Data from Client " << __FUNCTION__ << __LINE__ << std::endl;
						std::cout << "[DEBUG] Read recv : " << length << std::endl;
#endif
						Write();
					}
					else
					{

					}
			});
	};

	void Write()
	{
		boost::asio::async_write(_socket,
			boost::asio::buffer(_buffer, _bufferLength),
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

					}
			});
	};

	void CopyBuffer(const std::string& str, int length)
	{
		_bufferLength = length;

		_buffer = new char[length];

		std::memset(_buffer, 0x0, _bufferLength);
		std::memcpy(_buffer, str.c_str(), _bufferLength);
	}

private:
	char* _buffer;
	int _bufferLength;

	boost::asio::io_context& _io_context;
	boost::asio::ip::tcp::socket _socket;

};

int main()
{

	StartProto();

	My_Net::Session mySession;
	InitProtoData(mySession);

	std::string SerializedStringMessage;
	SerializedStringMessage = mySession.SerializeAsString();


	boost::asio::io_context io_context;

	boost::asio::ip::tcp::resolver resolver(io_context);
	auto endpoints = resolver.resolve("localhost", "8090");

	ClientSession client(io_context, endpoints);

	std::thread t([&io_context]() { io_context.run(); });

	std::cout << "Sleep start" << std::endl;
	Sleep(5000);
	std::cout << "Sleep end" << std::endl;


	client.CopyBuffer(SerializedStringMessage, SerializedStringMessage.length());
	client.Write();

	
	//client.close();
	t.join();

	StopProto();

	return 0;
}