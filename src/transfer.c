#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <commdlg.h>

#include "host.h"

// Prototypes.

int receive_message(SOCKET connection_session, char *buffer, int buffer_size);
int send_message(SOCKET connection_session, const char *input_buffer);


int send_message(SOCKET connection_session, const char *input_buffer)
{
    int total_sent = 0;
    int message_length = (int)strlen(input_buffer);
    int total_length = message_length + 1;
    char newline = '\n';

    while (total_sent < total_length)
    {
        const char *data = total_sent < message_length
            ? input_buffer + total_sent
            : &newline;
        int remaining = total_sent < message_length
            ? message_length - total_sent
            : 1;

        int bytes_sent = send(
            connection_session,
            data,
            remaining,
            0
        );

        // Check whether sending failed.
        if (bytes_sent == SOCKET_ERROR)
        {
            printf("[-] Send failed: %d\n", WSAGetLastError());
            return 1;
        }

        // Check whether the connection was closed.
        if (bytes_sent == 0)
        {
            printf("[-] Connection closed while sending.\n");
            return 1;
        }

        total_sent += bytes_sent;
    }

    printf("[+] Message sent successfully.\n");
    return 0;
}


int receive_message(SOCKET connection_session, char *buffer, int buffer_size)
{
    int total_received = 0;
    char ch;

    // Validate the buffer.
    if (buffer == NULL || buffer_size <= 0)
    {
        return 1;
    }

    while (total_received < buffer_size - 1)
    {
        int bytes_received = recv(
            connection_session,
            &ch,
            1,
            0
        );

        // Check whether receiving failed.
        if (bytes_received == SOCKET_ERROR)
        {
            printf("[-] Receive failed: %d\n", WSAGetLastError());
            buffer[total_received] = '\0';
            return 1;
        }

        // Check whether the peer disconnected.
        if (bytes_received == 0)
        {
            printf("[-] Connection closed by peer.\n");
            buffer[total_received] = '\0';
            return 2;
        }

        // Stop receiving when a newline is encountered.
        if (ch == '\n')
        {
            buffer[total_received] = '\0';
            printf("Client: %s\n", buffer);
            return 0;
        }

        buffer[total_received++] = ch;
    }

    // Buffer filled before receiving a complete message.
    buffer[total_received] = '\0';

    printf("[-] Message exceeds buffer capacity.\n");
    return 3;
}
