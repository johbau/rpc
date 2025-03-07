#!/bin/bash

# Generate FlatBuffers schema
flatc --cpp hello.fbs
flatc --go hello.fbs

# Build the project
g++ -std=c++11 main.cpp rpc.cpp -o client -lpthread -lboost_system -lrt
