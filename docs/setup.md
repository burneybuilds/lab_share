# Setup Guide

This guide explains how to build and run Lab Share on Windows.

## Prerequisites

- Windows
- GCC (MinGW-w64)
- Git
- MSYS2

## 1. Install GNU Make

If you already have GCC installed through MSYS2 UCRT64, follow these steps.

**1. Open the MSYS2 UCRT64 terminal.**

**2. Install GNU Make:**

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-make
```

**3. Restart your terminal and verify the installation:**

```bash
gcc --version
make --version
```

If both commands display version information, you're ready to continue.

## 2. Clone the Repository

```bash
git clone <repository-url>
cd LabShare
```

Replace `<repository-url>` with the URL of the Lab Share repository.

## 3. Build the Executable

From the project root directory, run:

```bash
make
```

This command uses the project's `makefile` to compile the source code and generate the executable.

After a successful build, the executable will be available at:

```text
build/main.exe
```

## 4. Run Lab Share

```bash
./build/main.exe
```

## 5. Run on Another Computer

You can copy `build/main.exe` to another Windows computer, provided the required runtime dependencies are available.

For file transfers between computers, both devices need network connectivity. The host computer may also need to allow Lab Share through Windows Firewall.

**Note:** Lab Share is a work in progress, and some features are still under development.