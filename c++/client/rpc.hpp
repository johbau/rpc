#ifndef RPC_HPP
#define RPC_HPP

#include <mqueue.h>
#include <sys/stat.h>
#include <string>

#define MAX_SIZE 1024

namespace rpc {

class RpcClient {
public:
    explicit RpcClient(const std::string& service);
    
    ~RpcClient();
    
    void sendRequest(unsigned char *request, std::size_t request_size, unsigned char *response, std::size_t response_size);
    
private:
    std::string service_;
    mqd_t request_;
    mqd_t response_;
};

} // namespace rpc

#endif // rpc.hpp
