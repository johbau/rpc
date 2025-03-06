#ifndef RPC_HPP
#define RPC_HPP

#include <boost/interprocess/message_queue.hpp>
#include <flatbuffers/flatbuffer.h>
#include <flatbuffers/flatbuffers.h>
#include "hello_generated.h"
#include <string>
#include <memory>

using namespace boost::interprocess;

namespace rpc {

class RpcClient {
public:
    explicit RpcClient(const std::string& serverName);
    
    ~RpcClient();
    
    template<typename RequestT, typename ResponseT>
    ResponseT sendRequest(const RequestT& request);
    
    // Helper method to create a HelloRequest
    static flatbuffers::FlatBufferBuilder CreateHelloRequest(RequestType type, const std::string& message);
    
private:
    message_queue requestPipe_;
    message_queue responsePipe_;
    std::string serverName_;
};

} // namespace rpc

#endif // rpc.hpp
