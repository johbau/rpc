#include "rpc.hpp"
#include <boost/interprocess/ipc/message_queue.hpp>

#define MAX_SIZE 1024
#define SERVER_QUEUE_NAME(suffix) (("/server_queue_" + suffix).c_str())
#define CLIENT_QUEUE_NAME(suffix) (("/client_queue_" + suffix).c_str())

using namespace rpc;

RpcClient::RpcClient(const std::string& service)
    : service_(service)
    , request_(open_only, SERVER_QUEUE_NAME(service))
    , response_(create_only, CLIENT_QUEUE_NAME(service), 1, MAX_SIZE)
{
}

RpcClient::~RpcClient() {
    try {
        remove(CLIENT_QUEUE_NAME(service_));
    } catch (const std::exception& e) {
        // Handle any exceptions during cleanup
    }
}

void RpcClient::sendRequest(unsigned char *request, std::size_t request_size, unsigned char *response, std::size_t response_size) {
    // Send request through message queue (priority 0)
    request_.send(request, request_size, 0);
    
    // Receive response from message queue (priority 0)
    message_queue::size_type received_size;
    unsigned int priority;
    response_.receive(response, response_size, received_size, priority);
}
