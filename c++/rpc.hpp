#ifndef RPC_HPP
#define RPC_HPP

#include <boost/interprocess/ipc/message_queue.hpp>
#include <string>

using namespace boost::interprocess;

namespace rpc {

class RpcClient {
public:
    explicit RpcClient(const std::string& service);
    
    ~RpcClient();
    
    void sendRequest(unsigned char *request, std::size_t request_size, unsigned char *response, std::size_t response_size);
    
private:
    std::string service_;
    message_queue request_;
    message_queue response_;
};

} // namespace rpc

#endif // rpc.hpp
