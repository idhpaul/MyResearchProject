#ifndef INJECTOR_CLIENT_H
#define INJECTOR_CLIENT_H

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>
#include <thread>

#include "MyInjector.grpc.pb.h"


using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;
using hid::MouseRequest;
using hid::MouseResponse;
using hid::KeyboardRequest;
using hid::KeyboardResponse;
using hid::Injection;

enum MouseProceedType
{
    MOTION = 0,
    DOWN = 1,
    UP = 2,
    Wheel = 3,
};

enum MouseButtonType
{
    LEFT = 1,
    MIDDLE = 2,
    RIGHT = 3,
    NONE = 4
};

class InjectClient {
public:
    explicit InjectClient(std::shared_ptr<Channel> channel)
        : stub_(Injection::NewStub(channel)) {}

    void PushKeyboard(uint32_t kbCode, uint32_t extend);
    void PushMouse(MouseProceedType proceedType, MouseButtonType btType, int x, int y);
    void PushClipboard();
    
    // Out of the passed in Channel comes the stub, stored here, our view of the
    // server's exposed services.
    std::unique_ptr<Injection::Stub> stub_;
};

#endif