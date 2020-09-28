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
#include <google/protobuf/util/json_util.h>

#include <deque>
#include <iostream>
#include <memory>
#include <mutex>
#include <boost/asio.hpp>

static std::mutex g_Mutex;

constexpr unsigned int HashCode(const char* str)
{
	return str[0] ? static_cast<unsigned int>(str[0]) + 0xEDB8832Full * HashCode(str + 1) : 8603;
}

std::string make_string(boost::asio::streambuf& streambuf)
{
	return { buffers_begin(streambuf.data()),
			buffers_end(streambuf.data()) };
}

static void boostErrorHandler(const char* BeforeFucName, const int BeforeFucLine, const boost::system::error_code& ec)
{
	g_Mutex.lock();
	std::cerr << "{" << std::endl;
	std::cerr << "      Issue Func Name at : " << BeforeFucName << std::endl;
	std::cerr << "            Func Line at : " << BeforeFucLine << std::endl;
	std::cerr << "                 name    :  " <<ec.category().name() << std::endl;
	std::cerr << "                 value   :  " <<ec.value() << std::endl;
	std::cerr << "                 message :  " << ec.message() << std::endl;
	std::cerr << "}" << std::endl;
	g_Mutex.unlock();
}

class Session
{
public:
	Session(boost::asio::ip::tcp::socket socket)
		: _socket(std::move(socket))
	{
		set_SocketOption();

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
			_read_buffer, "\r\n",
			[this](boost::system::error_code ec, std::size_t length)
			{

					if (!ec)
					{
						#if _DEBUG
						std::cout << "[DEBUG] Read Data from Client " << "(" << __FUNCTION__ << " : " << __LINE__ << ")" << std::endl;
						std::cout << "[DEBUG] Read recv : " << length << std::endl;
						#endif


						std::string recvStr = make_string(_read_buffer);
						recvStr = recvStr.substr(0, recvStr.size() - 2);
						_read_buffer.consume(length);


						My_Net::Session mySessionRead;
						mySessionRead.ParseFromString(recvStr);

						std::cout << mySessionRead.host() << std::endl;
						std::cout << mySessionRead.user_agent() << std::endl;
						std::cout << mySessionRead.content_type() << std::endl;
						std::cout << mySessionRead.date() << std::endl;
						std::cout << mySessionRead.content_length() << std::endl;
						std::cout << mySessionRead.message() << std::endl;

						/// <Body>
						/// // REF : https://m.blog.naver.com/PostView.nhn?blogId=devmachine&logNo=220952781191&proxyReferer=https:%2F%2Fwww.google.com%2F
						switch (HashCode(mySessionRead.content_type().c_str()))
						{
						case HashCode("MY_SESSION_INIT"):
							{
								My_Net::SessionMessageInit myMessageInit;
								google::protobuf::util::JsonStringToMessage(mySessionRead.message(), &myMessageInit);

								std::cout << myMessageInit.session_key() << std::endl;
								break;
							}

						case HashCode("MY_SESSION_IDENTIFIED"):
							{
								My_Net::SessionMessageIdentified myMessageIdentified;
								google::protobuf::util::JsonStringToMessage(mySessionRead.message(), &myMessageIdentified);

								std::cout << myMessageIdentified.cpu_info() << std::endl;
								std::cout << myMessageIdentified.res_info() << std::endl;
								std::cout << myMessageIdentified.mem_info() << std::endl;
								std::cout << myMessageIdentified.net_info() << std::endl;
								std::cout << myMessageIdentified.bandwidth() << std::endl;
								break;
							}

						case HashCode("MY_SESSION_CREATE"):
							{
								My_Net::SessionMessageCreate myMessageCreate;
								google::protobuf::util::JsonStringToMessage(mySessionRead.message(), &myMessageCreate);

								std::cout << myMessageCreate.v_codec_info() << std::endl;
								std::cout << myMessageCreate.v_brc_info() << std::endl;
								std::cout << myMessageCreate.v_bitrate_info() << std::endl;
								std::cout << myMessageCreate.v_gop_info() << std::endl;
								std::cout << myMessageCreate.a_samplerate() << std::endl;
								std::cout << myMessageCreate.a_quantization() << std::endl;
								break;
							}

						case HashCode("MY_SESSION_DELETE"):
							{
								My_Net::SessionMessageDelete myMessageDelete;
								google::protobuf::util::JsonStringToMessage(mySessionRead.message(), &myMessageDelete);

								std::cout << myMessageDelete.usage_time() << std::endl;
								break;
							}

						case HashCode("MY_SESSION_START"):
							{
								My_Net::SessionMessageStart myMessageStart;
								google::protobuf::util::JsonStringToMessage(mySessionRead.message(), &myMessageStart);

								std::cout << myMessageStart.last_sate() << std::endl;
								break;
							}

						case HashCode("MY_SESSION_STOP"):
							{
								My_Net::SessionMessageStop myMessageStop;
								google::protobuf::util::JsonStringToMessage(mySessionRead.message(), &myMessageStop);

								std::cout << myMessageStop.last_sate() << std::endl;
								break;
							}
						case HashCode("MY_SESSION_RESET"):
							{
								My_Net::SessionMessageReset myMessageReset;
								google::protobuf::util::JsonStringToMessage(mySessionRead.message(), &myMessageReset);

								std::cout << myMessageReset.reset_v_codec_info() << std::endl;
								std::cout << myMessageReset.reset_v_brc_info() << std::endl;
								std::cout << myMessageReset.reset_v_bitrate_info() << std::endl;
								std::cout << myMessageReset.reset_v_gop_info() << std::endl;
								std::cout << myMessageReset.reset_a_samplerate() << std::endl;
								std::cout << myMessageReset.reset_a_quantization() << std::endl;
								break;
							}

						default:
							std::cerr << "Message type is out of range" << std::endl;
							break;
						}
						// </Body>


						std::string writeMessage = "resp for " + mySessionRead.content_type() + "\r\n";
						_write_msgs.push_back(writeMessage);

						Write();
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

private:

	void set_SocketOption()
	{
		boost::asio::socket_base::keep_alive keepAliveOption(true);
		_socket.set_option(keepAliveOption);

		boost::asio::socket_base::linger ligerOption(true, 30);
		_socket.set_option(ligerOption);
	};

	boost::asio::ip::tcp::socket _socket;


	boost::asio::streambuf _read_buffer;
	std::deque<std::string> _write_msgs;

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
		std::cout << "[DEBUG]  Destructor call" << "(" << __FUNCTION__ << " : " << __LINE__ << ")" << std::endl;
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
					std::cout << "Local Endpoint: " << socket.local_endpoint().address().to_string() << ", " << socket.local_endpoint().port() << std::endl;
					std::cout << "Remote Endpoint: " << socket.remote_endpoint().address().to_string() << ", " << socket.remote_endpoint().port() << std::endl;

					_mySession = std::make_unique<Session>(std::move(socket));
				}
				else
				{
					boostErrorHandler(__FUNCTION__, __LINE__, ec);
				}

				do_accept();
			});
	}
	std::thread _worker;
	std::unique_ptr<Session> _mySession;

	boost::asio::io_context _ioCtx;
	std::shared_ptr<boost::asio::io_service::work> _work; // Control io_context

	boost::asio::ip::tcp::acceptor	_acceptor;
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