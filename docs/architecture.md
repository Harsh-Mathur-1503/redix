# Redix - Architecture Overview

## Goal

Redix is a Redis-inspired in-memory key-value store implemented in modern C++20.

The objective of this project is to understand and implement the fundamental building blocks behind high-performance backend systems rather than simply reproducing Redis features. The project is developed incrementally, with each phase introducing one new architectural concept while keeping the existing system clean, testable, and modular.

The initial version focuses on:

- Text-based command parsing
- In-memory key-value storage
- Request execution pipeline
- TCP networking using POSIX sockets

Future iterations will progressively introduce persistence, concurrency, efficient I/O, replication, and distributed system concepts.

---

# Initial Commands

Redix v0 supports a small set of string-based commands.

| Command | Arguments | Description |
|----------|-----------|-------------|
| `PING` | none | Health check |
| `SET` | `key value` | Store or overwrite a value |
| `GET` | `key` | Retrieve a value |
| `DEL` | `key` | Delete a key |
| `EXISTS` | `key` | Check whether a key exists |

---

# Non-Goals for v0

The following features are intentionally excluded from the first version.

- RESP protocol
- Persistence (RDB/AOF)
- Replication
- Clustering
- Authentication
- Transactions
- Expiration (TTL)
- Event-driven I/O
- Multithreading
- Memory optimization

Keeping v0 intentionally small allows each architectural layer to be understood before introducing additional complexity.

---

# High-Level Architecture

```
                   TCP Client
                        │
                        ▼
                 +---------------+
                 |   TcpServer   |
                 +---------------+
                        │
                        ▼
              +-------------------+
              |  RequestHandler   |
              +-------------------+
                        │
                        ▼
                 +-------------+
                 |   Parser    |
                 +-------------+
                        │
                  Command object
                        │
                        ▼
             +--------------------+
             | CommandExecutor    |
             +--------------------+
                        │
                        ▼
              +-----------------+
              | KeyValueStore   |
              +-----------------+
                        │
                        ▼
                  Response String
                        │
                        ▼
                    TCP Client
```

---

# Request Processing Pipeline

Every client request follows the same processing pipeline.

1. The TCP server accepts a client connection.
2. A request line is read from the socket.
3. The request is passed to `RequestHandler`.
4. The parser converts the raw string into a structured `Command`.
5. The command executor validates the parse result.
6. Valid commands are executed against the key-value store.
7. A response string is returned.
8. The server sends the response back to the client.
9. The client connection is closed (v0).

This layered architecture keeps networking independent from parsing and business logic.

---

# Module Responsibilities

## TcpServer

Responsible for:

- creating sockets
- binding to a TCP port
- accepting client connections
- receiving request data
- sending responses

It intentionally contains **no command logic**.

---

## RequestHandler

Acts as the application's orchestration layer.

Responsibilities:

- receive raw client input
- invoke the parser
- invoke the command executor
- return the final response string

Networking only interacts with this component.

---

## Parser

Converts raw text into a structured `Command`.

Responsibilities:

- tokenize input
- recognize supported commands
- validate command arity
- classify parse errors

The parser performs no storage operations.

---

## CommandExecutor

Contains the application's business logic.

Responsibilities:

- interpret parsed commands
- invoke storage operations
- generate protocol responses

Examples:

```
SET -> OK
GET -> value/NIL
DEL -> 1/0
```

---

## KeyValueStore

Maintains all in-memory data.

Responsibilities:

- insert values
- retrieve values
- delete values
- check key existence

Implementation:

```
std::unordered_map<std::string, std::string>
```

The storage layer is completely independent from networking and parsing.

---

# Protocol Design

Redix v0 uses a simple line-oriented text protocol.

Example:

```
SET name harsh
OK

GET name
harsh

PING
PONG
```

Characteristics:

- commands are case-insensitive
- keys are case-sensitive
- tokens are separated by whitespace
- one request per line
- one response per line

RESP support will be introduced in a later version.

---

# Threading Model

Current implementation:

- single process
- single thread
- blocking sockets
- one client handled at a time

This design keeps the networking code easy to understand before introducing concurrency.

Future versions may adopt:

- thread-per-client
- thread pool
- event-driven I/O (`poll`, `epoll`, or `kqueue`)
- asynchronous networking

---

# Persistence Plan

Version 0 stores all data exclusively in memory.

When the process exits, all data is lost.

Future versions will explore:

- snapshot persistence
- append-only logging
- crash recovery
- configurable persistence strategies

---

# Testing Strategy

Each architectural layer is tested independently.

Current test suites include:

- Parser tests
- Storage tests
- Command executor tests
- Request handler tests

This layered testing approach ensures business logic can be verified without requiring network communication.

---

# Future Work

Planned improvements include:

- Multiple commands per client connection
- Proper line buffering
- Partial read/write handling
- RESP protocol support
- Expiration (TTL)
- Persistence (RDB/AOF)
- Concurrent client handling
- Event-driven networking
- Replication
- Configuration system
- Benchmarking
- Performance profiling

---

# Design Principles

Redix follows several guiding principles throughout development.

- **Separation of concerns** — each module has a single responsibility.
- **Incremental development** — features are introduced one architectural layer at a time.
- **Testability** — core components are unit-tested independently.
- **Modern C++** — leverage C++20 language features and RAII where appropriate.
- **Maintainability** — prioritize readability and modularity over premature optimization.
