# Redix

A Redis-inspired in-memory key-value store built from scratch in modern C++20.

Redix is a systems programming project focused on understanding how databases and network services are built internally. Instead of relying on existing frameworks, the project incrementally implements command parsing, request execution, storage management, testing infrastructure, and eventually networking, persistence, and concurrency.

---

## Motivation

Redis is one of the most widely used in-memory databases and caching systems.

The goal of Redix is not to clone Redis feature-for-feature, but to learn the underlying engineering principles behind:

* Request parsing
* Command execution
* In-memory data structures
* Client-server communication
* Network programming
* Persistence
* Concurrency
* Database architecture

This project is being built incrementally with an emphasis on clean architecture, testing, and documentation.

---

## Current Features

### Commands

| Command         | Description         |
| --------------- | ------------------- |
| `PING`          | Health check        |
| `SET key value` | Store a value       |
| `GET key`       | Retrieve a value    |
| `DEL key`       | Delete a key        |
| `EXISTS key`    | Check key existence |

### Example

```text
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

## Protocol

Redix v0 currently uses a simple line-based text protocol.

### Rules

* Commands are case-insensitive.
* Keys are case-sensitive.
* Values cannot contain spaces in v0.
* Commands are separated by whitespace.
* Responses are plain text.

### Errors

```text
ERR empty input
ERR unknown command
ERR invalid arity
```

---

## Architecture

### Request Processing Pipeline

```text
Raw Input
    ↓
RequestHandler
    ↓
Parser
    ↓
CommandExecutor
    ↓
KeyValueStore
    ↓
Response String
```

### Components

#### Parser

Responsible for converting raw text into structured commands.

Example:

```text
SET name harsh
```

becomes:

```text
CommandType::Set
["name", "harsh"]
```

---

#### CommandExecutor

Responsible for executing parsed commands and generating responses.

Example:

```text
GET name
```

becomes:

```text
harsh
```

---

#### KeyValueStore

In-memory storage engine built on top of:

```cpp
std::unordered_map<std::string, std::string>
```

Provides:

```cpp
set(key, value)
get(key)
del(key)
exists(key)
```

---

#### RequestHandler

Coordinates the parser and executor.

This layer allows future networking code to remain simple:

```cpp
response = handler.handleLine(request);
```

---

## Project Structure

```text
redix/
│
├── docs/
│   ├── architecture.md
│   ├── protocol.md
│   └── storage.md
│
├── src/
│   ├── parser/
│   │   ├── command.hpp
│   │   ├── parser.hpp
│   │   └── parser.cpp
│   │
│   ├── storage/
│   │   ├── key_value_store.hpp
│   │   └── key_value_store.cpp
│   │
│   ├── core/
│   │   ├── command_executor.hpp
│   │   ├── command_executor.cpp
│   │   ├── request_handler.hpp
│   │   └── request_handler.cpp
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

## Building

### Requirements

* C++20
* CMake 3.20+
* Clang or GCC

### Build

```bash
cmake -S . -B build
cmake --build build
```

---

## Running Tests

### Individual Test Suites

```bash
./bin/redix_parser_tests
./bin/redix_storage_tests
./bin/redix_executor_tests
./bin/redix_request_handler_tests
```

### CTest

```bash
ctest --test-dir build
```

---

## Development Principles

* Modern C++20
* Separation of concerns
* Test-driven development
* Incremental system design
* Minimal external dependencies
* Clean architecture

---

## Roadmap

### Completed

* Command parser
* In-memory storage engine
* Command executor
* Request handler
* Unit tests
* Integration tests

### In Progress

* Networking layer
* TCP server
* Client connections

### Planned

* RESP protocol support
* Persistence
* Snapshots
* Expiration (TTL)
* Multi-client support
* Thread safety
* Benchmarking
* Replication
* Pub/Sub

---

## Learning Goals

This project is primarily an educational exploration of:

* Systems Programming
* Database Internals
* Network Programming
* Modern C++
* Software Architecture
* Testing and Tooling

---

## License

MIT License
