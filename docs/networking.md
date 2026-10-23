# Lab Share --- Networking

## Scope

This document describes the TCP networking visible in the uploaded Lab
Share source files. The application uses Winsock on Windows, IPv4
addresses, TCP sockets, and port `8080` in the current host/client
connection code.

The current code establishes a connection and then closes the sockets.
Actual file-data transfer using `send()` and `recv()` is not implemented
in the uploaded files.

## TCP in this project

TCP provides a reliable, ordered byte stream between two connected
sockets. The host waits for an incoming connection; the client initiates
a connection to the host's IPv4 address and port.

``` text
Host                                      Client
----                                      ------
WSAStartup()                              WSAStartup()
socket()                                  socket()
bind(port 8080)
listen()
accept()  <-------- TCP connection ------ connect(host IPv4, port 8080)
```

The host's listening socket and the accepted client socket are different
sockets:

-   **Listening socket:** bound to the local port and used to wait for
    incoming connections.
-   **Accepted client socket:** returned by `accept()` and used for
    communication with that connected client.
-   **Client socket:** created by the client and passed to `connect()`.

## Host workflow

The host setup is implemented in `host.c`.

### 1. Initialize Winsock

`initialize_winsock()` calls:

``` c
WSAStartup(MAKEWORD(2, 2), &wsaData);
```

A return value of `0` indicates success. The host handler returns an
error code if initialization fails.

### 2. Create a TCP socket

`create_server_socket()` calls:

``` c
socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
```

-   `AF_INET` selects IPv4.
-   `SOCK_STREAM` selects a stream socket.
-   `IPPROTO_TCP` selects TCP.

The function checks for `INVALID_SOCKET`.

### 3. Bind to a local port

`bind_server_socket()` prepares a `sockaddr_in` structure with the
requested port and:

``` c
service.sin_addr.s_addr = htonl(INADDR_ANY);
```

`INADDR_ANY` tells the host to bind on all available local IPv4
interfaces. `htons(port)` converts the port to network byte order.

The current `server_side_handler()` passes port `8080`.

### 4. Listen for connections

`start_listening()` calls:

``` c
listen(ListenSocket, SOMAXCONN);
```

This puts the socket into listening mode. The host then prints that it
is ready.

### 5. Accept a client

`accept_client()` calls:

``` c
accept(ListenSocket, NULL, NULL);
```

This is a blocking call by default: the host waits until a client
connects or the call fails. On success, `accept()` returns a **new
connected socket**. The listening socket remains a separate handle.

The current handler accepts one client, prints a connection message, and
then closes the accepted socket and listening socket. It does not yet
exchange file data.

## Client workflow

The client setup is implemented in `client.c`.

### 1. Initialize Winsock and create a socket

`initialize_winsock_client()` calls `WSAStartup()`. `client_session()`
creates an IPv4 TCP socket with `socket()` and returns `INVALID_SOCKET`
if creation fails.

### 2. Read and validate the host IPv4 address

`take_ip()` prompts the user for an IPv4 address and reads up to 15
characters into a static buffer.

`validate_ip_address()` calls:

``` c
inet_pton(AF_INET, ip, &address);
```

For IPv4 text, `inet_pton()` returns `1` when conversion succeeds, `0`
when the text is not a valid address, and `-1` on an error. In the
uploaded client code, the function returns `0` for valid input and `1`
for invalid input; `connect_to_host()` uses that convention to repeat
the prompt until the input passes validation.

### 3. Prepare the destination address

The client prepares a `sockaddr_in` structure:

-   `sin_family` is set to `AF_INET`.
-   `sin_port` is set to `htons(8080)`.
-   `sin_addr` should contain the binary IPv4 address converted from the
    accepted text using `inet_pton()`.

### 4. Connect

The client calls:

``` c
connect(client, (SOCKADDR *)&serverAddress, sizeof(serverAddress));
```

A return value of `SOCKET_ERROR` indicates failure. A return value of
`0` indicates that the connection succeeded.

`127.0.0.1` is the IPv4 loopback address; using it connects to a service
running on the same computer. To connect to a host on another computer,
the client must use an IPv4 address reachable over the network, and the
host's port must be reachable through any firewall or network rules.

## Socket cleanup and lifetime

The normal lifecycle is:

``` text
WSAStartup()
    |
socket creation
    |
connect / accept
    |
use connected socket
    |
closesocket() for each socket
    |
WSACleanup() when the application is done using Winsock
```

Important distinctions:

-   `closesocket()` closes one socket handle.
-   `WSACleanup()` releases Winsock resources for the process's use of
    Winsock.
-   Do not close the connected socket before the transfer is complete.
-   Do not call `WSACleanup()` while the application still needs Winsock
    sockets or operations.

The current client closes its socket after displaying the connection
result. The current host closes the accepted socket and listening socket
after accepting one client. This is sufficient for a basic connection
test, but not for a persistent file-sharing session.

## Current error handling

The uploaded code checks several important results:

-   `WSAStartup()` result.
-   `socket()` result against `INVALID_SOCKET`.
-   `bind()`, `listen()`, `accept()`, and `connect()` results.
-   IPv4 conversion result from `inet_pton()`.

Potential improvements for the next iteration:

1.  Keep the validated IP string in the scope where it is used and
    ensure the converted address is written into
    `serverAddress.sin_addr` before calling `connect()`.
2.  Capture `WSAGetLastError()` after Winsock failures so the user can
    see the reason for a failure.
3.  Make sure all failure paths close any socket already created and
    call `WSACleanup()` if Winsock initialization succeeded.
4.  Return or otherwise preserve the connected socket so the caller can
    use it for `send()` and `recv()`.
5.  Add a retry policy for connection failures separately from retrying
    malformed IPv4 input.

## Next networking milestone: file transfer

Before implementing the transfer routines, decide on the protocol. At
minimum, the receiver needs a reliable way to determine:

-   The filename or destination name.
-   The file size (or another defined end-of-transfer signal).
-   Whether the transfer succeeded or failed.

Remember that TCP is a byte stream, not a message protocol: a single
`send()` does not guarantee that the receiver gets the entire buffer in
one `recv()`. Transfer code should loop until the expected number of
bytes has been sent or received, and handle errors and disconnects.
