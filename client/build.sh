#!/bin/bash

echo "Building Go client..."
GOOS=linux GOARCH=amd64 go build -o client/client main.go
