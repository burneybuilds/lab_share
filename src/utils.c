#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

void cli_options(void);
void waiting_for_connection(void);
void main_menu(void);
void error_message(void);
void disconnect_message(void);
char *get_local_ip(void);
int code_generator(void);
char *file_selector(void);

// Displays the file selection menu.
void cli_options(void){
    printf("==================Lab Share=====================\n");
    printf("1. Send File.\n");
    printf("2. Back.\n");
    printf("================================================\n");
}

// Displays the connection waiting screen.
void waiting_for_connection(void){
    printf("==================Lab Share=====================\n");
    printf("Waiting for Connection...\n");
    printf("Press 1 to go back.\n");
    printf("================================================\n");
}

// Displays the main application menu.
void main_menu(void){
    printf("===================Lab Share====================\n");
    printf("1. Host a Session\n");
    printf("2. Connect to a Session\n");
    printf("3. Settings\n");
    printf("4. Exit\n");
    printf("=================================================\n");
}

// Displays an error message for invalid user input.
void error_message(void){
    printf("===================Error=========================\n");
    printf("Invalid input! Please enter a valid number.\n");
    printf("Press Enter to continue...\n");
    printf("=================================================\n");
}

// Displays an disconnect message for invalid user input.
void disconnect_message(void){
    system("cls");
    printf("================== Disconnected ==================\n");
    printf("The peer has disconnected from the session.\n");
    printf("Press Enter to return to the main menu...\n");
    printf("==================================================\n");

}

// Generate a 4 Digits Random Code.
int code_generator(void)
{
    int number = 1000 + rand() % 9000;
    return number;
}


// Retrieves the local IPv4 address associated with the host machine.
// Returns the IP address as a string, or NULL if retrieval fails.
char *get_local_ip(void)
{
    char hostname[256];

    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR)
    {
        return NULL;
    }

    struct addrinfo hints = {0};
    struct addrinfo *result = NULL;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(hostname, NULL, &hints, &result) != 0)
    {
        return NULL;
    }

    static char ip[INET_ADDRSTRLEN];

    // Iterate through the available addresses and skip loopback addresses.
    for (struct addrinfo *ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {
        struct sockaddr_in *address = (struct sockaddr_in *)ptr->ai_addr;

        if (address->sin_addr.s_addr != htonl(INADDR_LOOPBACK))
        {
            inet_ntop(AF_INET, &address->sin_addr, ip, sizeof(ip));

            freeaddrinfo(result);
            return ip;
        }
    }

    freeaddrinfo(result);
    return NULL;
}



/**
 * @brief Opens a Windows file dialog to select a file.
 * @param user_input Opens the dialog if set to 1.
 * @return Allocated file path on success, otherwise NULL.
 * @note The caller is responsible for freeing the returned pointer.
 */
char *file_selector(void)
{
    // File pointers point toward files, unlike your crush. :')
    char filePath[MAX_PATH] = {0};

    OPENFILENAMEA ofn = {0};

    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = "All Files (*.*)\0*.*\0";
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;


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

    return NULL;
}
