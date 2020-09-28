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
	std::string host = "IDH";
	//std::cin >> host;
	//std::cin.ignore(256, '\n');
	data.set_host(host);

	std::cout << "Enter User-Agent( ex: IDH_MAC/WindowsVersion ): ";
	std::string user_agent = "IDH_PC";
	//std::cin >> user_agent;
	//std::cin.ignore(256, '\n');
	data.set_user_agent(user_agent);

	std::cout << "Enter Content_type( ex: IDH_SESSION_START ): ";
	std::string content_type = "HELLO_MESSAGE";
	//std::cin >> content_type;
	//std::cin.ignore(256, '\n');
	data.set_content_type(content_type);

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


// TODO: 프로토콜버퍼 2개를 연속으로 사용했을 때 string 메모리 접근 오류가 발생하여 외부로 빼냈음
//		원인은 못 찾음 _ 2020.09.28 IDH
void MakeMessageInit(std::string& outStr, int* strlength)
{
	My_Net::SessionMessageInit myMessageInit;
	myMessageInit.set_session_key("EXAMPLECRYPTOKEYVALUE");

	google::protobuf::util::JsonPrintOptions options;
	options.add_whitespace = true;
	options.always_print_primitive_fields = true;
	options.preserve_proto_field_names = true;
	google::protobuf::util::MessageToJsonString(myMessageInit, &outStr, options);

	*strlength = outStr.length();
}

// TODO: 프로토콜버퍼 2개를 연속으로 사용했을 때 string 메모리 접근 오류가 발생하여 외부로 빼냈음
//		원인은 못 찾음 _ 2020.09.28 IDH
void MakeMessageIdentified(std::string& outStr, int* strlength)
{
	My_Net::SessionMessageIdentified myMessageIdentified;
	myMessageIdentified.set_cpu_info("Intel(R) Core(TM) i7-7600U CPU @ 2.80GHz");
	myMessageIdentified.set_res_info("1920x1080");
	myMessageIdentified.set_mem_info("8g");
	myMessageIdentified.set_net_info("Intel(R) Dual Band Wireless-AC 8265 #2");
	myMessageIdentified.set_bandwidth("123456"); // Timestamp

	google::protobuf::util::JsonPrintOptions options;
	options.add_whitespace = true;
	options.always_print_primitive_fields = true;
	options.preserve_proto_field_names = true;
	google::protobuf::util::MessageToJsonString(myMessageIdentified, &outStr, options);

	*strlength = outStr.length();
}

// TODO: 프로토콜버퍼 2개를 연속으로 사용했을 때 string 메모리 접근 오류가 발생하여 외부로 빼냈음
//		원인은 못 찾음 _ 2020.09.28 IDH
void MakeMessageCreate(std::string& outStr, int* strlength)
{
	My_Net::SessionMessageCreate myMessageCreate;
	myMessageCreate.set_v_codec_info("H264");
	myMessageCreate.set_v_brc_info("VBR");
	myMessageCreate.set_v_bitrate_info("10000");
	myMessageCreate.set_v_gop_info("30");
	myMessageCreate.set_a_samplerate("48000");
	myMessageCreate.set_a_quantization("24");

	google::protobuf::util::JsonPrintOptions options;
	options.add_whitespace = true;
	options.always_print_primitive_fields = true;
	options.preserve_proto_field_names = true;
	google::protobuf::util::MessageToJsonString(myMessageCreate, &outStr, options);

	*strlength = outStr.length();
}

// TODO: 프로토콜버퍼 2개를 연속으로 사용했을 때 string 메모리 접근 오류가 발생하여 외부로 빼냈음
//		원인은 못 찾음 _ 2020.09.28 IDH
void MakeMessageDelete(std::string& outStr, int* strlength)
{
	My_Net::SessionMessageDelete myMessageDelete;
	myMessageDelete.set_usage_time("Usage time(hour:min) : 3:56");

	google::protobuf::util::JsonPrintOptions options;
	options.add_whitespace = true;
	options.always_print_primitive_fields = true;
	options.preserve_proto_field_names = true;
	google::protobuf::util::MessageToJsonString(myMessageDelete, &outStr, options);

	*strlength = outStr.length();
}

void MakeMessageStart(std::string& outStr, int* strlength)
{
	My_Net::SessionMessageStart myMessageStart;
	myMessageStart.set_last_sate("TEST-STATE");

	google::protobuf::util::JsonPrintOptions options;
	options.add_whitespace = true;
	options.always_print_primitive_fields = true;
	options.preserve_proto_field_names = true;
	google::protobuf::util::MessageToJsonString(myMessageStart, &outStr, options);

	*strlength = outStr.length();
}

// TODO: 프로토콜버퍼 2개를 연속으로 사용했을 때 string 메모리 접근 오류가 발생하여 외부로 빼냈음
//		원인은 못 찾음 _ 2020.09.28 IDH
void MakeMessageStop(std::string& outStr, int* strlength)
{
	My_Net::SessionMessageStop myMessageStop;
	myMessageStop.set_last_sate("TEST-STATE");

	google::protobuf::util::JsonPrintOptions options;
	options.add_whitespace = true;
	options.always_print_primitive_fields = true;
	options.preserve_proto_field_names = true;
	google::protobuf::util::MessageToJsonString(myMessageStop, &outStr, options);

	*strlength = outStr.length();
}

// TODO: 프로토콜버퍼 2개를 연속으로 사용했을 때 string 메모리 접근 오류가 발생하여 외부로 빼냈음
//		원인은 못 찾음 _ 2020.09.28 IDH
void MakeMessageReset(std::string& outStr, int* strlength)
{
	My_Net::SessionMessageReset myMessageReset;
	myMessageReset.set_reset_v_codec_info("H264");
	myMessageReset.set_reset_v_brc_info("VBR");
	myMessageReset.set_reset_v_bitrate_info("10000");
	myMessageReset.set_reset_v_gop_info("30");
	myMessageReset.set_reset_a_samplerate("48000");
	myMessageReset.set_reset_a_quantization("24");

	google::protobuf::util::JsonPrintOptions options;
	options.add_whitespace = true;
	options.always_print_primitive_fields = true;
	options.preserve_proto_field_names = true;

	google::protobuf::util::MessageToJsonString(myMessageReset, &outStr, options);

	*strlength = outStr.length();
}
