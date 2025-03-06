package main

import (
    "encoding/json"
    "fmt"
    "os"
    "syscall"
)

type Request struct {
    Method string      `json:"method"`
    Args   interface{} `json:"args"`
}

type Response struct {
    Result interface{} `json:"result"`
    Error  string      `json:"error"`
}

func main() {
    // Create or open the message queue
    mq, err := syscall.Mq_open("/rpc_server_queue", syscall.O_CREAT|syscall.O_RDWR, 0600, &syscall.MqAttr{})
    if err != nil {
        fmt.Printf("Failed to create message queue: %v\n", err)
        os.Exit(1)
    }
    defer syscall.Mq_close(mq)
    defer syscall.Mq_unlink("/rpc_server_queue")

    // Start processing requests
    fmt.Println("RPC server is listening on /rpc_server_queue...")
    
    for {
        // Receive message from queue
        msg := make([]byte, 1024)
        n, err := syscall.Mq_receive(mq, msg, 0)
        if err != nil {
            fmt.Printf("Failed to receive message: %v\n", err)
            continue
        }

        // Unmarshal the request
        var req Request
        err = json.Unmarshal(msg[:n], &req)
        if err != nil {
            fmt.Printf("Failed to unmarshal request: %v\n", err)
            continue
        }

        // Process the request
        var resp Response
        switch req.Method {
        case "add":
            a, b := req.Args.([]int)
            resp.Result = a + b
        case "echo":
            resp.Result = req.Args
        default:
            resp.Error = "Unknown method"
        }

        // Send response back to client queue
        clientQueueName, ok := req.Args.(string)
        if !ok {
            fmt.Println("No client queue name provided")
            continue
        }

        clientMq, err := syscall.Mq_open(clientQueueName, syscall.O_WRONLY, 0, nil)
        if err != nil {
            fmt.Printf("Failed to open client queue: %v\n", err)
            continue
        }
        
        responseBytes, err := json.Marshal(resp)
        if err != nil {
            fmt.Printf("Failed to marshal response: %v\n", err)
            syscall.Mq_close(clientMq)
            continue
        }

        _, err = syscall.Mq_send(clientMq, responseBytes, 0)
        if err != nil {
            fmt.Printf("Failed to send response: %v\n", err)
        }
        
        syscall.Mq_close(clientMq)
    }
}
