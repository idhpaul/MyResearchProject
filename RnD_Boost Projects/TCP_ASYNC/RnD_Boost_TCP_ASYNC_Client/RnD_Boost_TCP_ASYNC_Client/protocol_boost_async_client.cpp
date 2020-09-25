#include <boost/asio.hpp>

#include "MyProto.h"

#include <cstring>
#include <iostream>
#include <memory>
#include <string>

char recvbuffer[6];
char sendbuffer[3] = "hi";

static void boostErrorHandler(const char* BeforeFucName, const int BeforeFucLine, const boost::system::error_code& ec)
{

}

class ClientSession
{
public:

	ClientSession(std::string serverIP, std::string serverPort)
		: _work(new boost::asio::io_service::work(_ioCtx)),
		_resolver(_ioCtx),
		_socket(_ioCtx)
	{
		auto endpoints = _resolver.resolve(serverIP, serverPort);

		_worker = std::thread([&]() {
			_ioCtx.run();
			});

		do_connect(endpoints);
	};

	~ClientSession()
	{
#if _DEBUG
		std::cout << "[DEBUG]  Destructor call" << "(" << __FUNCTION__ << " : " << __LINE__ << ")" << std::endl;
#endif
		_ioCtx.stop();
		_worker.join();
		_work.reset();
	};

	void do_SESSION_INIT()
	{

#if _DEBUG
		std::cout << "[DEBUG] do_SESSION_INIT : " << "(" << __FUNCTION__ << " : " << __LINE__ << ")" << std::endl;
#endif

		StartProto();

		int bodyLength = 0;
		My_Net::Session mySession;

		mySession.set_host("MYTX");

		auto get_User = []() -> std::string {
			std::string returnName;
			char szUserName[100];
			unsigned long dwNameLength = 100;
			GetUserNameA(szUserName, &dwNameLength);

			char szComputerName[100];
			unsigned long dwComputerLength = 100;
			GetComputerNameA(szComputerName, &dwComputerLength);

			returnName.append(szUserName).append(":").append(szComputerName);

			return returnName;
		};
		mySession.set_user_agent(get_User());

		mySession.set_content_type("MY_SESSION_INIT");
		*mySession.mutable_date() = google::protobuf::util::TimeUtil::SecondsToTimestamp(time(NULL) + (3600 * 9));

		while (true)
		{
			std::string tmp_message;

			std::cout << "Enter message(or leave blank to finish): ";
			std::getline(std::cin, tmp_message);

			if (tmp_message.empty())
			{
				bodyLength = tmp_message.length();
				break;
			}
			else
			{
				bodyLength = tmp_message.length();
				break;
			}
		}

		mySession.set_content_length(bodyLength);

		std::string SerializedStringMessage;
		SerializedStringMessage = mySession.SerializeAsString() + "\r\n";

		CopyBuffer(SerializedStringMessage, SerializedStringMessage.length());

		StopProto();

		Write();

	};
	void do_SESSION_IDENTIFIED()
	{

#if _DEBUG
		std::cout << "[DEBUG] do_SESSION_IDENTIFIED : " << "(" << __FUNCTION__ << " : " << __LINE__ << ")" << std::endl;
#endif

		StartProto();

		int bodyLength = 0;
		My_Net::Session mySession;

		mySession.set_host("MYTX");

		auto get_User = []() -> std::string {
			std::string returnName;
			char szUserName[100];
			unsigned long dwNameLength = 100;
			GetUserNameA(szUserName, &dwNameLength);

			char szComputerName[100];
			unsigned long dwComputerLength = 100;
			GetComputerNameA(szComputerName, &dwComputerLength);

			returnName.append(szUserName).append(":").append(szComputerName);

			return returnName;
		};
		mySession.set_user_agent(get_User());

		mySession.set_content_type("MY_SESSION_IDENTIFIED");
		*mySession.mutable_date() = google::protobuf::util::TimeUtil::SecondsToTimestamp(time(NULL) + (3600 * 9));

		while (true)
		{
			std::string tmp_message;

			std::cout << "Enter message(or leave blank to finish): ";
			std::getline(std::cin, tmp_message);

			if (tmp_message.empty())
			{
				bodyLength = tmp_message.length();
				break;
			}
			else
			{
				bodyLength = tmp_message.length();
				break;
			}
		}

		mySession.set_content_length(bodyLength);

		std::string SerializedStringMessage;
		SerializedStringMessage = mySession.SerializeAsString() + "\r\n";

		CopyBuffer(SerializedStringMessage, SerializedStringMessage.length());

		StopProto();

		Write();
	};
	void do_SESSION_CREATE()
	{

#if _DEBUG
		std::cout << "[DEBUG] do_SESSION_CREATE : " << "(" << __FUNCTION__ << " : " << __LINE__ << ")" << std::endl;
#endif

		StartProto();

		int bodyLength = 0;
		My_Net::Session mySession;

		mySession.set_host("MYTX");

		auto get_User = []() -> std::string {
			std::string returnName;
			char szUserName[100];
			unsigned long dwNameLength = 100;
			GetUserNameA(szUserName, &dwNameLength);

			char szComputerName[100];
			unsigned long dwComputerLength = 100;
			GetComputerNameA(szComputerName, &dwComputerLength);

			returnName.append(szUserName).append(":").append(szComputerName);

			return returnName;
		};
		mySession.set_user_agent(get_User());

		mySession.set_content_type("MY_SESSION_CREATE");
		*mySession.mutable_date() = google::protobuf::util::TimeUtil::SecondsToTimestamp(time(NULL) + (3600 * 9));

		while (true)
		{
			std::string tmp_message;

			std::cout << "Enter message(or leave blank to finish): ";
			std::getline(std::cin, tmp_message);

			if (tmp_message.empty())
			{
				bodyLength = tmp_message.length();
				break;
			}
			else
			{
				bodyLength = tmp_message.length();
				break;
			}
		}

		mySession.set_content_length(bodyLength);

		std::string SerializedStringMessage;
		SerializedStringMessage = mySession.SerializeAsString() + "\r\n";

		CopyBuffer(SerializedStringMessage, SerializedStringMessage.length());

		StopProto();

		Write();
	};
	void do_SESSION_START()
	{

#if _DEBUG
		std::cout << "[DEBUG] do_SESSION_START : " << "(" << __FUNCTION__ << " : " << __LINE__ << ")" << std::endl;
#endif

		StartProto();

		int bodyLength = 0;
		My_Net::Session mySession;

		mySession.set_host("MYTX");

		auto get_User = []() -> std::string {
			std::string returnName;
			char szUserName[100];
			unsigned long dwNameLength = 100;
			GetUserNameA(szUserName, &dwNameLength);

			char szComputerName[100];
			unsigned long dwComputerLength = 100;
			GetComputerNameA(szComputerName, &dwComputerLength);

			returnName.append(szUserName).append(":").append(szComputerName);

			return returnName;
		};
		mySession.set_user_agent(get_User());

		mySession.set_content_type("MY_SESSION_START");
		*mySession.mutable_date() = google::protobuf::util::TimeUtil::SecondsToTimestamp(time(NULL) + (3600 * 9));

		while (true)
		{
			std::string tmp_message;

			std::cout << "Enter message(or leave blank to finish): ";
			std::getline(std::cin, tmp_message);

			if (tmp_message.empty())
			{
				bodyLength = tmp_message.length();
				break;
			}
			else
			{
				bodyLength = tmp_message.length();
				break;
			}
		}

		mySession.set_content_length(bodyLength);

		std::string SerializedStringMessage;
		SerializedStringMessage = mySession.SerializeAsString() + "\r\n";

		CopyBuffer(SerializedStringMessage, SerializedStringMessage.length());

		StopProto();

		Write();
	};
	void do_SESSION_STOP()
	{

#if _DEBUG
		std::cout << "[DEBUG] do_SESSION_STOP : " << "(" << __FUNCTION__ << " : " << __LINE__ << ")" << std::endl;
#endif

		StartProto();

		int bodyLength = 0;
		My_Net::Session mySession;

		mySession.set_host("MYTX");

		auto get_User = []() -> std::string {
			std::string returnName;
			char szUserName[100];
			unsigned long dwNameLength = 100;
			GetUserNameA(szUserName, &dwNameLength);

			char szComputerName[100];
			unsigned long dwComputerLength = 100;
			GetComputerNameA(szComputerName, &dwComputerLength);

			returnName.append(szUserName).append(":").append(szComputerName);

			return returnName;
		};
		mySession.set_user_agent(get_User());

		mySession.set_content_type("MY_SESSION_STOP");
		*mySession.mutable_date() = google::protobuf::util::TimeUtil::SecondsToTimestamp(time(NULL) + (3600 * 9));

		while (true)
		{
			std::string tmp_message;

			std::cout << "Enter message(or leave blank to finish): ";
			std::getline(std::cin, tmp_message);

			if (tmp_message.empty())
			{
				bodyLength = tmp_message.length();
				break;
			}
			else
			{
				bodyLength = tmp_message.length();
				break;
			}
		}

		mySession.set_content_length(bodyLength);

		std::string SerializedStringMessage;
		SerializedStringMessage = mySession.SerializeAsString() + "\r\n";

		CopyBuffer(SerializedStringMessage, SerializedStringMessage.length());

		StopProto();

		Write();
	};
	void do_SESSION_RESET()
	{

#if _DEBUG
		std::cout << "[DEBUG] do_SESSION_RESET : " << "(" << __FUNCTION__ << " : " << __LINE__ << ")" << std::endl;
#endif
		StartProto();

		int bodyLength = 0;
		My_Net::Session mySession;

		mySession.set_host("MYTX");

		auto get_User = []() -> std::string {
			std::string returnName;
			char szUserName[100];
			unsigned long dwNameLength = 100;
			GetUserNameA(szUserName, &dwNameLength);

			char szComputerName[100];
			unsigned long dwComputerLength = 100;
			GetComputerNameA(szComputerName, &dwComputerLength);

			returnName.append(szUserName).append(":").append(szComputerName);

			return returnName;
		};
		mySession.set_user_agent(get_User());

		mySession.set_content_type("MY_SESSION_RESET");
		*mySession.mutable_date() = google::protobuf::util::TimeUtil::SecondsToTimestamp(time(NULL) + (3600 * 9));

		while (true)
		{
			std::string tmp_message;

			std::cout << "Enter message(or leave blank to finish): ";
			std::getline(std::cin, tmp_message);

			if (tmp_message.empty())
			{
				bodyLength = tmp_message.length();
				break;
			}
			else
			{
				bodyLength = tmp_message.length();
				break;
			}
		}

		mySession.set_content_length(bodyLength);

		std::string SerializedStringMessage;
		SerializedStringMessage = mySession.SerializeAsString() + "\r\n";

		CopyBuffer(SerializedStringMessage, SerializedStringMessage.length());

		StopProto();

		Write();
	};
	void do_SESSION_DELETE()
	{

#if _DEBUG
		std::cout << "[DEBUG] do_SESSION_DELETE : " << "(" << __FUNCTION__ << " : " << __LINE__ << ")" << std::endl;
#endif

		StartProto();

		int bodyLength = 0;
		My_Net::Session mySession;

		mySession.set_host("MYTX");

		auto get_User = []() -> std::string {
			std::string returnName;
			char szUserName[100];
			unsigned long dwNameLength = 100;
			GetUserNameA(szUserName, &dwNameLength);

			char szComputerName[100];
			unsigned long dwComputerLength = 100;
			GetComputerNameA(szComputerName, &dwComputerLength);

			returnName.append(szUserName).append(":").append(szComputerName);

			return returnName;
		};
		mySession.set_user_agent(get_User());

		mySession.set_content_type("MY_SESSION_DELETE");
		*mySession.mutable_date() = google::protobuf::util::TimeUtil::SecondsToTimestamp(time(NULL) + (3600 * 9));

		while (true)
		{
			std::string tmp_message;

			std::cout << "Enter message(or leave blank to finish): ";
			std::getline(std::cin, tmp_message);

			if (tmp_message.empty())
			{
				bodyLength = tmp_message.length();
				break;
			}
			else
			{
				bodyLength = tmp_message.length();
				break;
			}
		}

		mySession.set_content_length(bodyLength);

		std::string SerializedStringMessage;
		SerializedStringMessage = mySession.SerializeAsString() + "\r\n";

		CopyBuffer(SerializedStringMessage, SerializedStringMessage.length());

		StopProto();

		Write();
	};

	

private:

	void do_connect(const boost::asio::ip::tcp::resolver::results_type& endpoints)
	{
		boost::asio::async_connect(_socket, endpoints,
			[this](boost::system::error_code ec, boost::asio::ip::tcp::endpoint)
			{
				std::cout << "do connect" << "(" << __FUNCTION__ << " : " << __LINE__ << ")" << std::endl;

				if (!ec)
				{

					set_SocketOption();

					do_SESSION_INIT();
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
					std::cout << "[DEBUG] Read Data from Client msg : "<< recvbuffer << "(" << __FUNCTION__ << " : " << __LINE__ << ")" << std::endl;
					std::cout << "[DEBUG] Read recv : " << length << std::endl;
#endif
				}
				else
				{
					boostErrorHandler(__FUNCTION__, __LINE__, ec);
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
					std::cout << "[DEBUG] Write Data from Client " << "(" << __FUNCTION__ << " : " << __LINE__ << ")" << std::endl;
					std::cout << "[DEBUG] Write length : " << length << std::endl;
#endif

					Read();

				}
				else
				{
					boostErrorHandler(__FUNCTION__, __LINE__, ec);
				}
			});
	};

	void set_SocketOption()
	{
		boost::asio::socket_base::keep_alive KeepAliveoption(true);
		_socket.set_option(KeepAliveoption);

		boost::asio::socket_base::linger Lingeroption(true, 30);
		_socket.set_option(Lingeroption);
	};
	
	void CopyBuffer(const std::string& str, int length)
	{
		std::memset(_buffer, 0x0, 100);

		_bufferLength = length;

		std::memset(_buffer, 0x0, _bufferLength);
		std::memcpy(_buffer, str.c_str(), _bufferLength);
	}



	// TODO: 동적 버퍼
	char _buffer[100];
	int _bufferLength;

	boost::asio::io_context _ioCtx;
	boost::asio::ip::tcp::resolver _resolver;
	boost::asio::ip::tcp::socket _socket;

	std::thread _worker;
	std::shared_ptr<boost::asio::io_service::work> _work;

};

int main()
{

	ClientSession client("localhost","8090");

	//client.do_SESSION_INIT();
	Sleep(10000);

	client.do_SESSION_IDENTIFIED();
	//Sleep(10000);

	client.do_SESSION_CREATE();
	//Sleep(10000);

	client.do_SESSION_START();
	//Sleep(10000);

	client.do_SESSION_STOP();
	//Sleep(10000);

	client.do_SESSION_RESET();
	//Sleep(10000);

	client.do_SESSION_DELETE();
	//Sleep(10000);
	while (true)
	{

	}


	return 0;
}