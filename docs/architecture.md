# Lab Share --- Architecture

## Overview

Lab Share is a Windows C console application intended to let users host
a session or connect to an existing session, with file sharing as the
planned core feature. The current source code includes the application
menu, host-side TCP setup, client-side TCP setup, settings display, and
a Windows file-selection dialog.

The file-transfer functions are currently placeholders; the uploaded
code does not yet implement sending or receiving file contents.

## Project layout

  -----------------------------------------------------------------------
  File                                Responsibility visible in the
                                      current code
  ----------------------------------- -----------------------------------
  `main.c`                            Runs the main menu, dispatches Host
                                      / Connect / Settings actions, and
                                      exits the application.

  `host.c`                            Creates and prepares the TCP
                                      listening socket, accepts one
                                      client, generates a room code, and
                                      displays the host menu.

  `host.h`                            Declares host and server-related
                                      functions.

  `client.c`                          Reads an IPv4 address, validates
                                      it, creates a TCP socket, and
                                      attempts to connect to the host.

  `client.h`                          Declares client-side functions.

  `transfer.c`                        Opens a Windows file-selection
                                      dialog and contains placeholder
                                      send/receive functions.

  `transfer.h`                        Declares file-selection and
                                      transfer functions.
  -----------------------------------------------------------------------

> The uploaded files do not include a `Makefile`, settings file, or full
> directory listing, so their build configuration and exact repository
> layout cannot be documented from these sources alone.

## Application flow

``` text
main()
  |
  v
cli_interface_handler()
  |
  +-- 1. Host a Session ------> host() ------> host_menu_handler()
  |                                         |
  |                                         v
  |                                  server_side_handler()
  |                                         |
  |                  Winsock -> socket -> bind -> listen -> accept
  |
  +-- 2. Connect to a Session -> client() -> client_side_handler()
  |                                         |
  |                       Winsock -> socket -> read IPv4 -> validate -> connect
  |
  +-- 3. Settings ------------> settings()
  |
  +-- 4. Exit
```

The menu is implemented in `main.c`. The `host()` and `client()` wrapper
functions call the relevant side's handler. The host menu displays Send
File, Receive File, and Back options.

## Main components

### Main menu and settings

`cli_interface_handler()` displays the main menu and reads the user's
choice. `main()` dispatches that choice to hosting, connecting,
settings, or exit. The `settings()` function attempts to read username,
port, connection timeout, and maximum connection values from
`../config/settings.txt` and prints them.

### Host-side TCP setup

The host-side networking functions in `host.c` split setup into
individual steps:

-   `initialize_winsock()` calls `WSAStartup()`.
-   `create_server_socket()` creates an IPv4 TCP socket with `socket()`.
-   `bind_server_socket()` binds the socket to the supplied port and
    `INADDR_ANY`.
-   `start_listening()` calls `listen()`.
-   `accept_client()` calls `accept()` and returns the connected client
    socket.
-   `server_side_handler()` coordinates these operations and handles
    several failure paths.

The current handler uses port `8080` and accepts one client. After
printing that a connection was established, it closes both sockets and
calls `WSACleanup()`. It does not yet maintain a connected session for
file transfer.

### Client-side TCP setup

The client-side functions in `client.c` divide the work into smaller
operations:

-   `initialize_winsock_client()` initializes Winsock.
-   `client_session()` creates a TCP socket.
-   `take_ip()` prompts for an IPv4 address.
-   `validate_ip_address()` uses `inet_pton()` to validate IPv4 text.
-   `connect_to_host()` repeats the prompt while the address is
    rejected, prepares a `sockaddr_in`, and calls `connect()`.
-   `client_side_handler()` coordinates initialization, connection,
    reporting, and cleanup.

The current code uses port `8080`. On success, `client_side_handler()`
pauses for input and then closes the socket and cleans up Winsock.

### File selection and transfer

`file_selecter()` uses the Windows common file dialog
(`GetOpenFileNameA`) to let the user select an existing file. It
allocates memory for a copy of the selected path; the caller in
`host_menu_handler()` frees that allocation.

`send_file()` and `receive_file()` are empty placeholders in the
uploaded `transfer.c`. The host menu currently selects a file but does
not pass it to a working transfer routine. Receiving is also not
implemented yet.

## Current state

### Present in the uploaded source

-   Main menu with Host, Connect, Settings, and Exit choices.
-   Host-side Winsock initialization and TCP socket setup.
-   Client-side Winsock initialization and TCP connection attempt.
-   IPv4 text validation using `inet_pton()`.
-   A Windows file-selection dialog.
-   Basic socket cleanup on the visible host/client paths.

### Not yet implemented or not shown as working

-   Sending file bytes over the established TCP connection.
-   Receiving and saving file bytes.
-   Keeping the client connection alive for an actual transfer session.
-   Passing an active connected socket into the transfer routines.
-   Multi-client hosting, device discovery, and transfer progress
    reporting.
-   Build instructions and project configuration, because those files
    were not included in the uploaded set.

## Design notes for future development

1.  Keep the listening socket and accepted client socket as separate
    handles; they serve different purposes.
2.  Keep the connected socket alive while sending or receiving data,
    then close it when the session ends.
3.  Make ownership of sockets explicit: the function that creates a
    socket should clearly document whether it retains or transfers
    responsibility for closing it.
4.  Before implementing file transfer, define a simple transfer
    protocol, including how the receiver learns the filename and file
    size and how the end of the file is detected.
5.  Check return values from input and file operations, and report
    useful error details when Winsock calls fail.
