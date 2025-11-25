# SocketDemo

SocketDemo is an event-driven client/server demonstration written in C++. \
It serves as a simple example of Unix socket programming, and uses the `libev` event loop library to handle asynchronous I/O.

This project was started to explore the fundamentals of creating efficient network services in C++, and was mainly a stepping stone that let me learn enough about network programming to prepare for my larger ongoing project, [Beholder](https://github.com/xwilson03/Beholder).


## Project Structure

### Library Overview

The main components of this library are the `Client` and `Server` classes, with a `Receiver` class for supporting `Server` operation.

- `Client`:
  - RAII wrapper for a socket that connects to a `Server`.
  - Exposes `connect()` and `send()` functions for connecting and sending character data to a `Server`.
- `Server`:
  - RAII wrapper for a socket that listens for incoming `Clients`.
  - Creates, tracks, and deletes `Receiver` objects for each incoming connection.
    - Uses a `std::unordered_map` to index `Receivers` by their file descriptor.
    - Provides `Receivers` with a callback to queue their deletion on client disconnection.
  - Provides a shutdown handle via dependency injection of an `ev::async` watcher.
- `Receiver`:
  - RAII wrapper for a socket that receives incoming data from `Clients`.
  - Logs incoming data and self-flags for deletion on `Client` disconnect.

### Example Applications

This repository contains three example applications.
- `server_example`: Hosts a server on `0.0.0.0:65535`.
- `client_example`: Connects and sends a message to the `Server` at `127.0.0.1:65535`.
- `stress_test`: Hosts a `Server` in one thread and spawns a large number of `Clients` in another to test concurrent connection handling.


## Design Choices

### Event-Driven Architecture

SocketDemo is built on an event-driven model rather than a traditional thread-per-connection or thread-per-request model. This offers several advantages to us:

- **Scalability**: An event-driven server can handle many concurrent connections even with a small number of threads, as it doesn't dedicate a thread to each client. This makes it much more efficient since we get to avoid a lot of context-switching overhead.
- **Resource Efficiency**: Resources are only consumed when an event occurs (like incoming data), which prevents us from wasting CPU time on idle connections.

### `libev`

`libev` was chosen because it offers us a simple and performant abstraction away from low-level system calls like `select()`, `poll()`, or `epoll()`. Here's a bit more on that:

- **High Performance**: `libev` automatically uses the most efficient I/O demultiplexing mechanism available on the host OS. While we're locked to using Unix sockets, this still affords us a bit of extra portability and avoids the added complexity of writing raw `select`/`poll`/`epoll`/`kqueue`/event ports code while still offering us great performance benefits.
- **Simplicity**: It abstracts away much of the complexity with managing file descriptors and event sources. By using watcher objects (`ev::io`, `ev::timer`, etc.), the server logic is greatly simplified, making the code cleaner and easier to maintain compared to manual event loop management.
- **Stability**: `libev` is a mature, widely-used, and battle-tested library, providing a reliable foundation for the project.


## Building and Running

### Prerequisites

- Any compiler with C++17 support
- cmake >=3.21
- libev-dev
- libspdlog-dev

### Setup

1.  **Clone:**
    ```sh
    git clone git@github.com:xwilson03/SocketDemo.git
    cd SocketDemo
    ```

2.  **Build:**
    ```sh
    ./build.sh
    ```

3.  **Run:**
    ```sh
    cd .install/bin

    # Run the stress test
    stress_test

    # Or, run the server and client separately.
    ./server_example &
    for i in {1..1024}; do ./client_example; done
    pkill server_example
    ```