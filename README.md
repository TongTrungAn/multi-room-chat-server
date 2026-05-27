````markdown
# Multi-Room Chat Server
## System Programming Final Project

An IRC-style multi-room chat server implemented in C using TCP sockets, POSIX threads, and Linux system programming techniques.

---

# Table of Contents

1. Project Overview
2. Features
3. System Architecture
4. Team Members & Work Division
5. Project Structure
6. Technologies Used
7. Requirements
8. Installation & Setup
9. Build Instructions
10. Running the Application
11. Supported Commands
12. Internal Design
13. Thread Synchronization
14. Socket Lifecycle
15. GitHub Workflow
16. Testing Plan
17. Error Handling
18. Security Considerations
19. Future Improvements
20. Demo Scenario
21. Screenshots
22. References

---

# 1. Project Overview

The Multi-Room Chat Server is a concurrent TCP-based communication system inspired by IRC (Internet Relay Chat).

The system allows multiple clients to:
- connect simultaneously
- join chat rooms
- communicate with users in the same room
- send private messages
- manage nicknames

The server uses:
- POSIX sockets
- pthreads
- mutex synchronization
- Linux networking APIs

The project demonstrates:
- system programming
- concurrent programming
- socket programming
- synchronization
- client-server architecture

---

# 2. Features

## Core Features

- Multi-client TCP server
- One thread per client
- Multiple chat rooms
- Room broadcasting
- Private messaging
- User nickname system
- Real-time communication
- Graceful disconnection handling

---

## Supported Commands

| Command | Description |
|----------|-------------|
| /nick <name> | Change nickname |
| /join <room> | Join a chat room |
| /leave | Leave current room |
| /list | List available rooms |
| /who | Show users in room |
| /msg <user> <text> | Private message |
| /quit | Disconnect |

---

# 3. System Architecture

```text
                   +----------------+
                   |    CLIENTS     |
                   +----------------+
                          |
                          |
                    TCP CONNECTION
                          |
                          v

+--------------------------------------------------+
|                    SERVER                        |
|--------------------------------------------------|
|                                                  |
|  Connection Manager                              |
|  Thread Manager                                  |
|  Command Parser                                  |
|  Room Manager                                    |
|  User Registry                                   |
|  Messaging System                                |
|                                                  |
|  Shared Resources (Mutex Protected)              |
|                                                  |
+--------------------------------------------------+
````

---

# 4. Team Members & Work Division

| Member   | Main Responsibility    | Assigned Modules           |
| -------- | ---------------------- | -------------------------- |
| Vu Xuan Bac | Core Networking        | server, threading, sockets |
| Tong Trung An | Room & Messaging       | rooms, broadcast, PM       |
| Do Huy Hieu | Client & Documentation | client UI, parser, report  |

---

## Vu Xuan Bac — Core Server & Networking

### Responsibilities

* TCP socket server
* Multi-threading
* Connection handling
* Mutex synchronization
* Signal handling

### Assigned Files

```text
server.c
connection.c
thread_manager.c
network_utils.c
```

### Tasks

* Implement socket lifecycle
* Handle client connections
* Create client threads
* Synchronize shared resources
* Handle disconnects safely

---

## Tong Trung An — Room & Messaging System

### Responsibilities

* Room management
* Broadcast system
* Private messaging
* User registry

### Assigned Files

```text
room_manager.c
user_manager.c
message.c
registry.c
```

### Tasks

* Implement room join/leave
* Broadcast messages
* Handle /msg command
* Track room membership
* Implement user lookup

---

## Do Huy Hieu — Client & Documentation

### Responsibilities

* Client application
* Command parser
* Testing
* Documentation

### Assigned Files

```text
client.c
parser.c
test_script.sh
README.md
report/
```

### Tasks

* Build interactive client
* Parse commands
* Display messages
* Write README and report
* Create testing scripts

---

# 5. Project Structure

```text
multi-room-chat-server/
│
├── src/
│   ├── server/
│   │   ├── server.c
│   │   ├── connection.c
│   │   ├── thread_manager.c
│   │   ├── room_manager.c
│   │   ├── user_manager.c
│   │   └── message.c
│   │
│   ├── client/
│   │   ├── client.c
│   │   └── parser.c
│   │
│   └── shared/
│       ├── protocol.h
│       ├── utils.c
│       └── logger.c
│
├── include/
│   ├── server.h
│   ├── room.h
│   ├── user.h
│   ├── message.h
│   └── protocol.h
│
├── tests/
│   ├── stress_test.sh
│   └── multi_client_test.sh
│
├── docs/
│   ├── screenshots/
│   └── report.pdf
│
├── Makefile
├── README.md
└── .gitignore
```

---

# 6. Technologies Used

| Component        | Technology       |
| ---------------- | ---------------- |
| Language         | C                |
| Networking       | POSIX Socket API |
| Threading        | pthread          |
| Synchronization  | pthread mutex    |
| Operating System | Linux            |
| Compiler         | GCC              |
| Version Control  | Git & GitHub     |

---

# 7. Requirements

## Software Requirements

* Ubuntu Linux
* GCC Compiler
* Make
* Git

---

## Install Dependencies

```bash
sudo apt update
sudo apt install build-essential git
```

---

# 8. Installation & Setup

## Clone Repository

```bash
git clone https://github.com/yourusername/multi-room-chat-server.git
```

---

## Enter Project Directory

```bash
cd multi-room-chat-server
```

---

# 9. Build Instructions

## Compile Server and Client

```bash
make
```

---

## Clean Build Files

```bash
make clean
```

---

# 10. Running the Application

## Start Server

```bash
./server 8080
```

---

## Start Client

```bash
./client 127.0.0.1 8080
```

---

## Open Multiple Clients

Use multiple terminal windows.

---

# 11. Supported Commands

## Change Nickname

```text
/nick alice
```

---

## Join Room

```text
/join general
```

---

## Leave Room

```text
/leave
```

---

## List Rooms

```text
/list
```

---

## Show Users in Room

```text
/who
```

---

## Private Message

```text
/msg bob hello
```

---

## Disconnect

```text
/quit
```

---

# 12. Internal Design

## Client Structure

```c
typedef struct
{
    int socket;
    char nickname[32];
    char room[32];
    pthread_t thread;
} Client;
```

---

## Room Structure

```c
typedef struct
{
    char name[32];
    Client *clients[MAX_CLIENTS];
    int count;
} Room;
```

---

# 13. Thread Synchronization

Multiple threads access shared resources:

* room registry
* user registry
* client list

Mutex protection is required.

---

## Example

```c
pthread_mutex_lock(&clients_mutex);

/* modify shared data */

pthread_mutex_unlock(&clients_mutex);
```

---

# 14. Socket Lifecycle

## Server Lifecycle

```text
socket()
bind()
listen()
accept()
recv()
send()
close()
```

---

## Client Lifecycle

```text
socket()
connect()
send()
recv()
close()
```

---

# 15. GitHub Workflow

## Branch Structure

```text
main
develop
member1-network
member2-room
member3-client
```

---

## Development Workflow

### Pull latest changes

```bash
git checkout develop
git pull origin develop
```

---

### Switch branch

```bash
git checkout member1-network
```

---

### Add changes

```bash
git add .
```

---

### Commit

```bash
git commit -m "Implement room broadcast system"
```

---

### Push

```bash
git push origin member1-network
```

---

### Create Pull Request

Open Pull Request on GitHub before merging.

---

# 16. Testing Plan

## Functional Testing

* Multiple clients
* Room creation
* Room joining
* Private messaging

---

## Concurrency Testing

* Simultaneous users
* Multiple broadcasts
* Concurrent joins/leaves

---

## Stress Testing

* Many clients connected
* Repeated messaging
* Rapid connect/disconnect

---

## Failure Testing

* Unexpected disconnect
* Broken sockets
* Invalid commands

---

# 17. Error Handling

## Common Errors Handled

| Error              | Solution           |
| ------------------ | ------------------ |
| Broken socket      | detect recv() <= 0 |
| SIGPIPE            | ignore signal      |
| Invalid command    | parser validation  |
| Duplicate nickname | reject request     |

---

## SIGPIPE Handling

```c
signal(SIGPIPE, SIG_IGN);
```

---

# 18. Security Considerations

## Input Validation

All user input is validated before processing.

---

## Safe Functions

Use:

```c
snprintf()
strncpy()
```

instead of unsafe functions like:

```c
strcpy()
sprintf()
```

---

## Buffer Overflow Prevention

All buffers use fixed maximum lengths.

---

# 19. Future Improvements

Possible future upgrades:

* epoll()
* thread pool
* authentication system
* SQLite integration
* TLS encryption
* ncurses interface
* chat history persistence
* file transfer
* admin commands

---

# 20. Demo Scenario

## User A

```text
/nick alice
/join general
hello everyone
```

---

## User B

```text
/nick bob
/join general
```

---

## Output

```text
[alice]: hello everyone
```

---

## Private Message Example

```text
/msg alice hi
```

---

# 21. Screenshots

## Server

```text
[SERVER] Listening on port 8080...
[NEW CONNECTION] Client connected
```

---

## Client

```text
Connected to server

/join general
[nick]: hello
```

---

# 22. References

## Documentation

* Beej's Guide to Network Programming
* Linux man pages
* POSIX Threads documentation

---

## APIs Used

* socket()
* bind()
* listen()
* accept()
* connect()
* send()
* recv()
* pthread_create()
* pthread_mutex_lock()

---

# Course Information

Course: System Programming
Project: Multi-Room Chat Server
Platform: Linux Ubuntu
Language: C Programming

---

# License

This project is developed for educational purposes.

```
```
