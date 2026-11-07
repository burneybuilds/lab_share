LABSHARE — CODE REVIEW TODO LIST
=================================

Basis: Review of the uploaded source files:
- client.c
- host.c
- main.c
- transfer.c
- client.h
- host.h
- transfer.h

Status: Review findings from static source inspection. These items have not
been verified by compiling or running the Windows Winsock application.

PRIORITY KEY
------------
[P0] Critical: fix before relying on the current connection/messaging flow.
[P1] High: fix before expanding functionality.
[P2] Medium: address during cleanup/refactoring or before release.

============================================================
1. CRITICAL BUGS — P0
============================================================

[+] P0-01: Return INVALID_SOCKET from every failed SOCKET-returning path.
    File: client.c
    Function: client_side_handler()
    Issue:
      The function returns 1 on Winsock initialization and socket/connect
      failures, even though callers check for INVALID_SOCKET. Since 1 is not
      INVALID_SOCKET, a failed setup can be mistaken for a valid socket.
    Fix:
      Return INVALID_SOCKET on every failure path. Keep success paths
      returning the actual socket handle.

[ ] P0-02: Standardize connect_to_host() error handling.
    File: client.c
    Function: connect_to_host()
    Issue:
      connect() failure returns SOCKET_ERROR (-1), but client_side_handler()
      checks for 1 and 2. The actual failure value is therefore not handled
      as intended, and the client may return an unconnected socket.
    Fix:
      Use a consistent success/failure convention (for example, 0 on success
      and non-zero on failure), capture WSAGetLastError() immediately after
      a failed Winsock call, and check inet_pton()'s return value.

[ ] P0-03: Validate the host connection before using its sockets.
    File: host.c
    Function: host_menu_handler()
    Issue:
      server_side_handler() can return a HostConnection containing
      INVALID_SOCKET handles after initialization, bind, listen, or accept
      failure. host_menu_handler() does not check the returned handles
      before calling send_file().
    Fix:
      Check client_socket and listen_socket immediately after setup. On
      failure, report the error and return without entering the connected
      menu or attempting I/O.

[ ] P0-04: Fix the message framing mismatch.
    Files: transfer.c, host.c
    Functions: send_file(), receive_message(), host_menu_handler()
    Issue:
      receive_message() waits for a newline delimiter. The host removes the
      newline from its input with strcspn() before sending, so the receiver
      may wait indefinitely for a delimiter or more data.
    Fix:
      Define one framing protocol and use it on both ends. For a simple
      text-only prototype, send the newline delimiter and read until it.
      For a more robust protocol, use a fixed-size header containing message
      type and payload length, then receive exactly that many bytes. TCP is
      a byte stream: one send() does not correspond to one recv().

[ ] P0-05: Implement receiving on the host side.
    File: host.c
    Function: host_menu_handler()
    Issue:
      The host's option for receiving currently only prints a message.
      It does not call receive_message(), so the current application does
      not provide complete bidirectional messaging.
    Fix:
      Wire the host receive operation to the receive function and ensure
      both client and host follow the same protocol.

[ ] P0-06: Fix settings-file NULL handling.
    File: main.c
    Function: settings()
    Issue:
      If fopen() fails, the code prints an error but continues to call
      fscanf() with a NULL FILE pointer. This is undefined behavior and can
      crash the application.
    Fix:
      Return immediately when fopen() returns NULL. Check every fscanf()
      result before using or printing the corresponding setting.

============================================================
2. HIGH-PRIORITY BUGS — P1
============================================================

[ ] P1-01: Fix the client menu's Back/exit mismatch.
    File: client.c
    Function: client_side_menu()
    Issue:
      The menu displays option 2 as Back, but the loop and break condition
      use option 3. Selecting 2 does not exit as the menu promises.
    Fix:
      Make the displayed options and loop condition consistent. If Back is
      option 2, exit on 2.

[ ] P1-02: Handle failed/non-numeric menu input.
    Files: client.c, host.c
    Issue:
      Some scanf() calls do not check whether conversion succeeded. If the
      user enters letters, the invalid input may remain in stdin and cause
      repeated failures or a stuck menu.
    Fix:
      Check scanf() return values and clear the rest of the input line on
      failure. Prefer a consistent fgets() plus strtol() input helper.

[ ] P1-03: Consume the newline before host fgets().
    File: host.c
    Function: host_menu_handler()
    Issue:
      scanf("%d", &user_input) leaves the newline in stdin. The following
      fgets() may read that newline immediately, producing an empty message.
    Fix:
      Consume the rest of the input line after successfully reading the menu
      number, or migrate menu input to fgets() and strtol().

[ ] P1-04: Make host menu options consistent.
    File: host.c
    Function: cli_options(), input_handler(), host_menu_handler()
    Issue:
      The displayed menu says option 2 is Back, but its implementation says
      it is waiting to receive a file. input_handler() accepts 1 through 3,
      although the displayed menu does not clearly define option 3.
    Fix:
      Decide and document the menu actions, then make display, validation,
      and implementation agree.

[ ] P1-05: Validate receive_message() arguments.
    File: transfer.c
    Function: receive_message()
    Issue:
      buffer and buffer_size are not validated. A NULL buffer or a size of
      zero/one can lead to invalid memory access or an out-of-bounds
      terminator write.
    Fix:
      Reject NULL buffers and buffer_size values that cannot hold at least
      one character plus the terminating NUL byte.

[ ] P1-06: Handle messages larger than the receive buffer.
    File: transfer.c
    Function: receive_message()
    Issue:
      If the message fills the buffer before the delimiter arrives, the
      function returns with the rest of that message still in the TCP stream.
      Those leftover bytes can be interpreted as the next message.
    Fix:
      Either reject and drain the rest of the framed message, or use a
      length-prefixed protocol and receive the declared payload length.

[ ] P1-07: Handle empty messages and disconnects consistently.
    Files: transfer.c, client.c, host.c
    Issue:
      Empty messages and connection closure need explicit protocol behavior.
      The receiver currently distinguishes socket error and orderly shutdown,
      but callers should handle each result consistently.
    Fix:
      Document return codes (success, disconnected, error, oversized input)
      and make both menus respond appropriately.

[ ] P1-08: Implement centralized host connection cleanup.
    Files: host.c, host.h
    Issue:
      close_connection() is declared in host.h but no implementation appears
      in the uploaded host.c. Cleanup is duplicated across setup and menu
      code, making missed/double cleanup easier.
    Fix:
      Implement close_connection(), close only valid sockets, reset handles,
      and define clearly which function owns WSACleanup(). Avoid calling
      WSACleanup() twice for one successful WSAStartup() lifecycle.

[ ] P1-09: Check all file and string operations.
    Files: main.c, transfer.c
    Issue:
      fscanf() results are not checked. transfer.c uses strlen() and strcpy()
      but does not include <string.h>.
    Fix:
      Include <string.h> where needed and validate all input, allocation,
      file, and conversion results.

[ ] P1-10: Make transfer.h self-contained.
    File: transfer.h
    Issue:
      The header uses SOCKET but does not include <winsock2.h>. It currently
      relies on include order in source files.
    Fix:
      Include <winsock2.h> in transfer.h and ensure headers can be included
      independently.

[ ] P1-11: Check socket API errors and preserve useful diagnostics.
    Files: client.c, host.c
    Issue:
      Several failure messages are generic. Important Winsock errors are not
      consistently captured and reported.
    Fix:
      Immediately capture WSAGetLastError() after failed Winsock calls such
      as socket(), bind(), listen(), accept(), connect(), send(), and recv().

[ ] P1-12: Check send/receive function results at call sites.
    Files: client.c, host.c
    Issue:
      send_file() results are not consistently checked. A failed send should
      affect the session state rather than silently returning to the menu.
    Fix:
      Check every return value and handle disconnect/error paths.

============================================================
3. MEDIUM-PRIORITY DESIGN AND MAINTENANCE — P2
============================================================

[ ] P2-01: Remove duplicate HostConnection definition.
    Files: host.c, host.h
    Issue:
      HostConnection is defined in both the source and header.
    Fix:
      Keep the canonical typedef in host.h and include that header in host.c.

[ ] P2-02: Remove duplicate function prototypes.
    File: host.c
    Issue:
      cli_options() and input_handler() are declared twice in the source.
    Fix:
      Remove duplicate declarations and keep public declarations in headers.

[ ] P2-03: Separate networking, protocol, file I/O, and menu responsibilities.
    Files: host.c, client.c, transfer.c
    Issue:
      Menu handling, connection setup, data transfer, and cleanup are mixed.
    Fix:
      Refactor incrementally into focused modules after the current behavior
      is stable. Suggested modules: network/socket, connection, protocol,
      message, file I/O, and file transfer.

[ ] P2-04: Clarify function naming.
    File: transfer.c
    Issue:
      send_file() currently sends a text message rather than a file.
    Fix:
      Rename it to send_message() for the current behavior. Reserve
      send_file()/receive_file() for actual binary file transfer.

[ ] P2-05: Improve file selector behavior.
    File: transfer.c
    Issue:
      file_selecter() is currently not connected to the transfer workflow.
      The dialog filter allows all files, and the function name has a typo.
    Fix:
      Rename to file_selector(), decide allowed file types, handle dialog
      cancellation distinctly from errors, and free returned memory in the
      caller.

[ ] P2-06: Make configuration loading robust.
    File: main.c
    Issue:
      The settings path is relative to the process working directory, and
      configuration values are not validated. The max_connections value is
      printed with the label "Connection Timeout".
    Fix:
      Use a predictable config path, validate ranges and required fields,
      and correct the output label.

[ ] P2-07: Clarify Winsock ownership.
    Files: client.c, host.c
    Issue:
      WSAStartup()/WSACleanup() ownership is spread across functions.
    Fix:
      Define one clear lifecycle owner for each client or host session.
      Ensure each successful WSAStartup() has a matching WSACleanup().

[ ] P2-08: Improve IP input handling.
    File: client.c
    Issue:
      take_ip() uses a static 16-byte buffer and scanf(). It is suitable for
      a textual IPv4 address length but does not handle overlong input cleanly.
    Fix:
      Use fgets(), remove the newline, reject overlong input, and validate
      inet_pton() before attempting connect().

[ ] P2-09: Treat room codes as non-security features for now.
    File: host.c
    Issue:
      code_generator() uses rand(), and the generated code is not checked by
      the client or used to authorize a session.
    Fix:
      Do not describe the room code as authentication. If it becomes a
      security control, use a suitable cryptographically secure random
      source and implement server-side validation.

[ ] P2-10: Document current single-client behavior.
    File: host.c
    Issue:
      The host calls accept() once and handles one client. There is no
      accept loop or concurrent client handling.
    Fix:
      Document the current single-client limit. Add multiple-client support
      only if it is a project requirement.

[ ] P2-11: Avoid unnecessary system("cls") dependence.
    File: main.c
    Issue:
      system("cls") is Windows-specific and invokes a command processor.
    Fix:
      Keep it only if Windows-only behavior is intentional; otherwise remove
      it or isolate terminal clearing behind a platform-specific helper.

============================================================
4. FILE TRANSFER WORK — NOT YET IMPLEMENTED
============================================================

[ ] FT-01: Define a file-transfer protocol.
    Include message type, filename policy, file size, and transfer status.

[ ] FT-02: Send file metadata before file contents.

[ ] FT-03: Open files in binary mode and check fopen() errors.

[ ] FT-04: Read and send file data in bounded chunks.

[ ] FT-05: Receive data in a loop until the declared file size is reached.

[ ] FT-06: Write received data in binary mode and check fwrite() results.

[ ] FT-07: Validate filenames and prevent path traversal or overwriting
          unintended files.

[ ] FT-08: Handle zero-byte files, large files, disk-full errors, and
          interrupted transfers.

[ ] FT-09: Verify received byte count and report transfer success only
          after all expected bytes are written.

[ ] FT-10: Define how the receiver chooses the destination directory and
          resolves filename collisions.

============================================================
5. TEST PLAN
============================================================

[ ] TEST-01: Build with the project's strict warning flags.

[ ] TEST-02: Start host and connect with a valid IPv4 address.

[ ] TEST-03: Try malformed IPv4 input.

[ ] TEST-04: Try connecting while the host is offline.

[ ] TEST-05: Start a second host on the same port and verify bind failure
            is handled without entering the session menu.

[ ] TEST-06: Send messages in both directions repeatedly.

[ ] TEST-07: Test empty, short, maximum-size, and oversized messages.

[ ] TEST-08: Disconnect the host during a send and verify clean handling.

[ ] TEST-09: Disconnect the client and verify the host detects it.

[ ] TEST-10: Enter alphabetic and out-of-range menu input.

[ ] TEST-11: Test missing and malformed settings files.

[ ] TEST-12: Verify all sockets are closed and Winsock is cleaned up on
            normal exit and every error path.

[ ] TEST-13: Test file transfer with empty, small, and large files once
            file transfer is implemented.

[ ] TEST-14: Test transfer interruption and destination write failures.

============================================================
6. RECOMMENDED FIX ORDER
============================================================

Work in this order to avoid debugging too many layers at once:

[ ] STEP 1: Fix client SOCKET return values and connect error handling.
[ ] STEP 2: Add host connection validation and centralized cleanup.
[ ] STEP 3: Fix menu input handling and menu option mismatches.
[ ] STEP 4: Fix settings file NULL handling and parsing.
[ ] STEP 5: Define and implement one TCP message-framing protocol.
[ ] STEP 6: Implement receiving on the host and bidirectional messaging.
[ ] STEP 7: Test disconnects, invalid input, and oversized messages.
[ ] STEP 8: Refactor modules and headers.
[ ] STEP 9: Implement actual binary file transfer.
[ ] STEP 10: Test file transfer and harden error handling.

============================================================
REVIEW NOTES
============================================================

- This checklist is based on static inspection of the seven uploaded files.
- No successful or failed compiler build was observed during this review.
- Runtime scenarios are potential consequences inferred from the code paths,
  not claims that the application was executed and reproduced those failures.
- The current send_file() function sends a string/message; it does not yet
  implement binary file transfer.
- The current host accepts one client connection and does not implement
  multiple-client handling.