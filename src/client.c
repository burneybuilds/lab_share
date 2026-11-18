
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


// Function prototypes.
int initialize_winsock_client(void);
char *take_ip(void);
SOCKET client_session(void);
int connect_to_host(SOCKET client, char *serverIP);
SOCKET client_side_handler(void);
void client_side_menu(void);
char *get_host_ip(void);
unsigned int __stdcall client_receive_handler(void *arg);


// Initializes Winsock for client-side network communication.
// Returns 0 on success, otherwise 1.
int initialize_winsock_client(void)
{
    WSADATA wsaData;

    // Start Winsock version 2.2.
    int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Check whether Winsock initialized successfully.
    if (wsaResult != 0) {
        return 1;
    }

    // Winsock is ready to use.
    return 0;
}


// Creates a TCP socket for the client to connect to the host.
// Returns the created socket on success, otherwise INVALID_SOCKET.
SOCKET client_session(void)
{
    // Create a TCP socket using IPv4.
    // AF_INET      -> Uses IPv4 addressing.
    // SOCK_STREAM  -> Uses TCP (reliable, connection-based communication).
    // IPPROTO_TCP  -> Specifies the TCP protocol.
    SOCKET tcp_connection_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Check whether the socket was created successfully.
    if (tcp_connection_socket == INVALID_SOCKET) {
        return INVALID_SOCKET;
    }

    // Return the socket so it can be used for connection.
    return tcp_connection_socket;
}


// Takes the host's IPv4 address as input from the user.
// Returns a pointer to the stored IP address.
char *take_ip(void)
{
    static char arr[16];

    printf("Enter the Host IP:  ");

    scanf("%15s", arr);

    return arr; // Returns a pointer to the stored IP address.
}


// Retrieves a valid IPv4 address from the user.
// Returns the IP address on success, or NULL if the user chooses to exit.
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


// Establishes a TCP connection with the specified host.
// @param client The client's socket.
// @param serverIP The host's IPv4 address.
// Returns 0 on success, SOCKET_ERROR on failure.
int connect_to_host(SOCKET client, char *serverIP)
{
    struct sockaddr_in server_address = {0};

    server_address.sin_family = AF_INET;   // Set the address family to IPv4.
    server_address.sin_port = htons(8080); // Set the host's port number to 8080.

    // Validates and converts the IPv4 address to binary network format.
    inet_pton(AF_INET, serverIP, &server_address.sin_addr);

    // Attempt to connect to the host.
    int connection_result = connect(
        client,
        (SOCKADDR *)&server_address,
        sizeof(server_address)
    );

    if (connection_result == SOCKET_ERROR)
    {
        return SOCKET_ERROR;
    }

    return connection_result;
}


// Initializes Winsock, creates a client socket, and connects to the host.
// Returns the connected client socket on success, otherwise INVALID_SOCKET.
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

    if (client == INVALID_SOCKET)
    {
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


// Handles incoming messages from the host.
// Stops when an error occurs or the connection is closed.
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


// Handles the client-side menu and user interactions.
void client_side_menu(void)
{
    unsigned int thread_id;   // Stores the receiver thread's ID.
    char message[1024];

    // Initialize the client and establish a connection.
    SOCKET client = client_side_handler();

    // Check whether the connection failed.
    if (client == INVALID_SOCKET) {
        return;
    }

    HANDLE receiver_thread = (HANDLE)_beginthreadex(
        NULL,
        0,
        client_receive_handler,
        &client,
        0,
        &thread_id
    );

    HANDLE handles[2];

    handles[0] = receiver_thread;
    handles[1] = GetStdHandle(STD_INPUT_HANDLE);

    if (receiver_thread == NULL) {
        printf("[-] Failed to create receiver thread.\n");
        closesocket(client);
        WSACleanup();
        return;
    }


    // Display the client menu.
    system("cls");
    cli_options();

    // Keep displaying the menu until the user exits.
    while (1)
    {
        DWORD wait_result = WaitForMultipleObjects(
                2,
                handles,
                FALSE,
                INFINITE
        );

        if (wait_result == WAIT_OBJECT_0){
            disconnect_message();
            getchar();
            getchar();
            break;
        }

        printf("You: ");

        if (fgets(message, sizeof(message), stdin) == NULL)
        {
            error_message();
            continue;
        }

        // Remove the trailing newline.
        message[strcspn(message, "\n")] = '\0';

        if (strcmp(message, "1") == 0) {
            break;
        }

        // Send the message to the host.
        if (send_message(client, message) != 0) {
            error_message();
        }
    }

    // Close the connection and stop the receiver thread.
    printf("[+] Closing connection...\n");

    shutdown(client, SD_BOTH);
    closesocket(client);

    WaitForSingleObject(receiver_thread, INFINITE);
    CloseHandle(receiver_thread);

    WSACleanup();
    return;
}
