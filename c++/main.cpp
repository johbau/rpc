#include "rpc.hpp"
#include "hello_generated.h"
#include "flatbuffers/flatbuffers.h"
#include <string>
#include <vector>
#include <iostream>

using namespace rpc;

int main() {
    // Create an RPC client
    const std::string service = "hello";
    std::cout << "Create RpcClient" << std::endl;
    RpcClient client(service);
    
    // Create a FlatBuffer request
    flatbuffers::FlatBufferBuilder fbb;
    
    // Create a HelloRequest with GREETING type and message
    Hello::RequestType request_type = Hello::RequestType_GREETING;
    auto message = fbb.CreateString("Hello, world!");
    auto request = CreateHelloRequest(fbb, request_type, message);
    fbb.Finish(request);

    // Send the request and get the response
    const std::size_t size = 1024;
    unsigned char response[size] = {0};
    client.sendRequest(fbb.GetBufferPointer(), fbb.GetSize(), response, size);

    // Verify the response
    auto helloResponse = Hello::GetHelloResponse(response);
    auto result = helloResponse->result();

    // Print the response
    std::cout << "Received response with " << result->size() << " strings." << std::endl;
    
    return 0;
}
