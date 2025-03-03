#define _GLIBCXX_USE_CXX11_ABI 0
#include "rpc.hpp"
#include <boost/interprocess/sync/stream_open_or_create.hpp>
#include <boost/interprocess/mapped_regions/map_reg.hpp>
#include <boost/interprocess/streams/bufferstream.hpp>
#include <flatbuffers/util.h>

using namespace rpc;

rpcClient::rpcClient(const std::string& serverName)
    : serverName_(serverName)
    , requestPipe_(open_only, serverName + "_request")
    , responsePipe_(create_only, "response_" + serverName_, read_write, 1024)
{
}

rpcClient::~rpcClient() {
    try {
        requestPipe_.close();
        responsePipe_.close();
        remove(("response_" + serverName_).c_str());
    } catch (const std::exception& e) {
        // Handle any exceptions during cleanup
    }
}

template<typename RequestT, typename ResponseT>
ResponseT rpcClient::sendRequest(const RequestT& request) {
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

template rpcClient::sendRequest<flatbuffers::FlatBufferBuilder, flatbuffers::FlatBufferBuilder>;
