# Lab Share

As a college student, I’ve often needed to transfer files between computers in college labs. I started wondering: why do we need to rely on apps like WhatsApp or Gmail just to send a file to someone sitting on the same local network? I wanted to build something simple that lets users transfer files directly between computers without needing accounts or uploading files to third-party services. That’s where the idea for Lab Share came from.

Lab Share is a peer-to-peer file-sharing application written in C with a command-line interface (CLI). The main goal is to make file transfers between computers on the same local area network (LAN), such as those in college computer labs, simple and convenient.

In the future, I also want to explore QR code-based sharing, allowing users to scan a code and download files directly to their devices through a browser. For now, though, I’m focusing on building the basic LAN file-transfer functionality.

Note :- Lab Share is a learning project built with the help of AI for guidance and debugging. Most of the code is written by me, and I’m focused on understanding how it works rather than blindly copying it. My goal is to learn C and networking by building something practical.


## Getting Started

### 1. Clone the Repository

```bash
git clone <repository-url>
cd LabShare
```

### 2. Build the Executable

```bash
make
```

### 3. Run Lab Share

```bash
./build/main.exe
```

**Prerequisites:** Windows, GCC (MinGW-w64), and GNU Make.

For detailed installation and setup instructions, see the [Setup Guide](docs/setup.md).


## Project Structure

```text
LabShare/
├── include/             # Header files containing function declarations
│   ├── client.h         
│   ├── host.h           
│   └── transfer.h       
├── src/
│   ├── main.c           # Entry point; starts the CLI and handles the main menu
│   ├── client.c         # Handles connecting to a host
│   ├── host.c           # Handles hosting and accepting connections
│   └── transfer.c       # Handles file transfer operations
├── build/
│   └── main.exe         # Compiled application executable
├── makefile             # Build instructions
├── settings.text        # Application configuration settings
└── README.md            # Project documentation
```

## Technology Stack

- **Language:** C
- **Compiler:** GCC (MinGW-w64)
- **Networking:** Winsock2 (Windows socket API)
- **Protocols:**
  - TCP — Reliable file transfers
  - UDP — Local network room discovery (planned)
  - HTTP — Browser-based file downloads via QR codes (planned)
- **Build System:** GNU Make
- **Current Platform:** Windows
- **Planned Platform Support:** Linux and macOS


## Feedback & Contributions

I'm open to feedback, constructive criticism, and suggestions for improvement. Feel free to reach out to me via email or LinkedIn. I'm always willing to learn, improve, and collaborate with others!
