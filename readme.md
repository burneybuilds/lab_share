# Lab Share

**Simple, peer-to-peer file sharing for computers on the same local network.**

Lab Share is a command-line file-sharing application written in C that aims to make transferring files between computers on a local area network (LAN) simple and convenient. No accounts, no cloud uploads, and no third-party file-sharing services required.

## About the Project

As a college student, I've often needed to transfer files between computers in college labs. I started wondering: why should we rely on applications like WhatsApp or Gmail just to share a file with someone sitting on the same local network?

That question inspired me to build **Lab Share** — a lightweight file-sharing application designed to transfer files directly between computers on the same LAN.

The project is built using C and Windows Sockets (Winsock2), with a command-line interface (CLI). My current focus is implementing the core file-transfer functionality and making it reliable and easy to use.

### Project Goals

- **Direct file transfers:** Transfer files between computers on the same local network.
- **No accounts required:** Keep the sharing process simple without requiring user registration.
- **No third-party uploads:** Transfer files directly between devices instead of relying on cloud services.
- **Lightweight and practical:** Build a simple application that works through a command-line interface.
- **Learn by building:** Use a real-world project to develop a deeper understanding of C programming and computer networking.

## Current Status

Lab Share is currently under development, with a focus on implementing the basic LAN file-transfer functionality.

### Planned Features

- [ ] UDP-based local network discovery.
- [ ] QR code-based file sharing.
- [ ] Browser-based file downloads over HTTP.
- [ ] Linux and macOS support.
- [ ] Additional improvements to reliability and usability.

These features are planned for future development and are not necessarily available in the current version.

## Getting Started

### Prerequisites

Before building Lab Share, make sure you have the following installed:

- **Operating system:** Windows
- **Compiler:** GCC (MinGW-w64)
- **Build system:** GNU Make

### 1. Clone the Repository

Clone the repository and navigate to the project directory:

```bash
git clone <repository-url>
cd LabShare
```

Replace `<repository-url>` with the actual URL of your Git repository.

### 2. Build the Application

Run the following command from the project directory:

```bash
make
```

This uses the project's Makefile to compile the source files and generate the executable.

### 3. Run Lab Share

Launch the application using:

```bash
./build/main.exe
```

**Note:** Make sure the build completes successfully before running the executable.

For detailed installation instructions, troubleshooting tips, and setup information, see the [Setup Guide](docs/setup.md).

## Project Structure

```text
LabShare/
├── include/              # Header files containing function declarations
│   ├── client.h          # Client functionality
│   ├── host.h            # Host functionality
│   └── transfer.h        # File-transfer functionality
├── src/
│   ├── main.c            # Application entry point and CLI menu
│   ├── client.c          # Handles connections to a host
│   ├── host.c             # Handles hosting and incoming connections
│   └── transfer.c        # Handles file-transfer operations
├── build/
│   └── main.exe          # Compiled application executable
├── makefile              # Build instructions
├── settings.text         # Application configuration settings
├── docs/
│   └── setup.md          # Installation and setup guide
└── README.md             # Project documentation
```

## Technology Stack

| Component | Technology |
|---|---|
| Programming Language | C |
| Compiler | GCC (MinGW-w64) |
| Networking API | Winsock2 |
| File Transfer Protocol | TCP |
| Network Discovery | UDP (planned) |
| Browser-Based Sharing | HTTP (planned) |
| Build System | GNU Make |
| Current Platform | Windows |
| Planned Platforms | Linux and macOS |

## Learning Objectives

Lab Share is also a hands-on learning project focused on understanding how low-level networking works in practice.

Through this project, I'm exploring:

- TCP socket programming in C.
- Client-server communication.
- File handling and binary data transfer.
- Network connections and error handling.
- Building and organizing multi-file C projects using Make.
- Designing a foundation for future features such as network discovery and browser-based sharing.

## A Note on AI-Assisted Development

Lab Share is a learning project developed with the help of AI tools for guidance, debugging, and problem-solving.

Most of the code is written by me, and I use AI as a learning aid rather than simply copying solutions. My goal is to understand the code I write, learn from mistakes, and strengthen my knowledge of C programming and computer networking by building something practical.

## Feedback & Contributions

I'm always open to feedback, constructive criticism, and suggestions for improvement.

If you have ideas for new features, spot a potential issue, or want to discuss the project, feel free to reach out via email or LinkedIn.

I'm excited to keep learning, improving Lab Share, and collaborating with others who share an interest in C programming and networking.

---

**Lab Share — Learning networking by building something useful.**