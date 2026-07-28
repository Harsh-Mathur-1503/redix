
# Networking Design (v0)

## Overview

The networking layer is responsible for exposing Redix over TCP.

Its responsibilities are intentionally limited:

* Accept incoming client connections.
* Receive raw request data.
* Forward requests to the `RequestHandler`.
* Send responses back to the client.
* Manage socket lifecycle.

The networking layer **does not** parse commands or perform storage operations. Those responsibilities belong to the parser, command executor, and key-value store.

---

# Architecture

The networking layer acts as the entry point to the application.

```text
Client
    │
    ▼
TcpServer
    │
    ▼
RequestHandler
    │
    ▼
Parser
    │
    ▼
CommandExecutor
    │
    ▼
KeyValueStore
    │
    ▼
Response
```

---

# Socket Lifecycle

The server follows the standard POSIX socket lifecycle.

```text
socket()
    │
    ▼
setsockopt()
    │
    ▼
bind()
    │
    ▼
listen()
    │
    ▼
accept()
    │
    ▼
recv()
    │
    ▼
RequestHandler
    │
    ▼
send()
    │
    ▼
close(client)
```

Each stage has a dedicated responsibility:

| Function         | Purpose                                                |
| ---------------- | ------------------------------------------------------ |
| `socket()`     | Create a TCP socket.                                   |
| `setsockopt()` | Enable socket options such as`SO_REUSEADDR`.         |
| `bind()`       | Associate the socket with a local IP address and port. |
| `listen()`     | Mark the socket as a listening socket.                 |
| `accept()`     | Accept an incoming client connection.                  |
| `recv()`       | Read request bytes from the connected client.          |
| `send()`       | Send the response back to the client.                  |
| `close()`      | Close the client connection and release resources.     |

---

# Server Configuration (v0)

| Setting                 | Value           |
| ----------------------- | --------------- |
| Host                    | `127.0.0.1`   |
| Port                    | `6379`        |
| Buffer Size             | `4096 bytes`  |
| Protocol                | Line-based text |
| Connection Type         | Blocking        |
| Threading               | Single-threaded |
| Requests per Connection | One             |

---

# Request Lifecycle

Each client request follows the pipeline below.

```text
Client connects
        │
        ▼
Receive request line
        │
        ▼
RequestHandler::handleLine()
        │
        ▼
Generate response
        │
        ▼
Send response
        │
        ▼
Close client connection
```

In v0, each TCP connection is expected to contain exactly one request.

---

# Error Handling

The server performs basic error handling for system calls.

Current behavior:

* Failure during `socket()`, `bind()`, `listen()`, or `setsockopt()` is treated as a fatal startup error.
* Failure during `accept()` logs the error and continues accepting new clients.
* If `recv()` returns `0`, the client has disconnected.
* Errors are reported using `perror()`.

Future versions may introduce structured error handling and logging.

---

# Current Limitations

The initial networking implementation intentionally favors simplicity over completeness.

Current limitations include:

* Blocking I/O.
* One request per connection.
* IPv4 only.
* Single client at a time.
* Fixed-size receive buffer.
* Assumes an entire request is received in a single `recv()` call.
* Assumes `send()` transmits the complete response in one call.

These assumptions simplify the implementation and allow the networking model to be understood before introducing more advanced concepts.

---

# Future Improvements

Planned networking enhancements include:

* Persistent client connections.
* Multiple requests per connection.
* Partial read and partial write handling.
* Graceful server shutdown.
* Configurable host and port.
* Connection logging.
* Thread-safe request handling.
* I/O multiplexing using `select()`, `poll()`, or `epoll()`.
* RESP (Redis Serialization Protocol) support.
* Performance benchmarking.

---

# Design Principles

The networking layer follows the following principles:

* Keep networking independent of application logic.
* Delegate request processing to the `RequestHandler`.
* Keep socket management simple and explicit.
* Minimize responsibilities within the `TcpServer` class.
* Build incrementally before introducing concurrency or asynchronous I/O.
