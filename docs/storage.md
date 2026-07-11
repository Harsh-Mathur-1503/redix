# Storage Layer Design (v0)

## Overview

The storage layer is responsible for managing key-value data inside Redix.

Its responsibilities are:

* Store values associated with keys.
* Retrieve values by key.
* Delete existing keys.
* Check whether a key exists.

The storage layer is intentionally independent of:

* Network communication
* Protocol parsing
* Command execution
* Client connections

This separation allows the storage implementation to evolve without affecting the parser, executor, or networking layers.

---

## Architectural Position

Current architecture:

```text
Client
   ↓
Parser
   ↓
Command
   ↓
Executor
   ↓
Storage
```

The storage layer never receives raw protocol commands.

For example:

```text
SET name harsh
```

is transformed by higher layers into:

```cpp
store.set("name", "harsh");
```

Similarly:

```text
GET name
```

becomes:

```cpp
store.get("name");
```

Storage operates only on keys and values.

---

## Design Principles

### Single Responsibility

The storage layer is responsible only for data management.

It must not:

* Parse commands
* Validate protocol syntax
* Generate protocol responses
* Manage network sockets

---

### Simplicity First

v0 prioritizes correctness and simplicity over advanced features.

The initial implementation will be entirely in-memory and use standard library containers.

---

### Replaceable Implementation

The rest of the system should depend on the storage interface, not on a specific implementation.

Future implementations may include:

* Persistent storage
* Replicated storage
* LSM-tree based storage
* Disk-backed storage

without requiring changes to the parser or executor.

---

## Data Model

v0 supports only string keys and string values.

Example:

```text
name  -> harsh
city  -> delhi
```

Both keys and values are case-sensitive.

Example:

```text
name
```

and

```text
Name
```

represent different keys.

---

## Storage API

The storage layer exposes four operations.

### Set

Stores a value associated with a key.

```cpp
void set(
    const std::string& key,
    const std::string& value);
```

Behavior:

* Inserts a new key if it does not exist.
* Overwrites the existing value if the key already exists.

Example:

```cpp
store.set("name", "harsh");
store.set("name", "john");
```

Final state:

```text
name -> john
```

---

### Get

Retrieves the value associated with a key.

```cpp
std::optional<std::string> get(
    const std::string& key) const;
```

Behavior:

* Returns the stored value if the key exists.
* Returns `std::nullopt` if the key does not exist.

Example:

```cpp
auto value = store.get("name");
```

Result:

```text
"harsh"
```

Missing key:

```cpp
auto value = store.get("unknown");
```

Result:

```text
nullopt
```

---

### Delete

Removes a key from storage.

```cpp
bool del(
    const std::string& key);
```

Behavior:

* Returns `true` if the key existed and was removed.
* Returns `false` if the key did not exist.

Example:

```cpp
store.del("name");
```

Result:

```text
true
```

---

### Exists

Checks whether a key exists.

```cpp
bool exists(
    const std::string& key) const;
```

Behavior:

* Returns `true` if the key exists.
* Returns `false` otherwise.

Example:

```cpp
store.exists("name");
```

Result:

```text
true
```

---

## Internal Data Structure

v0 uses:

```cpp
std::unordered_map<
    std::string,
    std::string
>
```

Reasoning:

* Average O(1) insert
* Average O(1) lookup
* Average O(1) delete
* Part of the standard library
* Simple implementation

Example:

```cpp
{
    {"name", "harsh"},
    {"city", "delhi"}
}
```

---

## Error Handling

The storage layer does not generate protocol responses.

Example:

Storage returns:

```cpp
std::nullopt
```

The executor decides to return:

```text
NOT_FOUND
```

Similarly:

Storage returns:

```cpp
false
```

The executor decides to return:

```text
NOT_FOUND
```

This separation keeps responsibilities clear.

---

## Thread Safety

v0 is not thread-safe.

Assumptions:

* Single process
* Single-threaded execution
* No concurrent clients

Thread safety may be introduced in future versions.



## Storage Assumptions

The storage layer treats keys and values as arbitrary strings.

Storage does not enforce protocol-level validation.

Examples:

- Keys are case-sensitive.
- Values are case-sensitive.
- Storage does not reject empty strings.
- Storage does not impose size limits in v0.

Validation of protocol rules is the responsibility of the parser and executor layers.

---

## Non-Goals for v0

The storage layer will not support:

* Persistence
* Snapshots
* Replication
* Expiration (TTL)
* Transactions
* Eviction policies
* Compression
* Disk storage
* Concurrency control
* Multiple data types

These features are intentionally deferred to later versions.

---

## Future Evolution

Potential future enhancements:

### Persistence

Store data on disk and recover after restart.

### Expiration

Support automatic key expiration.

Example:

```text
SET session abc EX 60
```

### Replication

Synchronize data between multiple Redix instances.

### Additional Data Types

Support:

* Lists
* Sets
* Hashes
* Sorted sets

### Concurrent Access

Allow multiple client connections to safely access storage simultaneously.

---

## Summary

v0 storage is:

* In-memory
* String-only
* Single-threaded
* Based on `std::unordered_map`
* Independent of protocol parsing
* Independent of networking

The goal is to provide a simple, reliable foundation on which future persistence, replication, and concurrency features can be built.
