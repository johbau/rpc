#include "rpc.hpp"
#include "../hello_generated.h"
#include "flatbuffers/flatbuffers.h"
#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>

RpcServer::RpcServer(const std::string& service) {
    // Create message queue for server (with O_CREAT flag)
    const std::string queueName = "/server_queue_" + service;

    struct mq_attr attr{
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_msgsize = MaxMsgSize
    };

    serverQueue_ = mq_open(queueName.c_str(), O_RDWR | O_CREAT, 0666, &attr);
    if(serverQueue_ == -1) {
      throw std::runtime_error("Failed to create server message queue: " +
                               std::system_error(errno, std::generic_category()).what());
    }

    std::cout << "Created server message queue: " << queueName << "\n";
}

RpcServer::~RpcServer() {
    mq_close(serverQueue_);
}

bool RpcServer::handleRequest() {
     char buffer[MaxMsgSize];

     struct timespec ts;
     clock_gettime(CLOCK_REALTIME, &ts);
     ts.tv_sec += Timeout.tv_sec;

     ssize_t msgLen = mq_timedreceive(serverQueue_, buffer, MaxMsgSize, nullptr, &ts);

     if(msgLen > 0) {
         processMessage(buffer, msgLen);
         return true;
     }

     if(errno != ETIMEDOUT) { // Real error
         throw std::runtime_error("Error receiving message: " +
                                 std::system_error(errno, std::generic_category()).what());
     }

     return false;  // Timeout is normal behavior for polling
}

 void RpcServer::processMessage(char* buffer, ssize_t msgLen) const {
     auto request = Hello::GetHelloRequest(buffer);

     FlatBufferBuilder fbb;

     switch(request->request_type()) {
         case Hello::RequestType_GREETING: {
             // Create response message
             std::string_view originalMsg(reinterpret_cast<const char*>(request->message()->data()),
                                        request->message()->size());

             auto modified = fbb.CreateString("Server received: " + std::string(originalMsg));

             Hello::HelloResponseBuilder resp(fbb);
             resp.add_result(modified);
             auto response = resp.Finish();
             fbb.Finish(response);

             // Get the client's return queue
             const char* replyQueueName = reinterpret_cast<const char*>(request->message()->data());

             mqd_t clientQueue = mq_open(replyQueueName, O_WRONLY);
             if(clientQueue == -1) {
                 std::cerr << "Failed to open client queue: " << replyQueueName << "\n";
                 return;
             }

             // Send response back
             if(mq_send(clientQueue, reinterpret_cast<char*>(fbb.GetBufferPointer()), fbb.GetSize(), 0) == -1) {
                 std::cerr << "Failed to send response: "
                          << std::system_error(errno, std::generic_category()).what() << "\n";
             }

             mq_close(clientQueue);
         } break;

         default:
             throw std::runtime_error("Unknown request type");
     }
 }