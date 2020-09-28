#pragma once
#ifndef MYPROTO_H
#define MYPROTO_H

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

void StartProto();
void StopProto();

void InitProtoData(My_Net::Session& data);

void MakeMessageInit(std::string& outStr, int* strlength);
void MakeMessageIdentified(std::string& outStr, int* strlength);
void MakeMessageCreate(std::string& outStr, int* strlength);
void MakeMessageDelete(std::string& outStr, int* strlength);
void MakeMessageStart(std::string& outStr, int* strlength);
void MakeMessageStop(std::string& outStr, int* strlength);
void MakeMessageReset(std::string& outStr, int* strlength);

#endif // !MYPROTO_H
