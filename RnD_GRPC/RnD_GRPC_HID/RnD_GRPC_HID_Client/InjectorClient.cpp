#include "InjectorClient.h"

void InjectClient::PushKeyboard(uint32_t kbCode, uint32_t extend)
{
    // Data we are sending to the server.
    KeyboardRequest request;

    request.set_keycode(kbCode);
    request.set_extend(extend);

    // Container for the data we expect from the server.
    KeyboardResponse reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The producer-consumer queue we use to communicate asynchronously with the
    // gRPC runtime.
    CompletionQueue cq;

    // Storage for the status of the RPC upon completion.
    Status status;

    // stub_->PrepareAsyncSayHello() creates an RPC object, returning
    // an instance to store in "call" but does not actually start the RPC
    // Because we are using the asynchronous API, we need to hold on to
    // the "call" instance in order to get updates on the ongoing RPC.
    std::unique_ptr<ClientAsyncResponseReader<KeyboardResponse> > rpc(stub_->PrepareAsyncPushKeyboardInject(&context, request, &cq));

    // StartCall initiates the RPC call
    rpc->StartCall();

    // Request that, upon completion of the RPC, "reply" be updated with the
    // server's response; "status" with the indication of whether the operation
    // was successful. Tag the request with the integer 1.
    rpc->Finish(&reply, &status, (void*)2);
    void* got_tag;
    bool ok = false;
    // Block until the next result is available in the completion queue "cq".
    // The return value of Next should always be checked. This return value
    // tells us whether there is any kind of event or the cq_ is shutting down.
    GPR_ASSERT(cq.Next(&got_tag, &ok));

    // Verify that the result from "cq" corresponds, by its tag, our previous
    // request.
    GPR_ASSERT(got_tag == (void*)2);
    // ... and that the request was completed successfully. Note that "ok"
    // corresponds solely to the request for updates introduced by Finish().
    GPR_ASSERT(ok);

    // Act upon the status of the actual RPC.
    if (status.ok()) {
        std::cout << "Keybaord RPC OK" << std::endl;
    }
    else {
        std::cout << "Keyboard RPC Failed" << std::endl;
    }
}

void InjectClient::PushMouse(MouseProceedType proceedType, MouseButtonType btType, int x, int y)
{
    // Data we are sending to the server.
    MouseRequest request;

    // Set Mouse proceed type
    switch (proceedType)
    {
    case MOUSE_MOTION:
        request.set_proceedtype(hid::MouseRequest_ProceedType::MouseRequest_ProceedType_MOTION); break;
    case MOUSE_DOWN:
        request.set_proceedtype(hid::MouseRequest_ProceedType::MouseRequest_ProceedType_DOWN);break;
    case MOUSE_UP:
        request.set_proceedtype(hid::MouseRequest_ProceedType::MouseRequest_ProceedType_UP);break;
    case MOUSE_WHEEL:
        request.set_proceedtype(hid::MouseRequest_ProceedType::MouseRequest_ProceedType_Wheel);break;
    default:
        break;
    }
  
    // Set Mouse button type
    switch (btType)
    {
    case LEFT:
        request.set_buttontype(hid::MouseRequest_ButtonType::MouseRequest_ButtonType_LEFT);break;
    case MIDDLE:
        request.set_buttontype(hid::MouseRequest_ButtonType::MouseRequest_ButtonType_MIDDLE); break;
    case RIGHT:
        request.set_buttontype(hid::MouseRequest_ButtonType::MouseRequest_ButtonType_RIGHT);break;
    case NONE:
        request.set_buttontype(hid::MouseRequest_ButtonType::MouseRequest_ButtonType_NONE);break;
    default:
        break;
    }

    //std::cout << "X : " << x << " Y : " << y << std::endl;

    int64_t location = 0;
    location = int64_t(x) << 32;
    location |= y;

    request.set_location(location);

    // Container for the data we expect from the server.
    MouseResponse reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The producer-consumer queue we use to communicate asynchronously with the
    // gRPC runtime.
    CompletionQueue cq;

    // Storage for the status of the RPC upon completion.
    Status status;

    // stub_->PrepareAsyncSayHello() creates an RPC object, returning
    // an instance to store in "call" but does not actually start the RPC
    // Because we are using the asynchronous API, we need to hold on to
    // the "call" instance in order to get updates on the ongoing RPC.
    std::unique_ptr<ClientAsyncResponseReader<MouseResponse> > rpc( stub_->PrepareAsyncPushMouseInject(&context, request, &cq) );

    // StartCall initiates the RPC call
    rpc->StartCall();

    // Request that, upon completion of the RPC, "reply" be updated with the
    // server's response; "status" with the indication of whether the operation
    // was successful. Tag the request with the integer 1.
    rpc->Finish(&reply, &status, (void*)1);
    void* got_tag;
    bool ok = false;
    // Block until the next result is available in the completion queue "cq".
    // The return value of Next should always be checked. This return value
    // tells us whether there is any kind of event or the cq_ is shutting down.
    GPR_ASSERT(cq.Next(&got_tag, &ok));

    // Verify that the result from "cq" corresponds, by its tag, our previous
    // request.
    GPR_ASSERT(got_tag == (void*)1);
    // ... and that the request was completed successfully. Note that "ok"
    // corresponds solely to the request for updates introduced by Finish().
    GPR_ASSERT(ok);

    // Act upon the status of the actual RPC.
    if (status.ok()) {
        std::cout << "Mouse RPC OK" << std::endl;
        if (reply.cursorsize() == 0)
        {
            //std::cout << "This is same cursor" << std::endl;
        }
        else
        {
            cursor_data
            destroyColorcursorCB();

            //Mouse Cursor Change
            createColorcursorCB(CursorData{ reply.width() , reply.height() ,reply.xhotspot() ,reply.yhotspot() ,reply.cursorsize() ,reply.data() });
        }
    }
    else {
        std::cout << "Mouse RPC Failed" << std::endl;
    }
}

void InjectClient::PushClipboard(const char* clipboardData)
{
    // Data we are sending to the server.
    ClipboardRequest request;

    request.set_clipboarddata(clipboardData);

    ClipboardResponse reply;
    ClientContext context;
    CompletionQueue cq;

    Status status;

    std::unique_ptr<ClientAsyncResponseReader<ClipboardResponse> > rpc(stub_->PrepareAsyncPushClipboardInject(&context, request, &cq));

    rpc->StartCall();
    rpc->Finish(&reply, &status, (void*)3);

    void* got_tag;
    bool ok = false;

    GPR_ASSERT(cq.Next(&got_tag, &ok));
    GPR_ASSERT(got_tag == (void*)3);
    GPR_ASSERT(ok);

    if (status.ok()) {
        std::cout << "Mouse RPC OK" << std::endl;
    }
    else {
        std::cout << "Mouse RPC Failed" << std::endl;
    }
}

void InjectClient::RegisterCreateCursorCB(std::function<void(CursorData cb)> createcb)
{
    createColorcursorCB = createcb;
}

void InjectClient::RegisterDestroyCursorCB(std::function<void()> destroycb)
{
    destroyColorcursorCB = destroycb;
}
