#!/bin/bash

# Generate FlatBuffers schema
flatc --cpp hello.fbs

# Build the project
g++ -std=c++11 main.cpp rpc.cpp -o mq_rpc_client -lpthread -lboost_system -lrt

# Run RPC client
./mq_rpc_client
