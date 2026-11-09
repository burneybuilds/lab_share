# Lab Share

Lab Share is a Windows command-line application written in C for
experimenting with peer-to-peer communication over a local network. A
host listens for one TCP client, and a client connects using the host's
IPv4 address.

> **Current status:** Lab Share is an early networking prototype. The
> current code establishes TCP connections and sends short text
> messages. Despite the `send_file` function name and menu label, file
> contents are not transferred yet.

## What works today

- Host-side Winsock initialization and TCP socket setup.
- Binding to all local IPv4 interfaces on port `8080`.
- Waiting for and accepting one client connection.
- Client-side IPv4 input and validation with `inet_pton`.
- Connecting to a host by IPv4 address.
- Sending text entered in the host or client menu.
- Displaying the values in `config/settings.txt`.
- Windows file-selection dialog code for future file-transfer work.

The host and client currently close their sockets after leaving their
menus. Network discovery, persistent multi-client sessions, file-byte
transfer, transfer progress, and receiving/saving files are not
implemented.

## Requirements

- Windows
- [MSYS2](https://www.msys2.org/)
- GCC from the MSYS2 UCRT64 environment
- GNU Make
- Git

Install the compiler and build tools from an **MSYS2 UCRT64** terminal:

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make
```

Verify the tools:

```bash
gcc --version
make --version
```

## Build

Clone the repository and enter its root directory:

```bash
git clone <repository-url>
cd lab_share
```

Build the application and test executable:

```bash
make
```

The build produces:

- `build/main.exe` — the interactive application.
- `build/tests.exe` — the test executable.

To build only one target:

```bash
make app
make tests
```

## Run

Start the application from the repository root:

```bash
./build/main.exe
```

The main menu provides:

1. **Host a Session** — starts a TCP listener on port `8080`, prints a
   local IPv4 address and waits for one client.
2. **Connect to a Session** — asks for the host's IPv4 address and
   connects to port `8080`.
3. **Settings** — reads the values in `config/settings.txt`.
4. **Exit** — closes the application.

For a local connection test, run one instance as the host and connect
from another instance using `127.0.0.1`. For two computers on the same
LAN, enter the host's reachable local IPv4 address. Windows Firewall may
need an inbound rule allowing TCP traffic on port `8080`.

The current session menu sends text input. It does not select, upload,
download, or save a file.

## Test

Run the Windows socket and validation tests with:

```bash
make test
```

The tests cover Winsock initialization, socket creation, binding,
listening, invalid client acceptance, room-code generation, IPv4
validation, and client socket creation.

Remove generated build output with:

```bash
make clean
```

## Configuration

The sample configuration is stored in
[config/settings.txt](config/settings.txt):

```text
username=Tushar
port=8080
connection_timeout=5
max_connections=1
```

The host and client networking code currently use the hard-coded port
`8080`; the other settings are displayed but are not yet used to control
network behavior. The settings reader expects its relative path
`../config/settings.txt`, so the Settings menu may not find the file
when the executable is launched from the repository root.

## Project layout

```text
lab_share/
├── include/                  # Public header declarations
│   ├── client.h
│   ├── host.h
│   ├── transfer.h
│   ├── utils.h
│   └── validate.h
├── src/
│   ├── main.c                # Main menu and settings display
│   ├── host.c                # TCP server setup and host menu
│   ├── client.c              # TCP client setup and client menu
│   ├── transfer.c            # Current text send/receive helpers
│   ├── utils.c               # Menus, local IP lookup, file dialog, room code
│   └── validate.c             # Input and IPv4 validation
├── tests/
│   ├── test_host.c
│   └── test_client.c
├── config/settings.txt       # Sample settings values
├── docs/
│   ├── architecture.md       # Component and lifecycle notes
│   ├── networking.md         # Winsock/TCP details
│   └── setup.md              # Windows setup guide
├── makefile                  # Application and test build targets
└── readme.md                 # Project documentation
```

## Technical overview

Lab Share uses:

| Area | Current implementation |
|---|---|
| Language | C |
| Platform | Windows |
| Networking API | Winsock2 |
| Addressing | IPv4 |
| Transport | TCP |
| Port | `8080` |
| Build system | GNU Make |
| Link libraries | `ws2_32`, `comdlg32` |

The host calls `WSAStartup`, creates an IPv4 TCP socket, binds it to
`INADDR_ANY:8080`, listens, and accepts one client. The client starts
Winsock, creates a socket, validates the entered IPv4 address, and calls
`connect` on port `8080`. Both sides use a menu after connection and
clean up their sockets with `closesocket` and `WSACleanup`.

See the [setup guide](docs/setup.md), [architecture notes](docs/architecture.md),
and [networking notes](docs/networking.md) for more detail.

## Roadmap

Planned work includes:

- Keep the connection alive for a complete session lifecycle.
- Define a framed message and file-transfer protocol.
- Implement reliable file sending and receiving.
- Add transfer progress and disconnection handling.
- Add local-network discovery.
- Decide whether to support multiple clients.
- Add Linux and macOS support.

Lab Share is a learning project focused on understanding C,
socket programming, resource ownership, and network protocols. Feedback
and constructive suggestions are welcome.
