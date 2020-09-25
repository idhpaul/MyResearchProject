/*******************************************************************************
* Created  		Sep 21, 2020
* Modified  		Sep 21, 2020
* FILE:			main.cpp
* Author:		IDH
* DESCRIPTION:	Test proto wirte
*
*******************************************************************************/

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

#include <iostream>
#include <string>


int main()
{
	// Verify that the version of the library that we linked against is
	// compatible with the version of the headers we compiled against.
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	My_Net::Session mySession;

	std::cout << "Enter Host(Only MSLM): ";
	std::string host;
	std::cin >> host;
	mySession.set_host(host);
	std::cin.ignore(256, '\n');

	std::cout << "Enter User-Agent( ex: RX_MAC/WindowsVersion ): ";
	std::string user_agent;
	std::cin >> user_agent;
	mySession.set_user_agent(user_agent);
	std::cin.ignore(256, '\n');

	std::cout << "Enter Content_type( ex: MSLM_SESSION_START ): ";
	std::string content_type;
	std::cin >> content_type;
	mySession.set_content_type(content_type);
	std::cin.ignore(256, '\n');

	// Default UTC time + 9 hour(3600*9) is Seoul Time
	*mySession.mutable_date() = google::protobuf::util::TimeUtil::SecondsToTimestamp(time(NULL) + (3600 * 9));
	std::cout << "Set Time : " << mySession.date() << std::endl;

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

	mySession.set_content_length(bodyLength);

	std::string SerializedStringMessage;
	std::string JsonOutput;

	google::protobuf::util::JsonPrintOptions options;
	options.add_whitespace = true;
	options.always_print_primitive_fields = false;
	google::protobuf::util::MessageToJsonString(mySession, &JsonOutput);

	std::cout << JsonOutput << std::endl;


	SerializedStringMessage = mySession.SerializeAsString();

	std::cout << SerializedStringMessage << std::endl;
	std::cout << "Size of Serialized : " << SerializedStringMessage.size() << std::endl;
	std::cout << "length of Serialized : " << SerializedStringMessage.length() << std::endl;


	My_Net::Session mySessionRead;
	mySessionRead.ParseFromString(SerializedStringMessage);

	std::cout << mySessionRead.host() << std::endl;
	std::cout << mySessionRead.user_agent() << std::endl;
	std::cout << mySessionRead.content_type() << std::endl;
	std::cout << mySessionRead.date() << std::endl;
	std::cout << mySessionRead.content_length() << std::endl;


	// Optional:  Delete all global objects allocated by libprotobuf.
	google::protobuf::ShutdownProtobufLibrary();

	return 0;
}