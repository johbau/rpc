#include "rpc.hpp"
#include <flatbuffers/flatbuffers.h>
#include <flatbuffers/flatbuffer_builder.h>
#include <flatbuffers/flatbuffer.h>
#include <string>
#include <vector>

using namespace rpc;

int main() {
    // Create an RPC client
    RpcClient client("example_server");
    
    // Create a FlatBuffer request
    flatbuffers::FlatBufferBuilder fbb;
    
    // Create a HelloRequest with GREETING type and message
    HelloRequest::RequestType request_type = HelloRequest::GREETING;
    std::string message = "Hello, world!";
    auto request = fbb.CreateHelloRequest(request_type, message);
    
    // Send the request and get the response
    auto response = client.sendRequest<flatbuffers::FlatBufferBuilder, flatbuffers::FlatBufferBuilder>(fbb);
    
    // Verify the response
    auto helloResponse = HelloResponse::GetRootAsHelloResponse(response->data());
    auto result = helloResponse->result();
    
    // Print the response
    std::cout << "Received response with " << result->size() << " strings." << std::endl;
    
    return 0;
}
