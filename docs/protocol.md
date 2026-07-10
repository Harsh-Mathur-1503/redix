# Protocol Specification (v0)

## Overview

The initial version of Redix uses a simple text-based protocol over TCP.

Each command is sent as a single line of text and is terminated by a newline character (`\n`).

Commands and arguments are separated by whitespace.

Examples:

```text
PING
SET name Harsh
GET name
DEL name
EXISTS name
```

---

## Protocol Rules

### Command Handling

Commands are case-insensitive.

The following requests are equivalent:

```text
PING
ping
PiNg
```

The server normalizes command names before execution.

### Keys and Values

Keys remain case-sensitive.

Values remain case-sensitive.

Example:

```text
SET Name Harsh
GET Name
```

is different from:

```text
GET name
```

### Whitespace Handling

Multiple spaces between arguments are allowed.

The following requests are equivalent:

```text
SET name Harsh
SET    name    Harsh
```

Leading and trailing whitespace should be ignored.

The following request is valid:

```text
   GET name
```

### Request and Response Format

* Each request occupies a single line.
* Each response occupies a single line.
* Commands are separated using whitespace.
* Keys and values must not contain spaces in v0.

---

## Supported Commands

### PING

Checks whether the server is alive.

Request:

```text
PING
```

Response:

```text
PONG
```

---

### SET

Stores a value associated with a key.

Request:

```text
SET <key> <value>
```

Example:

```text
SET name Harsh
```

Response:

```text
OK
```

Notes:

* Existing keys are overwritten.
* Keys and values cannot contain spaces in v0.

---

### GET

Retrieves the value associated with a key.

Request:

```text
GET <key>
```

Example:

```text
GET name
```

Response:

```text
Harsh
```

If the key does not exist:

```text
NOT_FOUND
```

Note:

Unlike Redis, Redix v0 returns `NOT_FOUND` for missing keys to keep responses human-readable during development.

---

### DEL

Deletes a key from the store.

Request:

```text
DEL <key>
```

Example:

```text
DEL name
```

If the key exists:

```text
OK
```

If the key does not exist:

```text
NOT_FOUND
```

---

### EXISTS

Checks whether a key exists.

Request:

```text
EXISTS <key>
```

Example:

```text
EXISTS name
```

Response:

```text
1
```

If the key does not exist:

```text
0
```

---

## Error Handling

All protocol errors must begin with the prefix:

```text
ERR
```

### Empty Command

Request:

```text
```

Response:

```text
ERR empty command
```

---

### Missing Arguments

Request:

```text
SET key
```

Response:

```text
ERR wrong number of arguments
```

---

### Too Many Arguments

Request:

```text
SET key value extra
```

Response:

```text
ERR wrong number of arguments
```

---

### Missing Key

Request:

```text
GET
```

Response:

```text
ERR wrong number of arguments
```

---

### Unknown Command

Request:

```text
UNKNOWN key
```

Response:

```text
ERR unknown command
```

---

## Design Constraints (v0)

The following limitations are intentionally accepted for the first version:

* Keys cannot contain spaces.
* Values cannot contain spaces.
* Data is stored entirely in memory.
* No persistence is provided.
* No authentication is provided.
* No replication is provided.
* No concurrency guarantees are provided.
* Only basic key-value commands are supported.

These limitations will be revisited in future protocol versions.

---

## Future Evolution

Planned protocol improvements:

1. RESP (Redis Serialization Protocol) support.
2. Quoted string values.
3. Binary-safe values.
4. Pipelining support.
5. Bulk responses.
6. Persistence commands.
7. Replication support.
8. Additional data structures.

---

## References

* Redis Documentation
* Redis Protocol Specification
* Beej's Guide to Network Programming
* cppreference
