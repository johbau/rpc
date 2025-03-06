#!/bin/bash

# Generate FlatBuffers schema
flatc --cpp hello.fbs

# Build the project
g++ -std=c++11 main.cpp rpc.cpp -o example -lpthread -lboost_system -lboost_filesystem -lboost_interprocess

# Run the example
./example
