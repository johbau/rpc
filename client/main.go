package main

import (
	"context"
	"fmt"
	"log"

	"github.com/aceofkid/posix_mq"
	"github.com/google/flatbuffers"
	"github.com/johbau/rpc/go/Hello"
)

const (
	serviceName = "hello"
	queueName   = "/client_queue_hello"
)

func main() {
	// Create or open the client message queue
	clientMq, err := posix_mq.NewMessageQueue(queueName, posix_mq.O_RDONLY | posix_mq.O_CREAT, 0666, nil)
	if err != nil {
		log.Fatalf("Failed to create client message queue: %v", err)
	}
	defer clientMq.Close()
	defer clientMq.Unlink()

	// Create or open the server message queue
	serverMq, err := posix_mq.NewMessageQueue("/server_queue_hello", posix_mq.O_WRONLY | posix_mq.O_CREAT, 0666, nil)
	if err != nil {
		log.Fatalf("Failed to open server message queue: %v", err)
	}
	defer serverMq.Close()

	// Create a FlatBuffer request
	builder := flatbuffers.NewBuilder(1024)
	Hello.HelloRequestStart(builder)
	Hello.HelloRequestAddGreetingStr(builder, "Hello from Go client!")
	request := Hello.HelloRequestEnd(builder)
	builder.Finish(request)

	// Send request to server
	_, err = serverMq.Send(context.Background(), builder.Data[0:request.BytesWritten()], 0)
	if err != nil {
		log.Fatalf("Failed to send request: %v", err)
	}

	// Receive response
	response := make([]byte, 1024)
	_, err = clientMq.Receive(context.Background(), response, 0)
	if err != nil {
		log.Fatalf("Failed to receive response: %v", err)
	}

	// Parse the response
	root := Hello.GetRootAsHelloResponse(bytes.NewBuffer(response))
	log.Printf("Received response: %s", root.Greeting())
}
