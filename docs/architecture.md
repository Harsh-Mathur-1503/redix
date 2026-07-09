# Redix - Architecture Overview

## Goal

Redix is a Redis-inspired in-memory key-value store implemented in C++20.

The primary objective of this project is to explore the core concepts behind high-performance backend systems, including networking, concurrency, storage engines, persistence, and distributed systems design.

Redix will be developed incrementally. The initial version will focus on a single-node, single-threaded architecture that supports basic key-value operations over TCP using a simple text-based command protocol.

Future versions will introduce persistence, multithreading, RESP protocol support, replication, and performance optimizations.

## Initial Commands

Redix v0 will support a small command set focused on string key-value operations:

| Command | Arguments | Behavior |
|---|---:|---|
| `SET` | `key value` | Stores or replaces the value for `key`. |
| `GET` | `key` | Returns the value for `key`, or a null/not-found response. |
| `DEL` | `key` | Removes `key` if it exists. |
| `EXISTS` | `key` | Returns whether `key` exists. |
| `PING` | none | Returns a simple health-check response. |

## Non-Goals for v0

## High-Level Architecture

## Module Responsibilities

## Protocol Design

## Threading Model

## Persistence Plan

## Future Work