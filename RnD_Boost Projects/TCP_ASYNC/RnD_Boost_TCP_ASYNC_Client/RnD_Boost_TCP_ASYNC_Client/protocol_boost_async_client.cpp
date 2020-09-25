#include <boost/asio.hpp>

#include "MyProto.h"

#include <cstring>
#include <iostream>
#include <memory>
#include <mutex>
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
		std::lock_guard<std::mutex> lock_guard(_Mutex);

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

		/// <Body>
		_JsonOutput.clear();
		My_Net::SessionMessageInit myMessageInit;
		myMessageInit.set_sessionkey("EXAMPLECRYPTOKEYVALUE");

		google::protobuf::util::MessageToJsonString(myMessageInit, &_JsonOutput);
		/// </Body>

		mySession.set_content_length(_JsonOutput.size());
		mySession.set_body(_JsonOutput);

		std::string SerializedStringMessage;
		SerializedStringMessage = mySession.SerializeAsString() + "\r\n";

		CopyBuffer(SerializedStringMessage, SerializedStringMessage.length());

		//StopProto();

		Write();

	};
	void do_SESSION_IDENTIFIED()
	{

#if _DEBUG
		std::cout << "[DEBUG] do_SESSION_IDENTIFIED : " << "(" << __FUNCTION__ << " : " << __LINE__ << ")" << std::endl;
#endif
		std::lock_guard<std::mutex> lock_guard(_Mutex);

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

		/// <Body>

		_JsonOutput.clear();
		My_Net::SessionMessageIdentified myMessageIdentified;
		myMessageIdentified.set_system_info_one("Intel(R) Core(TM) i7-7600U CPU @ 2.80GHz");
		myMessageIdentified.set_system_info_two("1920x1080");
		myMessageIdentified.set_system_info_three("8g");
		myMessageIdentified.set_system_info_four("Intel(R) Dual Band Wireless-AC 8265 #2");
		myMessageIdentified.set_bandwidth("123456"); // Timestamp

		google::protobuf::util::MessageToJsonString(myMessageIdentified, &_JsonOutput);


		/// </Body>

		mySession.set_content_length(_JsonOutput.size());
		mySession.set_body(_JsonOutput);

		std::string SerializedStringMessage;
		SerializedStringMessage = mySession.SerializeAsString() + "\r\n";

		CopyBuffer(SerializedStringMessage, SerializedStringMessage.length());

		//StopProto();

		Write();
	};
	void do_SESSION_CREATE()
	{

#if _DEBUG
		std::cout << "[DEBUG] do_SESSION_CREATE : " << "(" << __FUNCTION__ << " : " << __LINE__ << ")" << std::endl;
#endif
		std::lock_guard<std::mutex> lock_guard(_Mutex);

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

		/// <Body>
		_JsonOutput.clear();
		My_Net::SessionMessageCreate myMessageCreate;
		/*myMessageCreate.set_settinginfo1("H264");
		myMessageCreate.set_settinginfo2("VBR");
		myMessageCreate.set_settinginfo3("10000");
		myMessageCreate.set_settinginfo4("30");
		myMessageCreate.set_settinginfo5("48000");
		myMessageCreate.set_settinginfo6("24");*/

		google::protobuf::util::MessageToJsonString(myMessageCreate, &_JsonOutput);
		/// </Body>

		mySession.set_content_length(_JsonOutput.size());
		mySession.set_body(_JsonOutput);

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
		std::lock_guard<std::mutex> lock_guard(_Mutex);

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

		/// <Body>
		_JsonOutput.clear();
		My_Net::SessionMessageDelete myMessageDelete;
		myMessageDelete.set_usage_time("Usage time(hour:min) : 3:56");

		google::protobuf::util::MessageToJsonString(myMessageDelete, &_JsonOutput);
		/// </Body>

		mySession.set_content_length(_JsonOutput.size());
		mySession.set_body(_JsonOutput);

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
		std::lock_guard<std::mutex> lock_guard(_Mutex);

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

		/// <Body>
		_JsonOutput.clear();
		My_Net::SessionMessageStart myMessageStart;
		myMessageStart.set_lastsate("TEST-STATE");

		google::protobuf::util::MessageToJsonString(myMessageStart, &_JsonOutput);
		/// </Body>

		mySession.set_content_length(_JsonOutput.size());
		mySession.set_body(_JsonOutput);

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
		std::lock_guard<std::mutex> lock_guard(_Mutex);

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

		/// <Body>
		_JsonOutput.clear();
		My_Net::SessionMessageStop myMessageStop;
		myMessageStop.set_lastsate("TEST-STATE");

		google::protobuf::util::MessageToJsonString(myMessageStop, &_JsonOutput);
		/// </Body>

		mySession.set_content_length(_JsonOutput.size());
		mySession.set_body(_JsonOutput);

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
		std::lock_guard<std::mutex> lock_guard(_Mutex);

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

		/// <Body>
		_JsonOutput.clear();
		My_Net::SessionMessageReset myMessageReset;
		myMessageReset.set_settinginfo1("H264");
		myMessageReset.set_settinginfo2("VBR");
		myMessageReset.set_settinginfo3("10000");
		myMessageReset.set_settinginfo4("30");
		myMessageReset.set_settinginfo5("48000");
		myMessageReset.set_settinginfo6("24");

		google::protobuf::util::MessageToJsonString(myMessageReset, &_JsonOutput);
		/// </Body>

		mySession.set_content_length(_JsonOutput.size());
		mySession.set_body(_JsonOutput);

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

public:
	std::string _JsonOutput;

private:
	// TODO: 동적 버퍼
	char _buffer[100];
	int _bufferLength;


	boost::asio::io_context _ioCtx;
	boost::asio::ip::tcp::resolver _resolver;
	boost::asio::ip::tcp::socket _socket;

	std::thread _worker;
	std::shared_ptr<boost::asio::io_service::work> _work;

	std::mutex _Mutex;

};

int main()
{


	std::string JsonOutput;
	My_Net::SessionMessageIdentified myMessageIdentified;
	myMessageIdentified.set_system_info_one("Intel(R) Core(TM) i7-7600U CPU @ 2.80GHz");
	myMessageIdentified.set_system_info_two("1920x1080");
	myMessageIdentified.set_system_info_three("8g");
	myMessageIdentified.set_system_info_four("Intel(R) Dual Band Wireless-AC 8265 #2");
	myMessageIdentified.set_bandwidth("123456"); // Timestamp

	//google::protobuf::util::JsonPrintOptions options;
	//options.add_whitespace = true;
	//options.always_print_primitive_fields = false;
	google::protobuf::util::MessageToJsonString(myMessageIdentified, &JsonOutput/*, options*/);
	std::cout << JsonOutput << std::endl;

	ClientSession client("localhost","8090");

	//client.do_SESSION_INIT();
	Sleep(5000);

	client.do_SESSION_DELETE();
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

	
	while (true)
	{

	}


	return 0;
}