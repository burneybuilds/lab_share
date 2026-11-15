#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include <windows.h>

#include "transfer.h"
#include "utils.h"

typedef struct
{
    SOCKET client_socket;
    SOCKET listen_socket;
} HostConnection;

int initialize_winsock(void);
SOCKET create_server_socket(void);
int bind_server_socket(SOCKET ListenSocket, unsigned short port);
int start_listening(SOCKET ListenSocket);
SOCKET accept_client(SOCKET ListenSocket);

HostConnection server_side_handler(void);
unsigned int __stdcall receive_handler(void *arg);
void host_menu_handler(void);


// Start the Winsock library so the program can use Windows socket functions.
// Return 1 if Winsock fails to initialize, otherwise return 0.
int initialize_winsock(void)
{
    WSADATA wsaData;

    int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Check if the output of the WSAstartup is not 0 .
    if (wsaResult != 0)
    {
        return 1;
    }

    return 0;
}


// Creates a TCP socket that the server will use to listen for incoming client connections.
// AF_INET specifies IPv4, SOCK_STREAM specifies a reliable connection-based socket,
// and IPPROTO_TCP specifies the TCP protocol.
// Returns the created socket if successful, otherwise returns INVALID_SOCKET.
SOCKET create_server_socket(void)
{
    SOCKET ListenSocket = INVALID_SOCKET;

    ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (ListenSocket == INVALID_SOCKET)
    {
        return INVALID_SOCKET;
    }

    return ListenSocket;
}


// Binds the server socket to a specific port and all available local IPv4 interfaces.
// This allows clients to reach the server using its IP address and port.
int bind_server_socket(SOCKET ListenSocket, unsigned short port)
{
    struct sockaddr_in service = {0};  // Stores the IPv4 address and port information required by bind().

    service.sin_family = AF_INET; // Specifies that the server will use IPv4 addresses.
    service.sin_port = htons(port); // Converts the port number from host byte order to network byte order.
    service.sin_addr.s_addr = htonl(INADDR_ANY); // Allows the server to accept connections through all available local IPv4 interfaces.

    // Binds the socket to the specified IP address and port.
    int error_flag = bind(ListenSocket, (struct sockaddr *)&service, sizeof(service));
    if (error_flag == SOCKET_ERROR) // bind() returns SOCKET_ERROR if the binding operation fails.
    {
        return 1;
    }

    return 0;  // Socket successfully bound to the specified port.
}


// Puts the server socket into listening mode so it can wait for incoming client connections.
// SOMAXCONN allows Windows to use the maximum reasonable pending connection queue size.
int start_listening(SOCKET ListenSocket)
{
    // Start listening for incoming connections on the server socket.
    int e_flag = listen(ListenSocket, SOMAXCONN);
    if (e_flag == SOCKET_ERROR) // listen() returns SOCKET_ERROR if the operation fails.
    {
        return 1;
    }

    return 0; // Socket successfully entered listening mode.
}


// Accepts an incoming connection request from a client.
// Returns a new socket for communicating with the connected client,
// or INVALID_SOCKET if the connection cannot be accepted.
SOCKET accept_client(SOCKET ListenSocket)
{
    SOCKET ClientSocket;

    // Wait for an incoming client connection and accept it.
    // NULL arguments mean we don't need to retrieve the client's IP address or port.
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) // Check whether the connection was accepted successfully.
    {
        return INVALID_SOCKET;
    }

    return ClientSocket; // Return the new socket used to communicate with the client.
}


// Initializes the server, prepares it to accept incoming connections,
// and establishes a connection with a client.
// Returns a HostConnection containing the listening socket and client socket.
// Both sockets remain INVALID_SOCKET if the setup or connection fails.
HostConnection server_side_handler(void)
{
    // Initialize both sockets as INVALID_SOCKET.
    // This helps us identify whether socket creation or connection was successful.
    HostConnection connection;

    connection.client_socket = INVALID_SOCKET;
    connection.listen_socket = INVALID_SOCKET;

    // Initialize Winsock so the program can use Windows networking functions.
    // Returns 1 if initialization fails, otherwise returns 0.
    int wsa_flag = initialize_winsock();
    if (wsa_flag != 0)
    {
        printf("[-] Winsock initialization failed.\n");
        return connection;
    }

    // Create the server socket that will wait for incoming client connections.
    connection.listen_socket = create_server_socket();
    if (connection.listen_socket == INVALID_SOCKET)
    {
        printf("[-] Socket creation failed.\n");

        // Release Winsock resources because initialization was successful.
        WSACleanup();
        return connection;
    }

    // Bind the server socket to port 8080 and all available local IPv4 interfaces.
    // This allows clients to reach the server through its local IP address.
    int bind_flag = bind_server_socket(connection.listen_socket, 8080);
    if (bind_flag != 0)
    {
        printf("[-] bind failed with error.\n");

        // Close the created socket and release Winsock resources.
        closesocket(connection.listen_socket);
        WSACleanup();
        return connection;
    }

    // Put the server socket into listening mode so it can wait for client connections.
    int listen_flag = start_listening(connection.listen_socket);
    if (listen_flag != 0)
    {
        printf("[-] listen failed with error.\n");

        // Close the socket and release Winsock resources.
        closesocket(connection.listen_socket);
        WSACleanup();
        return connection;
    }

    // Fetch the local IPv4 address so the user can see
    // which address other devices on the network can use to connect.
    char *ip = get_local_ip();
    if (ip != NULL)
    {
        printf("IPv4 Address: %s\n", ip);
    }
    else
    {
        printf("[-] Could not fetch IPv4 address.\n");
    }

    // Notify the user that the server is ready and waiting for a client.
    printf("[+]Host Ready....\n[/]Waiting for a connection\n");

    // Wait for an incoming client connection.
    // This call blocks until a client connects or an error occurs.
    // On success, it returns a new socket for communicating with that client.
    connection.client_socket = accept_client(connection.listen_socket);

    if (connection.client_socket == INVALID_SOCKET)
    {
        printf("[-] accept failed with error.\n");

        closesocket(connection.listen_socket); // Close the listening socket and release Winsock resources.
        WSACleanup();
        return connection;
    }

    printf("[+] Connection Established.\n");

    // A client has successfully connected to the server.
    // Return both sockets to the caller.
    return connection;
}


unsigned int __stdcall receive_handler(void *arg)
{
    SOCKET client_socket = *(SOCKET *)arg;
    char buffer[1024];

    while (1)
    {
        int result = receive_message(client_socket, buffer, sizeof(buffer));

        // Stop receiving if an error or disconnection occurs.
        if (result != 0){
            break;
        }
    }

    return 0;
}


// Handles the host-side menu and user interactions after starting the server.
// Establishes a connection with a client and allows the host to send messages.
void host_menu_handler(void)
{
    char message[1024] = "";               // Stores the user's menu selection.
    unsigned int thread_id;           // Start the receiver thread.
    int room_code = code_generator(); // Generates a unique room code.

    waiting_for_connection(); // waiting screen.

    printf("Room Code: %d\n", room_code);

    // Initialize the server and wait for a client.
    HostConnection connection = server_side_handler();

    if (connection.client_socket == INVALID_SOCKET)
    {
        printf("Oops! Something went wrong.\n");
        printf("Press Enter to continue.\n");
        getchar();
        getchar();
        return;
    }

    HANDLE receiver_thread = (HANDLE)_beginthreadex(
        NULL,
        0,
        receive_handler,
        &connection.client_socket,
        0,
        &thread_id
    );

    if (receiver_thread == NULL)
    {
        printf("[-] Failed to create receiver thread.\n");

        closesocket(connection.client_socket);
        closesocket(connection.listen_socket);
        WSACleanup();
        return;
    }

    system("cls");
    cli_options(); // Display the menu once and keep it visible while the user interacts

    // Keep accepting menu selections until the user exits.
    while(1){
        printf("You:  ");

        if (fgets(message, sizeof(message), stdin) == NULL)
        {
            error_message();
            printf("[-] An Error occurred.\n");
            continue;
        }

        // Remove the trailing newline.
        message[strcspn(message, "\n")] = '\0';

        if (strcmp(message, "1") == 0)
        {

            // printf("Hii\n"); why hii idk why it was just there to check if thing work so.
            break;
        }

        // Send the message through the connected socket.
        if (send_message(connection.client_socket, message) != 0)
        {
            error_message(); // This just displays the error message not extra rocket sc.
        }
    }

    // Stop the receiver thread before cleaning up.
    shutdown(connection.client_socket, SD_BOTH);

    // omg this was the bug i was fighting for such a long timeeeee......
    closesocket(connection.client_socket);
    connection.client_socket = INVALID_SOCKET;

    WaitForSingleObject(receiver_thread, INFINITE);
    CloseHandle(receiver_thread);

    // Release networking resources.
    closesocket(connection.listen_socket);

    WSACleanup();

    return;
}
