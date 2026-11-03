#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <commdlg.h>

#include "host.h"

// Prototypes.
char *file_selecter(int user_input);
int send_file(SOCKET connection_session, const char *input_buffer);
int receive_message(SOCKET connection_session, char *buffer, int buffer_size);


/**
 * @brief Opens the Windows file dialog to select a file.
 * @param user_input Opens the dialog when the value is 1.
 * @return Dynamically allocated file path on success, otherwise NULL.
 * @note The caller must free the returned pointer.
 */
char *file_selecter(int user_input)
{
    // File pointers point toward files, unlike your crush. :')
    char filePath[MAX_PATH] = {0};

    OPENFILENAMEA ofn = {0};

    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = "All Files (*.*)\0*.*\0";
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

    if (user_input == 1)
    {
        if (GetOpenFileNameA(&ofn))
        {
            char *selectedPath = malloc(strlen(filePath) + 1);

            if (selectedPath == NULL)
                return NULL;

            strcpy(selectedPath, filePath);

            // printf("Selected file: %s\n", selectedPath);
            return selectedPath;
        }
        else
        {
            printf("No file selected.\n");
            return NULL;
        }
    }

    return NULL;
}

int send_file(SOCKET connection_session, const char *input_buffer)
{
    int total_sent = 0;
    int message_length = (int)strlen(input_buffer);

    while (total_sent < message_length)
    {
        int iResult = send(connection_session, input_buffer + total_sent, message_length - total_sent, 0);

        if (iResult == SOCKET_ERROR)
        {
            printf("Send failed: %d\n", WSAGetLastError());
            return 1;
            
        }

        if (iResult == 0)
        {
            printf("Connection closed while sending.\n");
            return 1;
        }

        total_sent += iResult;
    }

    printf("\n[+] Message sent successfully.\n");
    return 0;
}

int receive_message(SOCKET connection_session, char *buffer, int buffer_size)
{
    int total_received = 0;
    char ch;

    while (total_received < buffer_size - 1)
    {
        int result = recv(connection_session, &ch, 1, 0);

        if (result == SOCKET_ERROR)
        {
            printf("Receive failed: %d\n", WSAGetLastError());
            return 1;
        }

        if (result == 0)
        {
            printf("Client disconnected.\n");
            return 2;
        }

        if (ch == '\n')
        {
            break;
        }

        buffer[total_received++] = ch;
    }

    buffer[total_received] = '\0';

    printf("Client: %s\n", buffer);

    return 0;
}