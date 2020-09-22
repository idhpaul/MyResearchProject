#define _CRT_SECURE_NO_WARNINGS

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
	MySession(boost::asio::io_context& io_context,
		const boost::asio::ip::tcp::resolver::results_type& endpoints)
		: _io_context(io_context),
		_socket(io_context),
		_buffer(std::make_unique<char>(3)),
		_recvbuffer(std::make_unique<char>(5))

	{
		do_connect(endpoints);
	}

	void read()
	{
		std::cout << "read read" << std::endl;

		boost::asio::async_read(_socket,
			boost::asio::buffer(_recvbuffer.get(), 5),
			[this](boost::system::error_code ec, std::size_t length)
			{
				std::cout << "read sjpark" << std::endl;


				if (!ec)
				{
#if _DEBUG
					std::cout << "[DEBUG] Read Data from Server " << __FUNCTION__ << __LINE__ << std::endl;
					std::cout << "[DEBUG] recv : " << _recvbuffer.get() << length <<std::endl;
#endif
				}
				else
				{
#if _DEBUG
					std::cout << "[DEBUG] read err " << __FUNCTION__ << __LINE__ << std::endl;
#endif
					close();
				}
			});
	}

	void write()
	{

		boost::asio::async_write(_socket,
			boost::asio::buffer(_buffer.get(), 3),
			[this](boost::system::error_code ec, std::size_t /*length*/)
			{
				
				if (!ec)
				{
#if _DEBUG
					std::cout << "[DEBUG] Write Data to Server " << __FUNCTION__ << __LINE__ << std::endl;
					std::cout << "write call" << std::endl;
#endif
				}
				else
				{

#if _DEBUG
					std::cout << "[DEBUG] Write err " << __FUNCTION__ << __LINE__ << std::endl;
#endif

					close();
				}
			});
	}

	void close()
	{
		std::cout << "close call" << std::endl;
		_socket.close();
	}

private:
	void do_connect(const boost::asio::ip::tcp::resolver::results_type& endpoints)
	{
		boost::asio::async_connect(_socket, endpoints,
			[this](boost::system::error_code ec, boost::asio::ip::tcp::endpoint)
			{
				if (!ec)
				{
					//read();
					std::cout << "Connect lamda called" << std::endl;
				}
				else
				{
					close();
				}
			});
	}

public:
	std::unique_ptr<char> _buffer;


private:
	boost::asio::io_context&		_io_context;
	boost::asio::ip::tcp::socket	_socket;
	std::unique_ptr<char> _recvbuffer;

};

int main()
{

	try
	{
		boost::asio::io_context::count_type ioRes;
		boost::asio::io_context io_context;

		boost::asio::ip::tcp::resolver resolver(io_context);
		auto endpoints = resolver.resolve("localhost", "8090");

		MySession c(io_context, endpoints);

		std::thread t([&io_context]() { io_context.run(); });

		while (std::cin.getline(c._buffer.get(), 3))
		{

			if (c._buffer.get() == "q")
			{
				std::cout << "Quit" << std::endl;
				break;
			}

			c.write();
			c.read();
		}

		c.close();
		t.join();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}