#!/bin/bash

# Generate FlatBuffers schema
flatc --cpp hello.fbs

# Build the project
g++ -std=c++11 main.cpp rpc.cpp hello_generated.cpp -o example -lpthread -lboost_coroutine -lboost_context

# Run the example
./example
