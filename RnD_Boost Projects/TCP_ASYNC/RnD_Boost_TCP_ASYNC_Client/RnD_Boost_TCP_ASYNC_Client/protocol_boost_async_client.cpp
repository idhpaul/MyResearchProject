#include <boost/asio.hpp>

#include "MyProto.h"

#include <deque>
#include <cstring>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>

std::string make_string(boost::asio::streambuf& streambuf)
{
	return { buffers_begin(streambuf.data()),
			buffers_end(streambuf.data()) };
}

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
		StartProto();

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

		StopProto();

		_ioCtx.stop();
		_worker.join();
		_work.reset();
	};

	void do_SESSION_INIT()
	{

#if _DEBUG
		std::cout << "[DEBUG] do_SESSION_INIT : " << "(" << __FUNCTION__ << " : " << __LINE__ << ")" << std::endl;
#endif
		std::lock_guard<std::mutex> lock_guard(_Mutex);


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

		/// <Body>
		std::string message;
		int messageLength;
		MakeMessageInit(message, &messageLength);
		/// </Body>

		mySession.set_content_length(messageLength);
		mySession.set_message(message);

		std::string SerializedStringMessage;
		SerializedStringMessage = mySession.SerializeAsString() + "\r\n";

		_write_msgs.push_back(SerializedStringMessage);

		Write();

	};
	void do_SESSION_IDENTIFIED()
	{

#if _DEBUG
		std::cout << "[DEBUG] do_SESSION_IDENTIFIED : " << "(" << __FUNCTION__ << " : " << __LINE__ << ")" << std::endl;
#endif
		std::lock_guard<std::mutex> lock_guard(_Mutex);

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

		/// <Body>
		std::string message;
		int messageLength;
		MakeMessageIdentified(message, &messageLength);
		/// </Body>

		mySession.set_content_length(messageLength);
		mySession.set_message(message);

		std::string SerializedStringMessage;
		SerializedStringMessage = mySession.SerializeAsString() + "\r\n";

		_write_msgs.push_back(SerializedStringMessage);

		Write();
	};
	void do_SESSION_CREATE()
	{

#if _DEBUG
		std::cout << "[DEBUG] do_SESSION_CREATE : " << "(" << __FUNCTION__ << " : " << __LINE__ << ")" << std::endl;
#endif
		std::lock_guard<std::mutex> lock_guard(_Mutex);

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

		/// <Body>
		std::string message;
		int messageLength;
		MakeMessageCreate(message, &messageLength);
		/// </Body>

		mySession.set_content_length(messageLength);
		mySession.set_message(message);

		std::string SerializedStringMessage;
		SerializedStringMessage = mySession.SerializeAsString() + "\r\n";

		_write_msgs.push_back(SerializedStringMessage);

		Write();
	};
	void do_SESSION_DELETE()
	{

#if _DEBUG
		std::cout << "[DEBUG] do_SESSION_DELETE : " << "(" << __FUNCTION__ << " : " << __LINE__ << ")" << std::endl;
#endif
		std::lock_guard<std::mutex> lock_guard(_Mutex);

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

		/// <Body>
		std::string message;
		int messageLength;
		MakeMessageDelete(message, &messageLength);
		/// </Body>

		mySession.set_content_length(messageLength);
		mySession.set_message(message);

		std::string SerializedStringMessage;
		SerializedStringMessage = mySession.SerializeAsString() + "\r\n";

		_write_msgs.push_back(SerializedStringMessage);

		Write();
	};
	void do_SESSION_START()
	{

#if _DEBUG
		std::cout << "[DEBUG] do_SESSION_START : " << "(" << __FUNCTION__ << " : " << __LINE__ << ")" << std::endl;
#endif
		std::lock_guard<std::mutex> lock_guard(_Mutex);

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

		/// <Body>
		std::string message;
		int messageLength;
		MakeMessageStart(message, &messageLength);
		/// </Body>

		mySession.set_content_length(messageLength);
		mySession.set_message(message);

		std::string SerializedStringMessage;
		SerializedStringMessage = mySession.SerializeAsString() + "\r\n";

		_write_msgs.push_back(SerializedStringMessage);

		Write();
	};
	void do_SESSION_STOP()
	{

#if _DEBUG
		std::cout << "[DEBUG] do_SESSION_STOP : " << "(" << __FUNCTION__ << " : " << __LINE__ << ")" << std::endl;
#endif
		std::lock_guard<std::mutex> lock_guard(_Mutex);

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

		/// <Body>
		std::string message;
		int messageLength;
		MakeMessageStop(message, &messageLength);
		/// </Body>

		mySession.set_content_length(messageLength);
		mySession.set_message(message);

		std::string SerializedStringMessage;
		SerializedStringMessage = mySession.SerializeAsString() + "\r\n";

		_write_msgs.push_back(SerializedStringMessage);

		Write();
	};
	void do_SESSION_RESET()
	{

#if _DEBUG
		std::cout << "[DEBUG] do_SESSION_RESET : " << "(" << __FUNCTION__ << " : " << __LINE__ << ")" << std::endl;
#endif
		std::lock_guard<std::mutex> lock_guard(_Mutex);

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

		/// <Body>
		std::string message;
		int messageLength;
		MakeMessageReset(message, &messageLength);
		/// </Body>

		mySession.set_content_length(messageLength);
		mySession.set_message(message);

		std::string SerializedStringMessage;
		SerializedStringMessage = mySession.SerializeAsString() + "\r\n";

		_write_msgs.push_back(SerializedStringMessage);

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
		boost::asio::async_read_until(_socket,
			_read_buffer, "\r\n",
			[this](boost::system::error_code ec, std::size_t length)
			{

				if (!ec)
				{
#if _DEBUG
					std::cout << "[DEBUG] Read Data from Client msg : "<< make_string(_read_buffer) << "(" << __FUNCTION__ << " : " << __LINE__ << ")" << std::endl;
					std::cout << "[DEBUG] Read recv : " << length << std::endl;
#endif
					_read_buffer.consume(length);
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
			boost::asio::buffer(_write_msgs.front().data(),
				_write_msgs.front().length()),
			[this](boost::system::error_code ec, std::size_t length)
			{

				if (!ec)
				{
#if _DEBUG
					std::cout << "[DEBUG] Write Data from Client " << "(" << __FUNCTION__ << " : " << __LINE__ << ")" << std::endl;
					std::cout << "[DEBUG] Write length : " << length << std::endl;
#endif
					_write_msgs.pop_front();
					if (_write_msgs.empty())
					{
						Read();
					}
					else
					{
						#if _DEBUG
						std::cout << "[DEBUG] Write is not empty " << "(" << __FUNCTION__ << " : " << __LINE__ << ")" << std::endl;
						#endif
					}

					

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
	
private:

	size_t readSize;
	boost::asio::streambuf _read_buffer;
	std::deque<std::string> _write_msgs;

	boost::asio::io_context _ioCtx;
	boost::asio::ip::tcp::resolver _resolver;
	boost::asio::ip::tcp::socket _socket;

	std::thread _worker;
	std::shared_ptr<boost::asio::io_service::work> _work;

	std::mutex _Mutex;

};

int main()
{

	ClientSession client("localhost","8090");

	//client.do_SESSION_INIT();
	Sleep(5000);

	client.do_SESSION_IDENTIFIED();
	Sleep(5000);

	client.do_SESSION_CREATE();
	Sleep(5000);

	client.do_SESSION_START();
	Sleep(5000);

	client.do_SESSION_STOP();
	Sleep(5000);

	client.do_SESSION_RESET();
	Sleep(5000);

	client.do_SESSION_DELETE();
	Sleep(5000);

	while (true)
	{

	}


	return 0;
}