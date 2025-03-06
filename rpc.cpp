#define _GLIBCXX_USE_CXX11_ABI 0
#include "rpc.hpp"
#include <boost/interprocess/sync/stream_open_or_create.hpp>
#include <boost/interprocess/mapped_regions/map_reg.hpp>
#include <boost/interprocess/streams/bufferstream.hpp>
#include <flatbuffers/util.h>

using namespace rpc;

RpcClient::RpcClient(const std::string& serverName)
    : serverName_(serverName)
    , requestPipe_(open_only, serverName + "_request", read_write)
    , responsePipe_(create_only, "response_" + serverName_, read_write)
{
}

RpcClient::~RpcClient() {
    try {
        requestPipe_.close();
        responsePipe_.close();
        remove(("response_" + serverName_).c_str());
    } catch (const std::exception& e) {
        // Handle any exceptions during cleanup
    }
}

template<typename RequestT, typename ResponseT>
ResponseT RpcClient::sendRequest(const RequestT& request) {
    // Create HelloRequest
    flatbuffers::FlatBufferBuilder fbb;
    auto request = CreateHelloRequest(request_type, message);
    
    // Send request through message queue (priority 0, message type 1)
    request_pipe_.send(request.data(), request.size(), 0, 1);
    
    // Receive response from message queue
    char buffer[1024];
    unsigned int priority = 0;
    response_pipe_.receive(buffer, 1024, priority, 1);
    
    // Create a HelloResponse from the received buffer
    auto response = HelloResponse::GetRootAsHelloResponse(buffer);
    
    return response;
}

template RpcClient::sendRequest<flatbuffers::FlatBufferBuilder, flatbuffers::FlatBufferBuilder>;

flatbuffers::FlatBufferBuilder RpcClient::CreateHelloRequest(RequestType type, const std::string& message) {
    flatbuffers::FlatBufferBuilder fbb;
    
    // Create the HelloRequest
    auto request = HelloRequest::CreateHelloRequest(
        fbb, 
        type, 
        fbb.CreateString(message)
    );
    
    return fbb;
}
