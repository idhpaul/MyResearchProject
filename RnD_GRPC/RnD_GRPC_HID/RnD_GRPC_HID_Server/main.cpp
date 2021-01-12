#include <memory>
#include <iostream>
#include <string>
#include <thread>

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>
#include "MyInjector.grpc.pb.h"

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerCompletionQueue;
using grpc::Status;

using hid::MouseRequest;
using hid::MouseResponse;
using hid::KeyboardRequest;
using hid::KeyboardResponse;
using hid::Injection;
class ServerImpl final {
public:
	~ServerImpl() {
		server_->Shutdown();
		// Always shutdown the completion queue after the server.
		cq_->Shutdown();
	}

	// There is no shutdown handling in this code.
	void Run(const std::string& ip, const std::string& port) {
		std::string server_address = std::string(ip + ":" + port);

		ServerBuilder builder;
		// Listen on the given address without any authentication mechanism.
		builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
		// Register "service_" as the instance through which we'll communicate with
		// clients. In this case it corresponds to an *asynchronous* service.
		builder.RegisterService(&service_);
		// Get hold of the completion queue used for the asynchronous communication
		// with the gRPC runtime.
		cq_ = builder.AddCompletionQueue();
		// Finally assemble the server.
		server_ = builder.BuildAndStart();
		std::cout << "Server listening " << server_address << std::endl;

		Handle_Rpcs();

	}

private:

	// Class encompasing the state and logic needed to serve a request.
	class CallInjectData {
	public:
		// Take in the "service" instance (in this case representing an asynchronous
		// server) and the completion queue "cq" used for asynchronous communication
		// with the gRPC runtime.
		CallInjectData(Injection::AsyncService* service, ServerCompletionQueue* cq)
			: service_(service), cq_(cq), keyboard_responder_(&ctx_), mouse_responder_(&ctx_), status_(MOUSE_CREATE) {
			// Invoke the serving logic right away.
			Proceed();
		}
		CallInjectData(Injection::AsyncService* service, ServerCompletionQueue* cq, bool flag)
			: service_(service), cq_(cq), keyboard_responder_(&ctx_), mouse_responder_(&ctx_), status_(KEYBOARD_CREATE) {
			// Invoke the serving logic right away.
			Proceed();
		}

		void Proceed() {
			if (status_ == KEYBOARD_CREATE) {
				// Make this instance progress to the PROCESS state.
				status_ = KEYBOARD_PROCESS;

				service_->RequestPushKeyboardInject(&ctx_, &keyboard_request_, &keyboard_responder_, cq_, cq_, this);
			}
			else if (status_ == KEYBOARD_PROCESS) {
				// Spawn a new CallData instance to serve new clients while we process
				// the one for this CallData. The instance will deallocate itself as
				// part of its FINISH state.
				new CallInjectData(service_, cq_, true);

				
				std::string prefix("Push Keyboard Inject : ");

				INPUT in;
				ZeroMemory(&in, sizeof(in));
				//if ("keyboard" == keyboard_request_.type())
				//{
				//	in.type = INPUT_KEYBOARD;
				//	in.ki.wScan = 0;
				//	in.ki.time = 0;
				//	in.ki.dwExtraInfo = 0;

				//	in.ki.wVk = keyboard_request_.keycode();

				//	if (keyboard_request_.is_pressed())
				//	{
				//		//printf("key pressed");
				//		in.ki.dwFlags = 0;

				//		if (keyboard_request_.is_extend())
				//		{
				//			//printf("extend key pressed");
				//			in.ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
				//		}
				//	}
				//	else
				//	{
				//		//printf("key up");
				//		in.ki.dwFlags = KEYEVENTF_KEYUP;

				//		if (keyboard_request_.is_extend())
				//		{
				//			//printf("extend key up");
				//			in.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
				//		}
				//	}
				//}

				////
				//// write data to pipe
				//memcpy(chBuf, (char*)&in, BUFSIZE);

				//fSuccess = WriteFile(
				//	hPipe,                  // pipe handle 
				//	chBuf,             // message 
				//	BUFSIZE,              // message length 
				//	&cbWritten,             // bytes written 
				//	NULL);                  // not overlapped 

				//if (!fSuccess)
				//{
				//	std::cout << "WriteFile to pipe failed, GetLastError : " << GetLastError() << std::endl;

				//	hPipe = INVALID_HANDLE_VALUE;
				//	std::cout << "[TODO]Need exception process" << std::endl;
				//	//return Status::CANCELLED;
				//}

				////


				//keyboard_response_.set_reply(prefix + keyboard_request_.type());


				status_ = KEYBOARD_FINISH;
				keyboard_responder_.Finish(keyboard_response_, Status::OK, this);
			}
			else if (status_ == KEYBOARD_FINISH)
			{
				delete this;

			}
			else if (status_ == MOUSE_CREATE)
			{
				status_ = MOUSE_PROCESS;

				service_->RequestPushMouseInject(&ctx_, &mouse_request_, &mouse_responder_, cq_, cq_, this);
			}
			else if (status_ == MOUSE_PROCESS)
			{
				// Spawn a new CallData instance to serve new clients while we process
				// the one for this CallData. The instance will deallocate itself as
				// part of its FINISH state.
				new CallInjectData(service_, cq_);


				INPUT in;
				ZeroMemory(&in, sizeof(in));
				DEVMODE	mode;
				EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &mode);

				if (mouse_request_.proceedtype() == hid::MouseRequest_ProceedType::MouseRequest_ProceedType_MOTION)
				{
					std::cout << "Motion" << std::endl;
					in.type = INPUT_MOUSE;

					uint64_t location = mouse_request_.location();

					auto y = int32_t(location);
					auto x = int32_t(location >> 32);

					std::cout << "x : " << x << " y : " << y << std::endl;

					in.mi.dx = (x * 65535) / (mode.dmPelsWidth - 1);
					in.mi.dy = (y * 65535) / (mode.dmPelsHeight - 1);
					in.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

					SendInput(1, &in, sizeof(in));
				}
				else if (mouse_request_.proceedtype() == hid::MouseRequest_ProceedType::MouseRequest_ProceedType_DOWN)
				{
					in.type = INPUT_MOUSE;

					if (mouse_request_.buttontype() == hid::MouseRequest_ButtonType::MouseRequest_ButtonType_LEFT)
					{
						in.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
					}
					else if (mouse_request_.buttontype() == hid::MouseRequest_ButtonType::MouseRequest_ButtonType_MIDDLE)
					{
						in.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
					}
					else if (mouse_request_.buttontype() == hid::MouseRequest_ButtonType::MouseRequest_ButtonType_RIGHT)
					{
						in.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
					}

					SendInput(1, &in, sizeof(in));
				}
				else if (mouse_request_.proceedtype() == hid::MouseRequest_ProceedType::MouseRequest_ProceedType_UP)
				{
					in.type = INPUT_MOUSE;

					if (mouse_request_.buttontype() == hid::MouseRequest_ButtonType::MouseRequest_ButtonType_LEFT)
					{
						in.mi.dwFlags = MOUSEEVENTF_LEFTUP;
					}
					else if (mouse_request_.buttontype() == hid::MouseRequest_ButtonType::MouseRequest_ButtonType_MIDDLE)
					{
						in.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
					}
					else if (mouse_request_.buttontype() == hid::MouseRequest_ButtonType::MouseRequest_ButtonType_RIGHT)
					{
						in.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
					}

					SendInput(1, &in, sizeof(in));
				}
				else if (mouse_request_.proceedtype() == hid::MouseRequest_ProceedType::MouseRequest_ProceedType_Wheel)
				{
					in.type = INPUT_MOUSE;
					in.mi.dwFlags = MOUSEEVENTF_WHEEL;

					(mouse_request_.location() >> 32 > 0) ? in.mi.mouseData = +WHEEL_DELTA : in.mi.mouseData = -WHEEL_DELTA;;

					SendInput(1, &in, sizeof(in));
				}

				////Reply mouser cursor data
				//CursorPayload cursorData = mouse_cursor_capture_.CaptureCursor();
				////CursorInfo(&cursorData);

				//if (cursorData.cursorsize == 0)
				//{
				//	mouse_response_.set_cursorsize(0);
				//}
				//else
				//{
				//	mouse_response_.set_width(cursorData.width);
				//	mouse_response_.set_height(cursorData.height);
				//	mouse_response_.set_xhotspot(cursorData.xHotspot);
				//	mouse_response_.set_yhotspot(cursorData.yHotspot);
				//	mouse_response_.set_cursorsize(cursorData.cursorsize);
				//	mouse_response_.set_data(cursorData.cursordata.data(), cursorData.cursordata.size());
				//}
				// And we are done! Let the gRPC runtime know we've finished, using the
				// memory address of this instance as the uniquely identifying tag for
				// the event.
				status_ = MOUSE_FINISH;
				mouse_responder_.Finish(mouse_response_, Status::OK, this);
			}
			else if (status_ == MOUSE_FINISH)
			{
				delete this;

			}
			else {
				//GPR_ASSERT(status_ == FINISH);
				// Once in the FINISH state, deallocate ourselves (CallData).
				delete this;
			}
		}

	private:

		Injection::AsyncService* service_;

		ServerCompletionQueue* cq_;

		ServerContext ctx_;

		// What we get from the client.
		KeyboardRequest keyboard_request_;
		KeyboardResponse keyboard_response_;

		MouseRequest mouse_request_;
		MouseResponse mouse_response_;
		//MouseCursor mouse_cursor_capture_;

		// The means to get back to the client.
		ServerAsyncResponseWriter<KeyboardResponse> keyboard_responder_;
		ServerAsyncResponseWriter<MouseResponse> mouse_responder_;

		// Let's implement a tiny state machine with the following states.
		//enum CallStatus { CREATE, PROCESS, FINISH };

		enum CallStatus { MOUSE_CREATE, MOUSE_PROCESS, MOUSE_FINISH, KEYBOARD_CREATE, KEYBOARD_PROCESS, KEYBOARD_FINISH };

		CallStatus status_;  // The current serving state.
	};

	// This can be run in multiple threads if needed.
	void Handle_Rpcs() {
		// Spawn a new CallData instance to serve new clients.
		new CallInjectData(&service_, cq_.get());
		new CallInjectData(&service_, cq_.get(), true);
		void* tag;  // uniquely identifies a request.
		bool ok;
		while (true) {

			GPR_ASSERT(cq_->Next(&tag, &ok));
			GPR_ASSERT(ok);
			static_cast<CallInjectData*>(tag)->Proceed();

		}
	}

	std::unique_ptr<ServerCompletionQueue> cq_;
	Injection::AsyncService service_;
	std::unique_ptr<Server> server_;
};

int main()
{

	ServerImpl server;
	server.Run(std::string("0.0.0.0"), std::string("8099"));

	return 0;
}