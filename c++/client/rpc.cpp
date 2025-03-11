#include "rpc.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include <stdexcept>
#include <mqueue.h>

#define MAX_SIZE 1024
#define SERVER_QUEUE_NAME(suffix) (("/server_queue_" + suffix).c_str())
#define CLIENT_QUEUE_NAME(suffix) (("/client_queue_" + suffix).c_str())

using namespace rpc;

RpcClient::RpcClient(const std::string& service)
    : service_(service)
{
    struct mq_attr attr = {0};
    attr.mq_maxmsg = 1;
    attr.mq_msgsize = MAX_SIZE;

    // Open server queue (request queue)
    request_ = mq_open(SERVER_QUEUE_NAME(service), O_WRONLY);
    if(request_ == -1) {
        throw std::runtime_error("Failed to open server queue");
    }

    // Create client response queue
    response_ = mq_open(CLIENT_QUEUE_NAME(service), O_CREAT | O_RDONLY, 0644, &attr);
    if(response_ == -1) {
        throw std::runtime_error("Failed to create client queue");
    }
}

RpcClient::~RpcClient() {
    mq_close(request_);
    mq_close(response_);
    mq_unlink(CLIENT_QUEUE_NAME(service_));
}

void RpcClient::sendRequest(unsigned char *request, std::size_t request_size, unsigned char *response, std::size_t response_size) {
    // Send request through message queue
    if(mq_send(request_, reinterpret_cast<char*>(request), request_size, 0) == -1) {
        throw std::runtime_error("Failed to send message");
    }

    // Receive response with timeout (5 seconds)
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 5;

    ssize_t bytes = mq_timedreceive(response_, reinterpret_cast<char*>(response),
                                  response_size, nullptr, &ts);
    if(bytes == -1) {
        throw std::runtime_error("Failed to receive message");
    }
}
