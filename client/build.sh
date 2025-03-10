#!/bin/bash

echo "Building Go client..."
GOOS=linux GOARCH=amd64 go build -o go/client/client go/client/main.go
