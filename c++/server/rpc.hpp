//
// Created by johannes on 3/12/25.
//

#ifndef RPC_HPP
#define RPC_HPP

#include <mqueue.h>
#include <system_error>
#include <stdexcept>

class RpcServer {
public:
    explicit RpcServer(const std::string& service);
    ~RpcServer();

    bool handleRequest();

private:
    mqd_t serverQueue_;
    static constexpr int MaxMsgSize = 4096;
    static constexpr struct timespec Timeout {5,0};

    void processMessage(char* buffer, ssize_t msgLen) const;
};
#endif //RPC_HPP
