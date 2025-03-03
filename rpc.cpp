#define _GLIBCXX_USE_CXX11_ABI 0
#include "rpc.hpp"
#include <boost/interprocess/sync/stream_open_or_create.hpp>
#include <boost/interprocess/mapped_regions/map_reg.hpp>
#include <boost/interprocess/streams/bufferstream.hpp>
#include <flatbuffers/util.h>

using namespace rpc;

RPCClient::RPCClient(const std::string& server_name)
    : server_name_(server_name)
    , request_pipe_(open_only, server_name + "_request")
    , response_pipe_(create_only, "response_" + server_name_, read_write, 1024)
{
}

RPCClient::~RPCClient() {
    request_pipe_.close();
    response_pipe_.close();
    remove("response_" + server_name_.c_str());
}

template<typename RequestT, typename ResponseT>
ResponseT RPCClient::send_request(const RequestT& request) {
    // Write request to request pipe
    request_pipe_.write(request.data(), request.size());
    
    // Read response from response pipe
    char buffer[1024];
    response_pipe_.read(buffer, 1024);
    
    // Deserialize flatbuffer response
    flatbuffers::FlatBufferBuilder fbb;
    auto response = flatbuffers::GetResponseT(buffer);
    
    return response;
}

template RPCClient::send_request<flatbuffers::FlatBufferBuilder, flatbuffers::FlatBufferBuilder>;
