#include "rpc.hpp"
#include "../hello_generated.h"
#include "flatbuffers/flatbuffers.h"
#include <string>
#include <vector>
#include <iostream>

using namespace rpc;
using namespace Hello;
using namespace flatbuffers;

int main() {
    // Create an RPC client
    const std::string service = "hello";
    std::cout << "Create RpcClient" << std::endl;
    RpcClient client(service);
    
    // Create a FlatBuffer request
    std::cout << "Create flatbuffers builder" << std::endl;
    FlatBufferBuilder fbb;

    // Create a HelloRequest with GREETING type and message
    std::cout << "Create request" << std::endl;
    RequestType request_type = RequestType_GREETING;
    auto message = fbb.CreateString("Hello, world!");
    auto request = CreateHelloRequest(fbb, request_type, message);
    fbb.Finish(request);

    // Send the request and get the response
    std::cout << "Create make RPC call" << std::endl;
    const std::size_t size = MAX_SIZE;
    unsigned char response[size] = {0};
    try {
    	client.sendRequest(fbb.GetBufferPointer(), fbb.GetSize(), response, size);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    // Verify the response
    std::cout << "Verify response" << std::endl;
    auto verifier = Verifier(response, size);
    if (VerifyHelloResponseBuffer(verifier)) {
        auto helloResponse = GetHelloResponse(response);
        auto result = helloResponse->result();
        std::cout << "Received response with " << result->size() << " strings." << std::endl;
    } else {
        std::cout << "No valid response received." << std::endl;
    }
    return 0;
}
