#include <iostream>
#include "rpc.hpp"

int main() {
    // Create and run the RPC server
    try {
        const std::string service = "hello";
        std::cout << "Starting RPC server for service: " << service << std::endl;

        RpcServer server(service);

        while(true) {
            if(!server.handleRequest()) {
                // Sleep to prevent CPU spin
                usleep(100000); // 100ms
            }
        }
    } catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}