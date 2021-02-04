#ifndef INJECTOR_CLIENT_H
#define INJECTOR_CLIENT_H

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>
#include <thread>

#include "MyInjector.grpc.pb.h"
#include "MyCursorStruct.h"

using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;
using hid::MouseRequest;
using hid::MouseResponse;
using hid::KeyboardRequest;
using hid::KeyboardResponse;
using hid::ClipboardRequest;
using hid::ClipboardResponse;
using hid::Injection;

enum MouseProceedType
{
    MOUSE_MOTION = 0,
    MOUSE_DOWN = 1,
    MOUSE_UP = 2,
    MOUSE_WHEEL = 3,
};

enum MouseButtonType
{
    LEFT = 1,
    MIDDLE = 2,
    RIGHT = 3,
    NONE = 4
};

enum KeyboardProceedType
{
    KEYBOARD_DOWN = 0,
    KEYBOARD_UP = 1
};

class InjectClient {
public:
    explicit InjectClient(std::shared_ptr<Channel> channel)
        : stub_(Injection::NewStub(channel)) {}

    void PushKeyboard(uint32_t kbCode, uint32_t extend);
    void PushMouse(MouseProceedType proceedType, MouseButtonType btType, int x, int y);
    void PushClipboard(const char* clipboardData);

    void RegisterCreateCursorCB(std::function<void(CursorData cb)> createcb);
    void RegisterDestroyCursorCB(std::function<void()> destroycb);

private:
    std::function<void(CursorData)> createColorcursorCB;
    std::function<void()> destroyColorcursorCB;
    
    // Out of the passed in Channel comes the stub, stored here, our view of the
    // server's exposed services.
    std::unique_ptr<Injection::Stub> stub_;
};

#endif