# Lab Share — Software Architecture

## 1. Overview

Lab Share is a Windows-based console application written in C. It is designed to allow users to establish direct connections between computers and exchange files without relying on external file-sharing services.

The application follows a client-server architecture built on Winsock and TCP/IP.

The host creates a listening socket and waits for an incoming connection, while the client connects using the host's IPv4 address.

Once connected, both sides maintain an active session and support bidirectional text communication through dedicated receiver threads.

The current implementation includes:

- Main application menu and navigation.
- Host-side TCP server initialization.
- Client-side TCP connection establishment.
- IPv4 address validation and local IP discovery.
- Persistent host-client sessions.
- Bidirectional text messaging.
- Concurrent message reception using Windows threads.
- Peer-disconnection detection.
- Socket shutdown and cleanup.
- Windows file-selection dialog.
- Basic settings-file reading.

Actual file transmission and reception are still under development. The existing `send_file()` function is currently a placeholder.

## 2. Architectural Overview

Lab Share uses a modular architecture in which application control, networking, communication, validation, and utility functionality are separated into different source files.

### High-level architecture

```text
                    LAB SHARE
                         |
                         |
                 Application Layer
                         |
                       main.c
                         |
                Main Menu / Navigation
                         |
             +-----------+-----------+
             |                       |
          Host Mode               Client Mode
             |                       |
           host.c                 client.c
             |                       |
             +-----------+-----------+
                         |
                  Communication Layer
                         |
                     transfer.c
                         |
                  Winsock 2.2 API
                         |
                       TCP/IP
                         |
                   Network Interface
```

Supporting modules provide shared functionality:

```text
                  Supporting Modules
                         |
              +----------+----------+
              |                     |
            utils.c              validate.c
              |                     |
       CLI / IP Discovery       Input / IPv4
       Room Code / File        Validation
          Selection
```

## 3. Project Structure

The following table describes the responsibilities visible in the uploaded source files.

| File | Responsibility |
|---|---|
| `main.c` | Application entry point, main menu, navigation and settings |
| `host.c` | Host initialization, TCP server setup, connection acceptance and host session management |
| `client.c` | Client initialization, IPv4 input, connection establishment and client session management |
| `transfer.c` | Bidirectional text transmission and reception; file-transfer placeholder |
| `utils.c` | CLI screens, local IPv4 discovery, room-code generation and Windows file selection |
| `validate.c` | Menu input validation and IPv4 address validation |

### Header files

The source files include several corresponding header files:

- `host.h`
- `client.h`
- `transfer.h`
- `utils.h`
- `validate.h`

These provide declarations used by the respective modules. Their complete contents and exact declaration structure were not included in the uploaded source set.

The complete repository directory structure and build configuration cannot be established from the uploaded files alone.

## 4. Application Layer

The application layer is responsible for controlling the program's execution and handling user navigation.

Its primary implementation is located in `main.c`.

### 4.1 Application entry point

The `main()` function acts as the entry point of Lab Share.

It continuously displays the main application menu and dispatches the user's selection to the appropriate functionality.

The application provides four primary options:

1. Host a Session.
2. Connect to a Session.
3. Settings.
4. Exit.

The application remains active until the user selects the exit option.

### 4.2 Menu handling

The `cli_interface_handler()` function manages the main menu interaction.

Its responsibilities include:

- Displaying the main menu.
- Reading user input.
- Validating menu input.
- Returning the selected option.

The main function uses the returned selection to determine which application component should execute.

### 4.3 Application execution flow

```text
                      main()
                         |
                         |
                cli_interface_handler()
                         |
                    User Selection
                         |
          +--------------+--------------+
          |              |              |
          v              v              v
       Option 1       Option 2        Option 3
          |              |              |
          v              v              v
    Host Session    Client Session   Settings
          |              |              |
          |              |              |
          +--------------+--------------+
                         |
                    Return to Menu
                         |
                    Option 4?
                         |
                    Exit Program
```

### 4.4 Settings management

The `settings()` function reads configuration values from:

`../config/settings.txt`

The values include:

- Username.
- Port.
- Connection timeout.
- Maximum connections.

The function currently displays the loaded values.

Although these settings are read successfully when the configuration file is available, the networking implementation does not yet consume them dynamically.

For example, the host and client currently use port `8080` directly.

## 5. Host Module

The host module is implemented in `host.c`.

It is responsible for preparing the application to accept an incoming TCP connection and managing the host side of an established session.

### 5.1 Host initialization

The host starts by executing `host_menu_handler()`.

The function:

1. Generates a four-digit room code.
2. Displays the waiting screen.
3. Initializes the host networking components.
4. Waits for an incoming client.
5. Starts a receiver thread after successful connection.
6. Enters the host session menu.

### 5.2 HostConnection structure

The host uses a custom structure named `HostConnection` to maintain its socket handles.

It contains:

| Member | Responsibility |
|---|---|
| `client_socket` | Connected socket used for communication with the client |
| `listen_socket` | Listening socket used to accept incoming connections |

Separating these sockets makes their responsibilities explicit.

The listening socket remains separate from the accepted client socket throughout the session.

### 5.3 Host networking functions

The host networking implementation is divided into smaller functions:

- `initialize_winsock()` — initializes Winsock.
- `create_server_socket()` — creates the IPv4 TCP listening socket.
- `bind_server_socket()` — binds the socket to port `8080`.
- `start_listening()` — places the socket into listening mode.
- `accept_client()` — accepts an incoming connection.
- `server_side_handler()` — coordinates the complete host initialization and connection process.

The `server_side_handler()` function returns a `HostConnection` structure containing the listening and connected sockets.

### 5.4 Host session management

After successfully accepting a client, the host starts a dedicated receiver thread using `_beginthreadex()`.

The main host thread handles user input and outgoing messages, while the receiver thread continuously waits for incoming messages.

The host menu currently displays:

- Send File.
- Back.

The menu interaction is implemented in `host_menu_handler()`.

Although the interface displays a file-sharing option, the actual file transmission functionality is not yet implemented.

## 6. Client Module

The client module is implemented in `client.c`.

It is responsible for establishing a TCP connection with an existing host and managing the client side of the session.

### 6.1 Client initialization

The client starts through `client_side_menu()`.

The function calls `client_side_handler()`, which coordinates:

1. Winsock initialization.
2. TCP socket creation.
3. Host IPv4 input.
4. IPv4 validation.
5. Connection establishment.
6. Returning the connected socket.

### 6.2 IPv4 input and validation

The client obtains the host's IPv4 address using `take_ip()`.

The `get_host_ip()` function manages the input and validation process.

The actual IPv4 validation is performed by `validate_ip_address()` from `validate.c`.

The client uses `inet_pton()` to validate the supplied address.

Invalid addresses cause the client to request another input.

### 6.3 Connection establishment

The `connect_to_host()` function prepares the destination address and attempts to establish a TCP connection.

The client uses:

- IPv4 addressing.
- TCP transport.
- Port `8080`.

Once the connection succeeds, the connected socket is returned to `client_side_menu()`.

Unlike the original connection-testing implementation, the current client maintains the connected socket throughout the session.

### 6.4 Client session management

After establishing a connection, the client creates a dedicated receiver thread through `_beginthreadex()`.

The main thread handles outgoing messages and user interaction, while the receiver thread handles incoming messages.

The client can therefore send and receive messages simultaneously during an active session.

## 7. Communication Layer

The communication layer is implemented in `transfer.c`.

It provides the functionality used by both the host and client to exchange text messages.

The module currently exposes three functions:

- `send_message()`
- `receive_message()`
- `send_file()`

The first two functions implement text communication. The file-transfer function remains a placeholder.

### 7.1 Message transmission

The `send_message()` function sends text through an established TCP socket.

It uses Winsock's `send()` operation.

The current message format uses a newline delimiter.

The function maintains a byte counter and continues sending until the complete message, including its newline terminator, has been transmitted.

It also handles partial sends and reports transmission errors.

### 7.2 Message reception

The `receive_message()` function receives incoming text using `recv()`.

It reads data one byte at a time until a newline is encountered.

The function checks for:

- Invalid buffers.
- Socket errors.
- Peer disconnection.
- Messages exceeding the available buffer capacity.

It returns a status value indicating whether the message was received successfully or whether an error occurred.

### 7.3 Bidirectional communication

Both host and client use the same communication module.

```text
                 HOST                  CLIENT
                  |                       |
             Main Thread             Main Thread
                  |                       |
                  |                       |
             send_message() --------> receive_message()
                  |                       |
                  |                       |
             receive_message() <----- send_message()
                  |                       |
             Receiver Thread         Receiver Thread
                  |                       |
                  +------ TCP Socket -----+
```

This shared communication module avoids maintaining separate message-transmission implementations for the host and client.

## 8. Threading Architecture

Lab Share uses Windows threading to handle incoming messages independently of the main application flow.

Both host and client create a receiver thread using `_beginthreadex()`.

### 8.1 Thread responsibilities

| Execution Context | Responsibility |
|---|---|
| Main thread | User input, menu handling and outgoing messages |
| Receiver thread | Incoming messages and peer-disconnection detection |

### 8.2 Host receiver thread

The host uses `receive_handler()`.

It continuously calls `receive_message()` using the connected client socket.

The receiver thread terminates when the communication function returns a non-zero status.

### 8.3 Client receiver thread

The client uses `client_receive_handler()`.

Its behavior is similar to the host receiver thread.

It continuously monitors the connected socket for incoming messages.

### 8.4 Thread coordination

Both session handlers use `WaitForMultipleObjects()` to monitor:

1. The receiver thread handle.
2. The standard input handle.

When the receiver thread terminates, the session displays the disconnection message and exits the session loop.

When the user selects the exit option, the application shuts down the socket and waits for the receiver thread to finish before releasing its thread handle.

## 9. Utility and Validation Modules

### 9.1 Utility module — `utils.c`

The utility module contains shared functionality used across the application.

Its current responsibilities include:

**CLI functionality**
- `main_menu()`
- `cli_options()`
- `waiting_for_connection()`
- `error_message()`
- `disconnect_message()`

**Network-related utilities**
- `get_local_ip()`
- `code_generator()`

**File selection**
- `file_selector()`

The `get_local_ip()` function retrieves a non-loopback IPv4 address associated with the host machine.

The `code_generator()` function generates a four-digit random room code.

The `file_selector()` function uses the Windows common file dialog API to allow the user to select an existing file.

It returns an allocated copy of the selected file path, which must be released by the caller.

### 9.2 Validation module — `validate.c`

The validation module handles input validation.

It currently contains:

- `input_validater()` — checks menu input for invalid numeric input.
- `validate_ip_address()` — validates IPv4 address strings.

Separating validation from the networking implementation keeps the validation functionality reusable.

## 10. File Selection and Transfer Architecture

Lab Share includes the initial components required for file selection, but the complete file-transfer workflow has not yet been implemented.

### 10.1 File selection

The `file_selector()` function in `utils.c` uses the Windows API:

`GetOpenFileNameA()`

It opens a Windows file-selection dialog and returns the selected file path.

The selected path is allocated dynamically, so the caller is responsible for releasing the allocated memory.

### 10.2 File transmission

The `send_file()` function in `transfer.c` currently returns `0` without performing any transmission.

There is no implemented file-data protocol, file metadata exchange, or binary transmission in the uploaded source.

### 10.3 Planned file-transfer workflow

The intended architecture is to build file transfer on top of the existing TCP session.

```text
                  File Selection
                        |
                        |
                  Selected Path
                        |
                        |
                  File Processing
                        |
                        |
                 Transfer Protocol
                        |
                        |
                  TCP Transmission
                        |
                        |
                  File Reception
                        |
                        |
                 Destination Storage
```

The following components will need to be implemented:

- File metadata handling.
- Filename transmission.
- File-size communication.
- Binary data transmission.
- File reception.
- Destination file creation.
- Transfer completion verification.
- Error and cancellation handling.

This represents planned functionality rather than the current implementation.

## 11. Resource Management

The application manages Winsock resources, socket handles and receiver-thread handles.

### Host cleanup

The host performs the following operations when leaving a session:

1. Initiates socket shutdown using `shutdown()`.
2. Closes the connected client socket.
3. Waits for the receiver thread.
4. Closes the receiver thread handle.
5. Closes the listening socket.
6. Calls `WSACleanup()`.

### Client cleanup

The client performs:

1. Socket shutdown.
2. Connected socket closure.
3. Receiver-thread synchronization.
4. Receiver-thread handle closure.
5. Winsock cleanup.

Failure paths also attempt to release resources created during initialization.

The listening socket and connected socket have separate ownership and cleanup responsibilities.

## 12. Current Architecture Status

| Component | Current Status |
|---|---|
| Main menu and navigation | Implemented |
| Host initialization | Implemented |
| Client initialization | Implemented |
| IPv4 validation | Implemented |
| Local IPv4 discovery | Implemented |
| TCP connection establishment | Implemented |
| Persistent session management | Implemented |
| Receiver threads | Implemented |
| Bidirectional text messaging | Implemented |
| Partial-send handling | Implemented |
| Peer-disconnection detection | Implemented |
| Socket cleanup | Implemented |
| Settings display | Implemented |
| Windows file selection | Implemented |
| Room-code generation | Implemented |
| Room-code verification | Not implemented |
| File transmission | Not implemented |
| File reception | Not implemented |
| Multi-client hosting | Not implemented |
| Transfer progress reporting | Not implemented |

## 13. Architectural Considerations and Future Improvements

The following areas should be considered as Lab Share continues to grow.

### 13.1 File-transfer integration

The communication layer needs to be extended to support file transmission without interfering with the existing text-messaging protocol.

A clearly defined application-level protocol should distinguish different types of communication.

### 13.2 Protocol design

The current communication protocol uses newline-delimited text messages.

File transfer will require a more structured framing mechanism capable of handling binary data, file metadata and transfer completion.

### 13.3 Room-code functionality

The existing room-code generation should eventually be integrated into the connection workflow if room-based access is part of the intended design.

### 13.4 Configuration management

Networking configuration should eventually be loaded dynamically rather than relying on hardcoded values.

### 13.5 Error handling

Winsock errors, thread-creation failures, file-operation failures and invalid user input should have consistent handling across the application.

### 13.6 Module separation

As the project grows, new responsibilities should remain separated into appropriate modules.

The communication, file-processing, validation, session-management and user-interface responsibilities should not become unnecessarily coupled.

---

## 14. Architecture Summary

Lab Share currently follows a modular client-server architecture implemented in C using Winsock 2.2.

The application layer handles user navigation and session selection through `main.c`.

The host and client modules manage their respective TCP connection lifecycles and session execution.

The communication layer, implemented in `transfer.c`, provides shared bidirectional text-messaging functionality.

Dedicated receiver threads allow incoming messages to be processed independently of user interaction.

Supporting modules provide validation, local IPv4 discovery, room-code generation, CLI functionality and Windows file selection.

The networking and communication foundation is now established. The next major architectural milestone is integrating a complete file-transfer subsystem into the existing session architecture.

**Documentation status:** Updated to reflect the uploaded source files.

**Implementation status:** TCP sessions and bidirectional text messaging implemented; file-transfer functionality remains under development.
