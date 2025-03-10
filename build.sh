#!/bin/bash

# Generate FlatBuffers schema
flatc --cpp hello.fbs
flatc --go hello.fbs

rm -f c++/hello_generated.h
mv hello_generated.h c++
rm -rf go/Hello
mv -u Hello go

# Build Go client and server binaries
mkdir -p go/client/bin
cd go/server && go build -o ../bin/rpc_server .
cd ../../go/client && go build -o ../bin/rpc_client .
