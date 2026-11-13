#include <stdio.h>
#include <stdbool.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include <windows.h>

#include "client.h"
#include "transfer.h"
#include "utils.h"
#include "validate.h"


// Prototypes.
int initialize_winsock_client(void);
char *take_ip(void);
SOCKET client_session();
int connect_to_host(SOCKET client,  char *serverIP);
SOCKET client_side_handler(void);
void client_side_menu();
char *get_host_ip(void);
unsigned int __stdcall client_receive_handler(void *arg);


/// @brief Initializes Winsock so the client can use network functions.
/// @param None
/// @return 0 if Winsock is initialized successfully, 1 if initialization fails.
int initialize_winsock_client(void)
{
    WSADATA wsaData;

    // Start Winsock version 2.2.
    int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Check if Winsock started successfully.
    if (wsaResult != 0) {
        return 1;
    }

    // Winsock is ready to use.
    return 0;
}

/// @brief Creates a TCP socket for the client to connect to a server.
/// @param None
/// @return The created socket if successful, INVALID_SOCKET if creation fails.
SOCKET client_session(void)
{
    // Create a TCP socket using IPv4.
    // AF_INET      -> Uses IPv4 addressing.
    // SOCK_STREAM  -> Uses TCP (reliable, connection-based communication).
    // IPPROTO_TCP  -> Specifies the TCP protocol.
    SOCKET TcpConnectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // Returns -> INVALID_SOCKET -> IF Failed..

    // Check if the socket was created successfully.
    if (TcpConnectionSocket == INVALID_SOCKET) {
        return INVALID_SOCKET;
    }

    // Return the socket so it can be used for connection.
    return TcpConnectionSocket;
}


/// @brief Takes the host's IPv4 address as input from the user.
/// @param None
/// @return A pointer to the first character of the stored IP address.
char *take_ip(void)
{
    static char arr[16];

    printf("Enter the Host IP:  ");

    scanf("%15s", arr);

    return arr; // This is Returning a pointer to the first character of the array.
}


/// @brief Takes a valid IPv4 address from the user.
/// @param None
/// @return 0 when a valid IP is entered, 1 when the user chooses to exit.
char *get_host_ip(void)
{
    while (true)
    {
        waiting_for_connection();
        char *serverIP = take_ip();

        if (strcmp(serverIP, "1") == 0)
        {
            return NULL;
        }

        if (validate_ip_address(serverIP) == 0)
        {
            return serverIP;
        }

        printf("[-] Invalid IPv4 address. Try again, Sherlock.\n");
        printf("Press Enter.........\n");
        getchar();
        getchar();
        system("cls");
    }
}

/// @brief Establishes a TCP connection with the specified host.
/// @param client The client's socket.
/// @param serverIP The IPv4 address of the host.
/// @return 0 on success, SOCKET_ERROR on connection failure.
int connect_to_host(SOCKET client, char *serverIP)
{
    struct sockaddr_in serverAddress = {0};

    serverAddress.sin_family = AF_INET;   // Set the address family to IPv4.
    serverAddress.sin_port = htons(8080); // Set the host's port number to 8080.
    inet_pton(AF_INET, serverIP, &serverAddress.sin_addr); // Convert the IP address from string format to binary format.

    // Attempt to connect to the host.
    int connection_pool = connect(client, (SOCKADDR *)&serverAddress, sizeof(serverAddress));

    if (connection_pool == SOCKET_ERROR)
    {
        return SOCKET_ERROR;
    }

    return connection_pool;
}

/// @brief Initializes Winsock, creates a client socket, and connects to the host.
/// @param NULL
/// @return client -> SOCKET || INVALID_SOCKET.
SOCKET client_side_handler(void)
{
    // Let's get this thing online.
    int flag = initialize_winsock_client();
    if (flag != 0)
    {
        printf("[-] Failed to initialize Winsock.\n");
        return INVALID_SOCKET;
    }

    // Time to make a socket. No socket, no talking.
    // Yeah, you're too alone, so you're now watching sockets talk to each other.
    SOCKET client = client_session();
    if (client == INVALID_SOCKET){
        printf("[-] Failed to create client socket.\n");
        WSACleanup();
        return INVALID_SOCKET;
    }
    char *serverIP = get_host_ip();

    if (serverIP == NULL)
    {
        closesocket(client);
        WSACleanup();
        return INVALID_SOCKET;
    }

     // Let's see if the host picks up the phone because your crush isn't picking up the call...
    int connection_flag = connect_to_host(client, serverIP);
    if (connection_flag == SOCKET_ERROR) {
        printf("[-] Failed to connect to the host.\n");
        closesocket(client);
        WSACleanup();
        return INVALID_SOCKET;
    }
    return client;
}


unsigned int __stdcall client_receive_handler(void *arg)
{
    SOCKET server_socket = *(SOCKET *)arg;
    char buffer[1024];

    while (1)
    {
        int result = receive_message(
            server_socket,
            buffer,
            sizeof(buffer)
        );

        // Stop receiving if an error or disconnection occurs.
        if (result != 0)
        {
            break;
        }
    }

    return 0;
}


/// @brief Handles the client-side menu and user interactions.

void client_side_menu(void)
{
    int user_input = 0;

    // Initialize the client and establish a connection.
    SOCKET client = client_side_handler();

    // Check whether the connection failed.
    if (client == INVALID_SOCKET)
    {
        return;
    }

    // Start the receiver thread.
    unsigned int thread_id;

    HANDLE receiver_thread = (HANDLE)_beginthreadex(
        NULL,
        0,
        client_receive_handler,
        &client,
        0,
        &thread_id
    );

    if (receiver_thread == NULL)
    {
        printf("[-] Failed to create receiver thread.\n");
        closesocket(client);
        WSACleanup();
        return;
    }

    // Display the client menu.
    system("cls");
    cli_options();

    // Keep displaying the menu until the user exits.
    while (user_input != 3)
    {
        char message[1024];

        printf("You: ");

        if (fgets(message, sizeof(message), stdin) == NULL)
        {
            error_message();
            continue;
        }

        // Remove the trailing newline.
        message[strcspn(message, "\n")] = '\0';

        // Send the message to the host.
        if (send_message(client, message) != 0)
        {
            error_message();
        }

        // Option 1: Send a message to the host.
        if (user_input == 1)
        {
        }
        else if (user_input == 2)
        {
            // Return to the main menu.
            break;
        }
    }

    // Close the connection and stop the receiver thread.
    printf("[+] Closing connection...\n");

    shutdown(client, SD_BOTH);

    WaitForSingleObject(receiver_thread, INFINITE);
    CloseHandle(receiver_thread);

    closesocket(client);
    WSACleanup();
}
