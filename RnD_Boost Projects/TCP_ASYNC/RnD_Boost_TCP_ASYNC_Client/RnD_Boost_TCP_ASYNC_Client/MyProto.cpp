#include "MyProto.h"

void StartProto()
{
	// Verify that the version of the library that we linked against is
	// compatible with the version of the headers we compiled against.
	GOOGLE_PROTOBUF_VERIFY_VERSION;
}

void StopProto()
{
	// Optional:  Delete all global objects allocated by libprotobuf.
	google::protobuf::ShutdownProtobufLibrary();
}

void InitProtoData(My_Net::Session& data)
{
	std::cout << "Enter Host: ";
	std::string host;
	std::cin >> host;
	data.set_host(host);
	std::cin.ignore(256, '\n');

	std::cout << "Enter User-Agent( ex: IDH_MAC/WindowsVersion ): ";
	std::string user_agent;
	std::cin >> user_agent;
	data.set_user_agent(user_agent);
	std::cin.ignore(256, '\n');

	std::cout << "Enter Content_type( ex: IDH_SESSION_START ): ";
	std::string content_type;
	std::cin >> content_type;
	data.set_content_type(content_type);
	std::cin.ignore(256, '\n');

	// Default UTC time + 9 hour(3600*9) is Seoul Time
	*data.mutable_date() = google::protobuf::util::TimeUtil::SecondsToTimestamp(time(NULL) + (3600 * 9));
	std::cout << "Set Time : " << data.date() << std::endl;

	int bodyLength = 0;

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

	data.set_content_length(bodyLength);
}
