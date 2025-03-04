#define _GLIBCXX_USE_CXX11_ABI 0
#include "rpc.hpp"
#include <boost/interprocess/sync/stream_open_or_create.hpp>
#include <boost/interprocess/mapped_regions/map_reg.hpp>
#include <boost/interprocess/streams/bufferstream.hpp>
#include <flatbuffers/util.h>

using namespace rpc;

RpcClient::RpcClient(const std::string& serverName)
    : serverName_(serverName)
    , requestPipe_(open_only, serverName + "_request")
    , responsePipe_(create_only, "response_" + serverName_, read_write, 1024)
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
    
    // Write request to request pipe
    request_pipe_.write(request.data(), request.size());
    
    // Read response from response pipe
    char buffer[1024];
    response_pipe_.read(buffer, 1024);
    
    // Create a HelloResponse from the received buffer
    auto response = HelloResponse::GetRootAsHelloResponse(buffer);
    
    return response;
}

template RpcClient::sendRequest<flatbuffers::FlatBufferBuilder, flatbuffers::FlatBufferBuilder>;
