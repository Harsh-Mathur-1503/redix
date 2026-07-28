````markdown
# Redix

> A Redis-inspired in-memory key-value store built from scratch in modern C++20.

Redix is a systems programming project whose goal is to understand and implement the core components behind high-performance backend systems rather than simply recreating Redis features.

The project is being developed incrementally, with every phase introducing one new architectural concept while maintaining clean separation of concerns, comprehensive unit tests, and production-style project organization.

---

## Why Redix?

Modern backend systems like Redis demonstrate several important systems programming concepts:

- Efficient in-memory storage
- TCP networking
- Command parsing
- Request processing pipelines
- Event-driven architecture
- Concurrency
- Persistence
- Distributed systems

Instead of implementing everything at once, Redix builds these features layer by layer to better understand the design decisions behind each component.

---

# Current Features

## Command Parser

Supports the following commands:

| Command | Description |
|----------|-------------|
| `PING` | Health check |
| `SET key value` | Store or overwrite a value |
| `GET key` | Retrieve a value |
| `DEL key` | Delete a key |
| `EXISTS key` | Check whether a key exists |

Parser capabilities:

- Case-insensitive commands
- Arity validation
- Unknown command detection
- Empty input detection
- Structured `Command` representation

---

## In-Memory Storage

Implemented using:

```cpp
std::unordered_map<std::string, std::string>
```

Supported operations:

- set()
- get()
- del()
- exists()

The storage layer is intentionally independent from parsing, networking, and protocol logic.

---

## Command Execution

A dedicated `CommandExecutor` converts parsed commands into application responses.

Examples:

| Request | Response |
|----------|----------|
| `PING` | `PONG` |
| `SET name harsh` | `OK` |
| `GET name` | `harsh` |
| `GET unknown` | `NIL` |
| `DEL name` | `1` |
| `EXISTS name` | `1` |

Error responses:

```
ERR empty input
ERR unknown command
ERR invalid arity
```

---

## Request Pipeline

Networking is intentionally separated from business logic.

```
Raw Request
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

## TCP Server

Current implementation:

- POSIX sockets
- Blocking I/O
- IPv4
- One client connection at a time
- One request per connection
- Line-based text protocol

Server lifecycle:

```
socket()
setsockopt()
bind()
listen()
accept()
recv()
RequestHandler
send()
close()
```

Default configuration:

| Setting | Value |
|---------|------|
| Host | `127.0.0.1` |
| Port | `6379` |
| Buffer Size | `4096 bytes` |

---

# Project Structure

```text
redix/
├── docs/
│   ├── architecture.md
│   ├── networking.md
│   ├── protocol.md
│   └── storage.md
│
├── src/
│   ├── core/
│   │   ├── command_executor.*
│   │   └── request_handler.*
│   │
│   ├── networking/
│   │   └── tcp_server.*
│   │
│   ├── parser/
│   │   ├── command.hpp
│   │   ├── parser.hpp
│   │   └── parser.cpp
│   │
│   ├── storage/
│   │   ├── key_value_store.hpp
│   │   └── key_value_store.cpp
│   │
│   └── main.cpp
│
├── tests/
│   ├── parser_tests.cpp
│   ├── storage_tests.cpp
│   ├── command_executor_tests.cpp
│   └── request_handler_tests.cpp
│
├── CMakeLists.txt
└── README.md
```

---

# Building

Requirements:

- C++20 compiler
- CMake 3.20+
- macOS or Linux

Build:

```bash
cmake -S . -B build
cmake --build build
```

---

# Running

Start the server:

```bash
./bin/redix
```

Expected output:

```
Redix listening on 127.0.0.1:6379
```

---

# Testing

Run all tests using CTest:

```bash
ctest --test-dir build
```

Or execute each test suite individually:

```bash
./bin/redix_parser_tests
./bin/redix_storage_tests
./bin/redix_executor_tests
./bin/redix_request_handler_tests
```

---

# Manual Testing

Using Netcat:

```bash
nc 127.0.0.1 6379
```

Example session:

```
PING
PONG

SET name harsh
OK

GET name
harsh

EXISTS name
1

DEL name
1

GET name
NIL
```

---

# Design Principles

Redix follows a few guiding principles throughout development.

- Separation of concerns
- Incremental architecture
- Unit-test driven development
- Modern C++20 practices
- Readable and maintainable code
- Clear module boundaries before optimization

---

# Current Limitations

Current implementation intentionally keeps networking simple.

- Blocking sockets
- Single-threaded
- One request per connection
- No RESP protocol
- No persistence
- No TTL
- No replication
- No clustering
- No authentication
- No transactions

These limitations are deliberate to keep each development phase focused.

---

# Roadmap

### Phase 1 (Completed)

- Command parser
- Key-value storage
- Command executor
- Request handler
- Blocking TCP server
- Unit tests

### Phase 2

- Multiple commands per connection
- Proper line buffering
- Partial read/write handling

### Phase 3

- RESP protocol
- Better protocol framing

### Phase 4

- Thread-per-client architecture
- Synchronization primitives

### Phase 5

- Event-driven networking (`poll`, `epoll`, `kqueue`)

### Phase 6

- Persistence (RDB / AOF)

### Phase 7

- Replication
- Configuration system
- Performance benchmarking

---

# Learning Objectives

This project is intended to gain practical experience with:

- Modern C++20
- Systems programming
- POSIX networking
- Socket programming
- Software architecture
- Testable application design
- Build systems (CMake)
- Backend infrastructure fundamentals

---

## License

This project is developed as a learning exercise inspired by Redis. It is not intended to be a production replacement for Redis.
````
