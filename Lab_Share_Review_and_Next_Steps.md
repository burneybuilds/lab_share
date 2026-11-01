Lab Share
Code Review, Fix List, and Next Steps
Based on the uploaded C source files, headers, and Makefile

## Purpose

Use this document as a working checklist while improving Lab Share. Fix the issues in order, test after each stage, and avoid adding advanced features until a basic file transfer works reliably. The review is based on the files supplied in this conversation; the project has not yet been compiled or run here.

## Current status


| Area | Status | Observation |
| --- | --- | --- |
| Application menu | Present in main.c | The menu routes to host, client, settings, and exit. |
| TCP host setup | Basic implementation | Winsock startup, socket creation, bind, listen, and accept are present. |
| TCP client setup | Basic implementation | The client validates an IPv4 address and attempts to connect on port 8080. |
| File picker | Implemented | file_selecter() returns a dynamically allocated path; caller frees it. |
| File transfer | Not implemented | The send/receive functions are commented out or not connected to the socket workflow. |
| Session lifecycle | Needs fixing | The host closes the accepted client after accepting it; the client closes after the connection message. |
| Room code | Partial | A four-digit number is generated but is not used in a session-join protocol. |
| Multiple clients | Not implemented | The host currently accepts one client and then ends the server handler. |


## Priority 1 — Fix before implementing transfer


### Give every header a unique include guard.

host.h currently uses RANDOM_H, and transfer.h also uses RANDOM_H. If both are included in one translation unit, the second header can be skipped. Use HOST_H for host.h and TRANSFER_H for transfer.h.

### Make client.h self-contained.

client.h declares functions using SOCKET but does not include winsock2.h. Include <winsock2.h> in headers that expose SOCKET, and ensure winsock2.h is included before windows.h where both are used.

### Fix settings() error handling in main.c.

If fopen("../config/settings.txt", "r") returns NULL, print the error and return immediately. Do not call fscanf() or fclose() on a NULL FILE pointer. Check fscanf() results before printing settings.

### Correct the settings labels.

The current output labels both connection_timeout and max_connections as “Connection Timeout”. Label max_connections correctly.

### Handle invalid menu input consistently.

Check scanf() return values and clear invalid input. In host_menu_handler(), ensure invalid input does not trigger more than one misleading error message.

### Propagate host startup failures.

host_menu_handler() calls server_side_handler() but ignores its return code. If the server fails to initialize, bind, listen, or accept, handle that failure and do not continue as if hosting succeeded.

### Keep the accepted socket open for the session.

Do not close the accepted client socket immediately after accept(). Keep it alive while protocol messages and file data are exchanged, then close it during cleanup.

## Priority 2 — Define a minimal transfer protocol

For the first milestone, keep the design simple: one host and one client, one file per transfer, and a synchronous connection. Decide which side sends and which side receives before finalizing the public function signatures.
1. Connect: client connects to the host's IPv4 address and configured port.
1. Negotiate: sender announces a protocol version, filename (or safe basename), and file size using a defined representation.
1. Validate: receiver checks the metadata, rejects invalid sizes/names, and chooses a destination path.
1. Transfer: sender streams the file in binary mode; receiver writes chunks to disk.
1. Complete: receiver reports success only after all expected bytes have been written.
1. Cleanup: both sides close files and sockets on success, disconnect, or error.

### TCP implementation notes

- TCP is a byte stream. One send() call does not guarantee that every requested byte was sent, and one recv() call does not guarantee that a whole message arrived.
- Implement send_all() and recv_all() helpers that loop until the requested byte count is handled or an error/disconnect occurs.
- Use binary file modes: "rb" for reading the source and "wb" for writing the destination.
- Do not send a raw C struct as a wire format without defining byte order, field sizes, and packing. Use explicit, documented fields.
- Validate all received metadata. Never use an untrusted incoming filename as an unrestricted output path.
- For the first version, set a reasonable maximum file size and reject malformed or incomplete transfers.

## Suggested function responsibilities


| File | Responsibility |
| --- | --- |
| main.c | Menu and application navigation; settings loading and display. |
| host.c | Start server, accept a client, manage host-side session, call receive logic when the host receives a file. |
| client.c | Validate host address, connect, manage client-side session, call send logic when the client sends a file. |
| transfer.c | File I/O, send_all/recv_all, metadata exchange, transfer status, and transfer-specific validation. |
| host.h / client.h / transfer.h | Public declarations for functions implemented in the corresponding modules; unique include guards and required types. |
| Makefile | Compile sources, link Winsock and common-dialog libraries, track header dependencies, and provide clean/build targets. |


## Priority 3 — Makefile improvements

The current Makefile has useful warning flags and links -lws2_32 and -lcomdlg32. Keep those if they match the APIs you use. Improve the build in these ways:
- Add header dependencies so editing a .h file triggers a rebuild. For GCC, compiler-generated dependency files (-MMD -MP) are one option.
- Consider compiling each .c file to an object file in build/ and linking the objects. This avoids recompiling every source file after each small change.
- The commands mkdir -p and rm -f are Unix-style. They work in Git Bash/MSYS2 but may not work in Windows Command Prompt. Choose and document the shell used to run make.
- The Makefile uses -Wconversion, -Wsign-conversion, and -Wmaybe-uninitialized. These can be useful but may produce extra warnings depending on the compiler version; address warnings rather than suppressing them blindly.
- Check that the source paths in SRC match the actual project structure (src/main.c, src/host.c, src/client.c, src/transfer.c).

## Priority 4 — Reliability, security, and polish

- Check every important return value from WSAStartup(), socket(), bind(), listen(), accept(), connect(), send(), recv(), fopen(), fread(), fwrite(), and fclose() where applicable.
- Use clear error messages that include the failed operation; where useful, capture WSAGetLastError() for Winsock failures.
- Ensure all paths release allocated memory, open files, and sockets. Centralized cleanup paths can help.
- Do not treat a four-digit room code as strong authentication. If room codes are used, limit guessing attempts and combine them with sensible local-network assumptions or stronger authentication.
- For future versions, consider file-size limits, overwrite confirmation, transfer progress, cancellation, checksum verification, and a clear policy for where received files are saved.
- Only after one transfer is stable, consider multiple clients, background threads or asynchronous I/O, and QR-code-based joining.

## Test plan and definition of done


| Test | Pass condition |
| --- | --- |
| Build | make clean and make complete without errors; review and resolve relevant warnings. |
| Invalid input | Letters and out-of-range menu options do not crash the app or trap it in a confusing loop. |
| Connection success | Start host, connect one client on the same machine or LAN, and keep the connection open. |
| Connection failure | Try an invalid IP, a host that is not running, and a blocked/wrong port; the app reports failure and cleans up. |
| Small file | Transfer a small text file and compare source and destination. |
| Binary file | Transfer an image or other binary file and verify the bytes match. |
| Large file | Transfer a file larger than a single typical socket buffer; confirm the transfer loops correctly. |
| Disconnect/error | Close one side during transfer; the other side reports failure and releases resources. |
| Repeat transfer | Perform multiple transfers without restarting the application, if that is the intended session behavior. |


## Recommended implementation order

1. Fix header guards and required includes.
1. Fix settings() and input handling.
1. Build with the current Makefile and address compile errors.
1. Fix the host/client socket lifecycle so the connection stays open.
1. Agree on sender/receiver roles and define transfer function signatures.
1. Implement and test send_all() / recv_all().
1. Implement one-file transfer with metadata and safe destination handling.
1. Verify text, binary, and larger files; test disconnect cleanup.
1. Improve Makefile dependencies and object-file builds.
1. Add room-code joining, multiple clients, progress, and QR features later.

## Implementation notes to keep in mind

The current client asks for an IPv4 address and uses port 8080. The host binds to INADDR_ANY on port 8080. This is suitable for an initial local-network test, but the port should eventually come from validated configuration. The current host handler accepts one client and then closes the session, so the first goal is a stable single-client transfer—not multi-client support.
The headers currently declare handle_client(), close_connection(), and cleanup_server() in host.h, but those functions were not present in the supplied host.c. Either implement them when needed or keep the public header limited to functions that currently exist.

## Working checklist

- [ ] Unique include guards fixed
- [ ] Required headers included
- [ ] Settings file errors handled safely
- [ ] Menu input validated
- [ ] Project compiles cleanly
- [ ] Host keeps accepted socket open
- [ ] Client keeps connection open during transfer
- [ ] Transfer protocol documented
- [ ] send_all() and recv_all() implemented
- [ ] One file transfers correctly
- [ ] Binary and large-file tests pass
- [ ] Disconnect and cleanup tests pass
- [ ] Only then: room code and multiple clients
