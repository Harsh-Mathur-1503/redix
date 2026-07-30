# Networking Design (v0)

## Overview

The networking layer is responsible for exposing Redix over TCP.

Its responsibilities are intentionally limited:

* Accept incoming client connections.
* Receive raw request data.
* Frame incoming bytes into complete request lines.
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

Each client connection owns its own temporary input buffer. The buffer persists for the lifetime of that connection and allows fragmented and pipelined requests to be processed correctly.

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
handleClient()
        │
        ├── readRequest()
        ├── RequestHandler
        ├── sendAll()
        └── repeat until disconnect
    │
    ▼
close(client)
```

Each stage has a dedicated responsibility:

| Function         | Purpose                                                               |
| ---------------- | --------------------------------------------------------------------- |
| `socket()`       | Create a TCP socket.                                                  |
| `setsockopt()`   | Configure socket options such as `SO_REUSEADDR`.                      |
| `bind()`         | Associate the socket with a local IP address and port.                |
| `listen()`       | Mark the socket as a listening socket.                                |
| `accept()`       | Accept an incoming client connection.                                 |
| `handleClient()` | Process one client session until it disconnects or an error occurs.   |
| `readRequest()`  | Read and frame exactly one request line from the client.              |
| `sendAll()`      | Ensure the complete response is transmitted, handling partial writes. |
| `close()`        | Close the client connection and release resources.                    |

---

# Server Configuration (v0)

| Setting                 | Value                 |
| ----------------------- | --------------------- |
| Host                    | `127.0.0.1`           |
| Port                    | `6379`                |
| Maximum Request Size    | `4096 bytes`          |
| Receive Buffer Size     | `4096 bytes`          |
| Protocol                | Line-based text       |
| Connection Type         | Blocking              |
| Threading               | Single-threaded       |
| Requests per Connection | Multiple (sequential) |

---

# Protocol Rules

The networking layer uses a simple line-based text protocol.

* Every command is terminated by `\n`.
* `\r\n` is also accepted.
* A request payload may contain at most **4096 bytes**, excluding the line terminator.
* Responses are newline terminated.
* Requests are processed sequentially in the order they are received.
* Exactly one response is produced for each complete request.

Examples:

```text
PING\n
```

```text
SET name harsh\r\n
```

---

# Request Lifecycle

For each connected client:

1. Client connects.
2. `TcpServer` accepts the connection.
3. `handleClient()` creates a per-client input buffer.
4. `readRequest()` checks for a complete buffered request before calling `recv()`.
5. Incoming bytes are accumulated until a complete line is available.
6. One request is extracted from the buffer.
7. Remaining bytes (if any) are preserved for subsequent requests.
8. The completed request is forwarded to `RequestHandler`.
9. `Parser` converts the text into a `Command`.
10. `CommandExecutor` executes the command.
11. `KeyValueStore` performs any storage operations.
12. `sendAll()` transmits the complete response.
13. The server repeats the process until the client disconnects or an error occurs.

---

# Request Framing

TCP is a byte stream rather than a message-oriented protocol. A single command may arrive across multiple `recv()` calls, while multiple commands may arrive in a single `recv()`.

Examples:

Fragmentation:

```text
recv #1: PI
recv #2: NG\n
```

becomes

```text
PING
```

Pipelining:

```text
PING\nGET name\nSET age 21\n
```

is processed as

```text
PING
GET name
SET age 21
```

with one response generated for each request.

---

# Error Handling

The server performs basic error handling for system calls.

Current behavior:

* Failure during `socket()`, `bind()`, `listen()`, or `setsockopt()` is treated as a fatal startup error.
* `accept()`, `recv()`, and `send()` automatically retry when interrupted by `EINTR`.
* If `recv()` returns `0`, the client has disconnected.
* `sendAll()` retries partial writes until the complete response has been transmitted.
* If a request exceeds the maximum allowed size, the server responds with:

```text
ERR request too large
```

and immediately closes the client connection.

* If a client disconnects before sending a complete line, the incomplete request is discarded.

System call failures are reported using `perror()`.

---

# Current Limitations

* Blocking server
* Single-threaded
* One client can block all others while connected
* Maximum request size: 4096 bytes
* No authentication
* No RESP support
* No persistence
* No replication
* No I/O multiplexing

---

# Future Improvements

Planned networking enhancements include:

* Graceful server shutdown.
* Configurable host and port.
* Connection logging.
* Idle client timeouts.
* Thread-safe request handling.
* I/O multiplexing using `poll()`, `kqueue()`, or `epoll()`.
* RESP (Redis Serialization Protocol) support.
* Connection pooling.
* Performance benchmarking.

---

# Design Principles

The networking layer follows these principles:

* Keep networking independent of application logic.
* Delegate request processing to the `RequestHandler`.
* Keep socket management explicit and localized.
* Maintain one input buffer per client connection.
* Process requests sequentially in arrival order.
* Handle TCP fragmentation and pipelining transparently.
* Build incrementally before introducing concurrency or asynchronous I/O.
