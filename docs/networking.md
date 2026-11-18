# Lab Share — Networking Architecture

## 1. Scope

This document describes the current TCP networking implementation of Lab Share, based on the uploaded source files.

Lab Share is a Windows-based client-server application written in C using Winsock 2.2. It uses IPv4 addressing and TCP sockets to establish a direct connection between two computers.

The current networking implementation supports:

- TCP host and client connections.
- Persistent connections during a session.
- Bidirectional text messaging.
- Dedicated receiver threads on both sides.
- Newline-delimited message framing.
- Peer-disconnection detection.
- Socket lifecycle management and cleanup.

File transfer is part of the intended application functionality, but the actual file transmission and reception routines are not implemented yet.

## 2. Networking Stack

| Layer / Component | Implementation |
|---|---|
| Operating System | Windows |
| Networking API | Winsock 2.2 |
| Address Family | IPv4 (`AF_INET`) |
| Transport Protocol | TCP (`IPPROTO_TCP`) |
| Socket Type | Stream (`SOCK_STREAM`) |
| Default Port | 8080 |
| Message Handling | `send()` and `recv()` |
| Concurrency | Windows threads using `_beginthreadex()` |
| Message Delimiter | Newline (`\n`) |

### TCP in Lab Share

TCP provides a reliable, ordered byte stream between the host and client.

The host listens for incoming connections, while the client initiates a connection using the host's IPv4 address and port.

Once the connection is established, both sides retain their connected sockets and use them for bidirectional communication.

```text
             HOST                              CLIENT
              |                                  |
       WSAStartup()                        WSAStartup()
              |                                  |
       socket()                            socket()
              |                                  |
       bind(port 8080)                           |
              |                                  |
       listen()                                  |
              |                                  |
       accept()  <-------- TCP -------------- connect()
              |                                  |
              +---------- CONNECTION -----------+
              |                                  |
       Receiver Thread                    Receiver Thread
              |                                  |
       send_message()  ----------------> receive_message()
              |                                  |
       receive_message() <---------------- send_message()
              |                                  |
              +---------- SESSION ---------------+
                             |
                       DISCONNECTION
                             |
                    Socket Cleanup
```

## 3. Network Architecture

Lab Share follows a client-server architecture with bidirectional communication after connection establishment.

There are two primary networking components:

- **Host:** Creates a listening socket, waits for an incoming client, and manages the established session.
- **Client:** Creates a TCP socket, validates the host's IPv4 address, and initiates the connection.

After establishing a connection, both sides operate similarly. Each side can send messages while independently receiving messages through a dedicated receiver thread.

### 3.1 Host-side architecture

The host-side networking implementation is primarily located in `host.c`.

The `HostConnection` structure maintains the two sockets required by the host:

- `client_socket`: The connected socket used for communication with the client.
- `listen_socket`: The listening socket used to accept incoming connections.

These sockets have separate responsibilities and lifetimes.

The listening socket remains responsible for accepting connections, while the accepted client socket handles communication during the session.

### 3.2 Client-side architecture

The client-side networking implementation is located in `client.c`.

The client performs the following operations:

1. Initializes Winsock.
2. Creates an IPv4 TCP socket.
3. Requests the host's IPv4 address.
4. Validates the supplied address.
5. Converts the IPv4 address into its binary network representation.
6. Attempts to establish a TCP connection.
7. Returns the connected socket to the session handler.

The connected socket is retained throughout the session rather than being closed immediately after connection establishment.

## 4. Host Workflow

### 4.1 Winsock initialization

The host initializes Winsock through `initialize_winsock()`.

It uses:

`WSAStartup(MAKEWORD(2, 2), &wsaData)`

A return value of `0` indicates successful initialization.

If initialization fails, the host cannot proceed with socket operations.

### 4.2 TCP socket creation

The host creates its listening socket through `create_server_socket()`.

The socket uses:

- `AF_INET`: IPv4 addressing.
- `SOCK_STREAM`: TCP stream communication.
- `IPPROTO_TCP`: TCP protocol.

The function checks whether socket creation returns `INVALID_SOCKET`.

### 4.3 Binding

The `bind_server_socket()` function prepares a `sockaddr_in` structure containing the address and port information.

The host uses:

- `AF_INET` as the address family.
- `htons(8080)` as the port.
- `INADDR_ANY` as the local IPv4 binding address.

`INADDR_ANY` allows the host to listen on all available local IPv4 interfaces.

The current implementation uses port `8080`.

### 4.4 Listening

The `start_listening()` function calls:

`listen(ListenSocket, SOMAXCONN)`

The host socket enters listening mode and waits for incoming connection requests.

### 4.5 Accepting a client

The `accept_client()` function calls:

`accept(ListenSocket, NULL, NULL)`

This is a blocking operation by default.

The host waits until a client connects or an error occurs.

When a connection is accepted, Windows returns a new connected socket.

The host stores this socket in `connection.client_socket`, while retaining the original listening socket separately.

### 4.6 Local IPv4 discovery

Before accepting a client, the host retrieves a local IPv4 address using `get_local_ip()` from `utils.c`.

The function:

1. Retrieves the local hostname using `gethostname()`.
2. Resolves its network addresses using `getaddrinfo()`.
3. Restricts the results to IPv4.
4. Iterates through the available addresses.
5. Skips the loopback address.
6. Converts the selected address into readable IPv4 text using `inet_ntop()`.

The resulting address is displayed so another device on the network can use it to connect.

Note that this function selects the first non-loopback address returned. It does not guarantee that the selected interface is reachable by every client.

### 4.7 Room code generation

The host generates a four-digit room code using `code_generator()` from `utils.c`.

The code is displayed before the host starts waiting for a connection.

**Current limitation:** The room code is not yet part of the TCP connection protocol. It is generated and displayed, but the client does not submit or verify it during connection establishment.

## 5. Client Workflow

### 5.1 Winsock initialization

The client initializes Winsock through `initialize_winsock_client()`.

It uses Winsock version 2.2.

If initialization fails, the client-side handler returns `INVALID_SOCKET`.

### 5.2 Socket creation

The `client_session()` function creates an IPv4 TCP socket.

The socket is created using:

`socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)`

The resulting socket is returned to the connection handler.

### 5.3 Host IPv4 input

The client requests the host's IPv4 address through `take_ip()`.

The input is stored in a static character buffer with a maximum IPv4 text length of 15 characters.

The `get_host_ip()` function manages the input and validation process.

The user can enter `1` to return to the previous menu.

### 5.4 IPv4 validation

The `validate_ip_address()` function in `validate.c` uses:

`inet_pton(AF_INET, ip, &address)`

The function returns:

- `0` when the IPv4 address is valid.
- `1` when the address is invalid.

The client repeats the input process until a valid IPv4 address is provided or the user exits.

### 5.5 Preparing the destination address

The client prepares a `sockaddr_in` structure containing:

- `sin_family`: `AF_INET`.
- `sin_port`: `htons(8080)`.
- `sin_addr`: The converted binary IPv4 address.

The address conversion is performed using `inet_pton()`.

### 5.6 Establishing the connection

The `connect_to_host()` function calls:

`connect(client, (SOCKADDR *)&server_address, sizeof(server_address))`

A return value of `0` indicates a successful connection.

`SOCKET_ERROR` indicates a connection failure.

If the connection fails, the client closes its socket and releases Winsock resources.

If successful, the connected socket is returned to `client_side_menu()` for the remainder of the session.

## 6. Bidirectional Communication

After establishing a connection, both the host and client start a dedicated receiver thread.

This allows incoming messages to be processed independently of the main thread, which handles user input and outgoing messages.

### 6.1 Receiver threads

The application creates receiver threads using:

`_beginthreadex()`

The host uses:

`receive_handler()`

The client uses:

`client_receive_handler()`

Both functions continuously call `receive_message()` until receiving fails or the peer disconnects.

The receiver thread receives the connected socket through its argument.

This creates two independent execution paths:

| Main thread | Receiver thread |
|---|---|
| Handles user input | Waits for incoming messages |
| Displays session menu | Processes received messages |
| Sends outgoing messages | Detects peer disconnection |
| Handles session exit | Terminates on receive failure |

### 6.2 Message transmission

Message transmission is implemented in `transfer.c` through `send_message()`.

The function uses the Winsock `send()` operation.

The current message format is:

`MESSAGE CONTENT + NEWLINE`

For example:

```text
Hello from Lab Share!\n
```

The newline character acts as the message delimiter.

The transmission function maintains a counter for the total number of bytes sent.

It continues sending until the entire message, including its terminating newline, has been transmitted.

This handles situations where a single `send()` operation transmits fewer bytes than requested.

The function returns:

- `0` when the complete message is transmitted.
- `1` when transmission fails or the connection is closed.

### 6.3 Message reception

Message reception is implemented through `receive_message()` in `transfer.c`.

Unlike transmission, the current receiver processes incoming data one byte at a time.

The function:

1. Validates the destination buffer.
2. Receives one byte using `recv()`.
3. Checks for socket errors.
4. Checks whether the peer has disconnected.
5. Stops reading when a newline is encountered.
6. Adds a null terminator to the received message.
7. Displays the message.

The receiver uses the newline character to determine where a message ends.

The current implementation supports messages up to the available buffer capacity.

The receiver returns:

| Return value | Meaning |
|---|---|
| `0` | Message received successfully |
| `1` | Receive error or invalid buffer |
| `2` | Peer disconnected |
| `3` | Message exceeded buffer capacity |

Both host and client receiver threads terminate their receive loops when the function returns a non-zero value.

### 6.4 Current message protocol

Lab Share currently uses a simple newline-delimited text protocol.

```text
Sender                              Receiver
  |                                    |
  |------ Message bytes -------------->|
  |                                    |
  |------ Newline delimiter ---------->|
  |                                    |
  |                              Read bytes
  |                                    |
  |                              Detect newline
  |                                    |
  |                              Null terminate
  |                                    |
  |                              Display message
  |                                    |
```

TCP is a byte-stream protocol, so it does not preserve application-level message boundaries.

The newline delimiter provides the framing mechanism used by the current implementation.

**Current limitation:** The receiver processes one byte at a time and treats a full buffer without a newline as an oversized message. It does not implement a persistent receive buffer for reconstructing arbitrarily fragmented or oversized application messages.

## 7. Session Management and Disconnection Handling

The host and client maintain their TCP connections until the user exits the session or the peer disconnects.

Both sides monitor two handles using `WaitForMultipleObjects()`:

1. The receiver thread handle.
2. The standard input handle.

This allows the main thread to react to either receiver-thread termination or console input.

### 7.1 Peer disconnection

When the peer closes its connection, `recv()` returns `0`.

The `receive_message()` function identifies this condition and returns `2`.

The receiver thread then terminates.

The main session loop detects receiver-thread termination through `WaitForMultipleObjects()` and displays the disconnection message.

The user is then able to return to the main application menu.

### 7.2 User-initiated session exit

The session menu provides an option to return to the main menu.

When the user selects the exit option:

1. The session loop terminates.
2. The application initiates socket shutdown using `shutdown()`.
3. The connected socket is closed using `closesocket()`.
4. The receiver thread is waited on.
5. The receiver thread handle is closed.
6. Winsock resources are released using `WSACleanup()`.

The host additionally closes its listening socket.

### 7.3 Socket lifecycle

The current host lifecycle:

```text
WSAStartup()
     |
Create listening socket
     |
Bind to port 8080
     |
Listen
     |
Accept client
     |
Start receiver thread
     |
Bidirectional communication
     |
Session termination
     |
shutdown(client_socket)
     |
closesocket(client_socket)
     |
Wait for receiver thread
     |
Close thread handle
     |
closesocket(listen_socket)
     |
WSACleanup()
```

The current client lifecycle:

```text
WSAStartup()
     |
Create client socket
     |
Get host IPv4 address
     |
Validate IPv4 address
     |
connect()
     |
Start receiver thread
     |
Bidirectional communication
     |
Session termination
     |
shutdown(client_socket)
     |
closesocket(client_socket)
     |
Wait for receiver thread
     |
Close thread handle
     |
WSACleanup()
```

## 8. Source File Responsibilities

The networking implementation is separated across multiple source files.

| File | Responsibility |
|---|---|
| `main.c` | Application entry point, main menu and navigation |
| `host.c` | Host initialization, socket creation, binding, listening, accepting and host session management |
| `client.c` | Client initialization, IPv4 input, validation flow, connection establishment and client session management |
| `transfer.c` | Message transmission and reception |
| `utils.c` | CLI screens, local IPv4 discovery, room-code generation and file selection |
| `validate.c` | Menu input and IPv4 validation |

### Module relationships

```text
                     main.c
                       |
             +---------+---------+
             |                   |
          host.c              client.c
             |                   |
             +---------+---------+
                       |
                    transfer.c
                       |
                 Winsock API
                       |
                   TCP/IP
```

Additional utility dependencies:

```text
host.c
  |
  +---- utils.c
  |       +---- get_local_ip()
  |       +---- code_generator()
  |       +---- CLI functions
  |
  +---- transfer.c
          +---- send_message()
          +---- receive_message()


client.c
  |
  +---- validate.c
  |       +---- validate_ip_address()
  |
  +---- utils.c
  |       +---- CLI functions
  |
  +---- transfer.c
          +---- send_message()
          +---- receive_message()
```

## 9. Current Implementation Limitations

The following functionality is not yet implemented or remains limited in the uploaded source files.

### 9.1 File transfer

The `send_file()` function in `transfer.c` currently returns `0` without performing any file operations.

Although `utils.c` contains a Windows file-selection function, selecting a file is not the same as transmitting it.

The following functionality is still required:

- File metadata transmission.
- Filename transmission.
- File-size communication.
- Binary file transmission.
- File reception and storage.
- Transfer completion verification.
- Transfer error handling.

### 9.2 Room-code integration

The host generates and displays a four-digit room code.

However, the code is not currently integrated into the client connection process.

There is no room-code exchange or verification in the existing networking protocol.

### 9.3 Configuration integration

The `settings()` function in `main.c` reads values from `../config/settings.txt`, including:

- Username.
- Port.
- Connection timeout.
- Maximum connections.

However, the networking functions currently use port `8080` directly.

The other configuration values are not integrated into the networking workflow shown in the uploaded source files.

### 9.4 Connection management

The current host accepts one client connection per hosting session.

There is no implemented multi-client session management or connection pool.

### 9.5 Message handling

The current messaging implementation uses newline-delimited messages and a fixed-size receive buffer.

It does not yet provide:

- A structured message header.
- Message types.
- Binary-safe message framing.
- Persistent buffering for oversized messages.
- Application-level delivery acknowledgements.

### 9.6 Error handling

The application checks important Winsock operations, including:

- `WSAStartup()`
- `socket()`
- `bind()`
- `listen()`
- `accept()`
- `connect()`
- `send()`
- `recv()`

The messaging functions also retrieve Winsock error codes through `WSAGetLastError()`.

More detailed error reporting and consistent handling of thread-creation and socket failures remain areas for improvement.

## 10. Next Networking Milestone: File Transfer

The next major networking milestone is implementing file transmission over the existing TCP connection.

The current messaging layer already provides bidirectional communication between the host and client.

File transfer will require an additional application-level protocol that distinguishes file metadata from file contents.

A proposed transfer sequence is:

```text
SENDER                              RECEIVER
  |                                    |
  |------ Transfer Request ---------->|
  |                                    |
  |------ File Metadata -------------->|
  |                                    |
  |------ File Data ----------------->|
  |------ File Data ----------------->|
  |------ File Data ----------------->|
  |                                    |
  |------ Transfer Complete ---------->|
  |                                    |
  |<----- Transfer Result -------------|
  |                                    |
```

The transfer protocol should define:

1. Transfer identification.
2. Filename and destination handling.
3. File size.
4. Binary data transmission.
5. Partial-send and partial-receive handling.
6. Transfer completion.
7. Failure and cancellation handling.

TCP provides an ordered byte stream, but it does not automatically preserve file-transfer boundaries.

The receiver must know exactly how much file data belongs to a transfer or use another explicitly defined framing mechanism.

## 11. Current Architecture Summary

Lab Share currently implements a two-peer, client-server TCP communication model.

The host initializes a listening socket and accepts an incoming connection. The client validates the host's IPv4 address and establishes a connection over port `8080`.

Once connected, both sides maintain their sockets throughout the session and use separate receiver threads for incoming messages.

The `transfer.c` module provides bidirectional text messaging through newline-delimited messages, with handling for partial sends, receive failures and peer disconnections.

The networking foundation is therefore established, and the next major development stage is implementing the actual file-transfer protocol on top of the existing TCP session.

---

**Documentation status:** Updated to match the uploaded source files.

**Implementation status:** TCP connection management and bidirectional text messaging implemented; file transfer pending.
