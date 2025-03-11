package main

import (
	"fmt"
	"os"

	"github.com/aceofkid/posix_mq"
	"github.com/google/flatbuffers/go"
	"github.com/johbau/rpc/go/Hello"
)

func main() {
	// Create request builder
	builder := flatbuffers.NewBuilder(1024)

	// Build HelloRequest
	message := builder.CreateString("Hello from Go")
	Hello.HelloRequestStart(builder)
	Hello.HelloRequestAddRequestType(builder, Hello.RequestTypeGREETING)
	Hello.HelloRequestAddMessage(builder, message)
	request := Hello.HelloRequestEnd(builder)
	builder.Finish(request)

	// Open server message queue
	const service = "hello"
	srvMqName := "/server_queue_" + service
	const srvMqFlags = posix_mq.O_WRONLY

	reqMq, err := posix_mq.NewMessageQueue(srvMqName, srvMqFlags, 0, nil)
	if err != nil {
		fmt.Printf("Failed to open server queue %s: %v\n", srvMqName, err)
		os.Exit(1)
	}
	defer reqMq.Close()

	// Create client message queue for responses
	mqName := "/client_queue_" + service
	const mqFlags = posix_mq.O_RDONLY | posix_mq.O_CREAT | posix_mq.O_EXCL
	const mqMode = 0666

	respMq, err := posix_mq.NewMessageQueue(mqName, mqFlags, mqMode, nil)
	if err != nil {
		fmt.Printf("Failed to create response queue %s: %v\n", mqName, err)
		os.Exit(1)
	}
	defer respMq.Unlink()

	// Send request
	err = reqMq.Send(builder.FinishedBytes(), 0)
	if err != nil {
		fmt.Printf("Failed to send request: %v\n", err)
		return
	}

	// Wait for response
	data, _, err := respMq.Receive()
	if err != nil {
		fmt.Printf("Error receiving response: %v\n", err)
		return
	}

	response := Hello.GetRootAsHelloResponse(data, 0)
	resultLength := response.ResultLength()

	for i := 0; i < resultLength; i++ {
		resStr := string(response.Result(i))
		fmt.Printf("Received response: %s\n", resStr)
	}
}
