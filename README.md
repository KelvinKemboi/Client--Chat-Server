# TCP Chat - Client & Server

A multi-client TCP chat application written in C++ using POSIX sockets. A central server accepts connections from multiple clients and broadcasts messages between them.

---

## Project structure

```
.
├── server.cpp    - accepts connections, broadcasts messages to all clients
└── client.cpp    - connects to server, sends and receives messages
```

---

## Requirements

- Linux or macOS
- g++ with C++20 support

---

## Build

```bash
g++ -std=c++20 server.cpp -o server
g++ -std=c++20 client.cpp -o client
```

---

## Run

**Start the server first:**
```bash
./server
```
```
Server is listening on port: 8000
```

**Then connect clients in separate terminals:**
```bash
./client
```
```
Connected to server
Say something to the server: 
```

Open as many client terminals as you want. Messages typed in one client appear in all others.

---

## How it works

### Server

#### 1. Socket setup
```
socket() → bind() → listen() → accept() loop
```
A TCP socket is created, bound to port `8000`, and set to accept queued connections.

#### 2. Thread per client
Each time `accept()` returns a new client fd, a detached `std::thread` is spawned to handle that client independently. The main thread immediately loops back to `accept()`.

```cpp
thread t(eachClient, client);
t.detach();
```

#### 3. Client registry
Connected client file descriptors are stored in a shared `vector<int>`. A `std::mutex` protects every read and write to prevent data races between threads.

```cpp
vector<int> clients;
mutex clients_mut;
```

#### 4. Broadcast
When a message arrives from a client it is prefixed with the sender's fd and forwarded to every other connected client:

```cpp
string response = "Client " + to_string(client) + ": " + s;
broadcast(response, client);  // skips the sender
```

#### 5. Disconnect handling
`recv()` returns `0` on clean disconnect and `-1` on error. The client fd is closed and removed from the registry using erase-remove:

```cpp
clients.erase(remove(clients.begin(), clients.end(), client), clients.end());
```

---

### Client

#### 1. Connect to server
`inet_pton()` converts the string IP `"127.0.0.1"` to binary format. `connect()` establishes the TCP connection to the server on port `8000`.

```cpp
inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
connect(client_sock, (sockaddr*)&addr, sizeof(addr));
```

#### 2. Send and receive loop
The client alternates between two blocking calls:
- `getline(cin, message)` - waits for the user to type and press enter
- `recv()` - waits for a response from the server

```
user types → send() → recv() → print → repeat
```

> **Note:** because the client blocks on `getline` first, incoming messages from other clients only appear after you send something.
---

## Dependencies

All headers are standard POSIX.

| Header | Used in | Purpose |
|---|---|---|
| `<sys/socket.h>` | both | `socket()`, `bind()`, `accept()`, `recv()`, `send()`, `connect()` |
| `<netinet/in.h>` | both | `sockaddr_in`, `INADDR_ANY`, `htons()` |
| `<arpa/inet.h>` | client | `inet_pton()` - string IP to binary |
| `<unistd.h>` | both | `close()` |
| `<thread>` | server | `std::thread`, `detach()` |
| `<mutex>` | server | `std::mutex`, `std::lock_guard` |
| `<algorithm>` | server | `std::remove`|

---

## Limitations

- Clients are identified by file descriptor number, not a username
- Client send/receive loop is sequential - incoming messages only display after you send one
- No message framing - a long message may split across multiple `recv()` calls

---
