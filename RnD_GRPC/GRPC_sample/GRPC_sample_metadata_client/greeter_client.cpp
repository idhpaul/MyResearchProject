/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/crypto.h>

#ifdef BAZEL_BUILD
#include "examples/protos/helloworld.grpc.pb.h"
#else
#include "helloworld.grpc.pb.h"
#endif

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::Greeter;

void binary_print_data(const unsigned char* data, int len, char* msg)
{
    int i;
    printf("%s:\n", msg);
    for (i = 0; i < len; i++) {
        printf("0x%02x, ", data[i]);
        if (((i + 1) & 0x7) == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

class CustomHeaderClient {
public:
    CustomHeaderClient(std::shared_ptr<Channel> channel)
        : stub_(Greeter::NewStub(channel)) {}

    // Assembles the client's payload, sends it and presents the response back
    // from the server.
    std::string SayHello(const std::string& user) {
        // Data we are sending to the server.
        HelloRequest request;
        request.set_name(user);

        // Container for the data we expect from the server.
        HelloReply reply;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        ClientContext context;

        // Setting custom metadata to be sent to the server
        context.AddMetadata("custom-header", "Custom Value");

        unsigned char* key = (unsigned char*)OPENSSL_malloc(16);
        RAND_bytes(key, 16);
        binary_print_data(key, 16, const_cast<char*>("key"));

        // Setting custom binary metadata 
        char bytes[8] = { '\0', '\1', '\2', '\3', '\4', '\5', '\6', '\7' };

        std::string convertStr = static_cast<std::string>(reinterpret_cast<const char*>(key)); // new style
        context.AddMetadata("custom-bin", std::string(reinterpret_cast<const char*>(key),16));

        // The actual RPC.
        Status status = stub_->SayHello(&context, request, &reply);

        // Act upon its status.
        if (status.ok()) {
            std::cout << "Client received initial metadata from server: " << context.GetServerInitialMetadata().find("custom-server-metadata")->second << std::endl;
            std::cout << "Client received trailing metadata from server: " << context.GetServerTrailingMetadata().find("custom-trailing-metadata")->second << std::endl;
            return reply.message();
        }
        else {
            std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
            return "RPC failed";
        }

        OPENSSL_free(key);
    }

private:
    std::unique_ptr<Greeter::Stub> stub_;
};

int main(int argc, char** argv) {
    // Instantiate the client. It requires a channel, out of which the actual RPCs
    // are created. This channel models a connection to an endpoint (in this case,
    // localhost at port 50051). We indicate that the channel isn't authenticated
    // (use of InsecureChannelCredentials()).
    CustomHeaderClient greeter(grpc::CreateChannel(
        "192.168.0.202:50051", grpc::InsecureChannelCredentials()));
    std::string user("world");
    std::string reply = greeter.SayHello(user);
    std::cout << "Client received message: " << reply << std::endl;
    return 0;
}
