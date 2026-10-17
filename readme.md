# Lab Share

**A peer-to-peer file-sharing application built in C.**

Lab Share is a project focused on making file sharing between computers simple and efficient. The goal is to allow users to host sharing rooms, connect to other devices on a local network, and transfer files directly between peers.

The project is being developed using C and Winsock on Windows, with a focus on understanding network programming, TCP/IP communication, and file handling.

## Features

### Planned Features

- **Host a Room:** Start a server that listens for incoming connections.
- **Join a Room:** Connect to an existing host.
- **File Transfer:** Send files between connected devices using TCP.
- **Room Discovery:** Discover available rooms on the local network using UDP broadcast.
- **Room Keys:** Use a room key to control access to a sharing room.
- **Large File Support:** Transfer files in chunks without loading the entire file into memory.
- **Terminal Interface:** Provide a simple terminal-based user interface.
- **QR Code Sharing:** Generate a QR code that allows a phone to download a shared file through a browser.
- **Cross-Network Sharing:** Explore ways to share files between devices on different networks.

> **Note:** Lab Share is under development. Planned features may not be implemented yet.

## Technology Stack

- **Language:** C
- **Compiler:** GCC (MinGW-w64)
- **Networking:** Winsock2
- **Protocols:**
  - TCP for reliable file transfers.
  - UDP for local network discovery.
  - HTTP for the planned browser-based download feature.
- **Platform:** Windows (initial development target)

## Project Structure

The project is being organized into separate source files for the application's entry point, hosting functionality, and client functionality.

```text
LabShare/
├── main.c       # Application entry point
├── host.c       # Server and hosting functionality
├── client.c     # Client and connection functionality
└── README.md    # Project documentation
```

The structure may evolve as the project grows.

## Getting Started

### Prerequisites

To build the project, you need:

- Windows
- GCC with MinGW-w64
- Winsock2 development headers and libraries

### Clone the Repository

```bash
git clone <repository-url>
cd LabShare
```

Replace `<repository-url>` with the repository's actual URL.

### Compile the Host

```bash
gcc -Wall -Wextra -Wpedantic -g main.c host.c -o host.exe -lws2_32
```

### Compile the Client

```bash
gcc -Wall -Wextra -Wpedantic -g main.c client.c -o client.exe -lws2_32
```

These commands assume that `main.c` is compatible with both builds and that each build contains exactly one `main()` function.

### Run the Application

In the terminal, run:

```bash
./host.exe
```

Or run the client:

```bash
./client.exe
```

**Note:** These commands demonstrate the intended build setup. The application must implement the corresponding host and client functionality before the executables can be used for file sharing.

## How It Works

The planned architecture separates connection management, room discovery, and file transfer.

### 1. Hosting

The host creates a TCP socket, binds it to an IP address and port, and listens for incoming connections.

When a client connects, the host accepts the connection and uses the resulting connected socket to communicate with that client.

### 2. Connecting

The client connects to the host's IP address and TCP port.

Once connected, the client and host can exchange data over the TCP connection.

### 3. File Transfer

Files are read in binary mode and transferred in chunks.

The receiving device writes the incoming bytes to a file. A transfer protocol will track the file size and completion status so that the receiver knows when the complete file has arrived.

### 4. Room Discovery

UDP broadcast is planned to help devices discover sharing rooms on the same local network.

Discovery may depend on the network configuration. Some Wi-Fi networks, especially managed or campus networks, restrict communication between connected devices.

### 5. QR Code Downloads

A future feature will let users generate a QR code for a shared file.

The QR code will contain a URL that a phone can open in its browser. The phone will then download the file from the host or another designated server.

Access from outside the local network may require a tunneling service or relay infrastructure.

## Security Considerations

Security is an important part of the planned implementation.

- Validate incoming connections and file-transfer requests.
- Restrict access to files explicitly shared by the user.
- Validate filenames and prevent directory traversal.
- Handle incomplete transfers and connection failures safely.
- Use unpredictable, temporary access tokens for browser-based downloads.
- Expire temporary download links and stop serving files when sharing ends.
- Avoid exposing the host to public networks without appropriate security controls