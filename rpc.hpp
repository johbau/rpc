#ifndef RPC_HPP
#define RPC_HPP

#include <boost/interprocess/named_pipe.hpp>
#include <flatbuffers/flatbuffer.h>
#include <flatbuffers/flatbuffers.h>
#include <string>
#include <memory>

using namespace boost::interprocess;

namespace rpc {

class RPCClient {
public:
    explicit RPCClient(const std::string& server_name);
    
    ~RPCClient();
    
    template<typename RequestT, typename ResponseT>
    ResponseT send_request(const RequestT& request);
    
private:
    named_pipe_client request_pipe_;
    named_pipe_server response_pipe_;
    std::string server_name_;
};

} // namespace rpc

#endif // RPC_HPP
