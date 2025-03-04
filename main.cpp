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
    
    // Since our HelloRequest doesn't have any fields, we can just create an empty buffer
    // In a real scenario, you would add data to the request here
    auto request = fbb.CreateString("HelloRequest");
    
    // Send the request and get the response
    auto response = client.sendRequest<flatbuffers::FlatBufferBuilder, flatbuffers::FlatBufferBuilder>(fbb);
    
    // Verify the response
    auto helloResponse = HelloResponse::GetRootAsHelloResponse(response->data());
    auto result = helloResponse->result();
    
    // Print the response
    std::cout << "Received response with " << result->size() << " strings." << std::endl;
    
    return 0;
}
