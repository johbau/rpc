#!/bin/bash

# Generate FlatBuffers schema
flatc --cpp hello.fbs
flatc --go hello.fbs

rm -f c++/hello_generated
mv hello_generated.h c++
rm -rf go/Hello
mv -u Hello go
