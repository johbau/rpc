#!/bin/bash

# Build the project
g++ -std=c++11 main.cpp rpc.cpp -o client -lpthread -lrt
