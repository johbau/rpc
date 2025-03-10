package main

import (
	"fmt"
	"os"
	"time"

	"github.com/aceofkid/posix_mq"
	"github.com/google/flatbuffers/go"
	"github.com/johbau/rpc/go/Hello"
)

func main() {
	// Create or open the server message queue
	const serverFlag = posix_mq.O_RDONLY | posix_mq.O_CREAT
	serverMq, err := posix_mq.NewMessageQueue("/server_queue_hello", serverFlag, 0666, nil)
	if err != nil {
		fmt.Printf("Failed to create message queue: %v\n", err)
		os.Exit(1)
	}
	defer serverMq.Close()
	defer serverMq.Unlink()

	// Start processing requests
	fmt.Println("RPC server is listening on /server_queue_hello...")

	for {
		// Receive message from queue
		msg, _, err := serverMq.Receive()
		if err != nil {
			//fmt.Printf("Failed to receive message: %v\n", err)
			time.Sleep(1 * time.Second)
			continue
		}

		// Process the request using FlatBuffers schema
		request := Hello.GetRootAsHelloRequest(msg, 0)
		if request == nil {
			fmt.Printf("Failed to parse request\n")
			continue
		}

		requestType := request.RequestType()
		switch requestType {
		case Hello.RequestTypeGREETING:
			fmt.Printf("request GREETING\n")
		case Hello.RequestTypeGOODBYE:
			fmt.Printf("request GOODBYE\n")
		default:
			fmt.Printf("unknown request\n")
		}
		message := string(request.Message())
		fmt.Printf("message: %s\n", message)

		// Create a buffer for FlatBuffers
		builder := flatbuffers.NewBuilder(1024)

		// Create the response using the Flatbuffers schema
		result1 := builder.CreateString("Hello client")
		Hello.HelloResponseStartResultVector(builder, 1)
		builder.PrependUOffsetT(result1)
		result := builder.EndVector(1)
		Hello.HelloResponseStart(builder)
		Hello.HelloResponseAddResult(builder, result)
		response := Hello.HelloResponseEnd(builder)
		builder.Finish(response)

		// Open the client message queue
		const clientFlag = posix_mq.O_WRONLY
		clientMq, err := posix_mq.NewMessageQueue("/client_queue_hello", clientFlag, 0, nil)
		if err != nil {
			fmt.Printf("Failed to open client queue: %v\n", err)
			os.Exit(1)
		}

		err = clientMq.Send(builder.FinishedBytes(), 0)
		if err != nil {
			fmt.Printf("Failed to send response: %v\n", err)
		}

		clientMq.Close()
	}
}
