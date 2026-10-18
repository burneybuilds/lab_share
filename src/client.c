#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

void join_connection(void)
{
    WSADATA wsaData;
    SOCKET TcpConnectionSocket;

    // Initialize the Winsock API
    int flag = WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Create a TCP socket for the client
    TcpConnectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Initialize the server's socket address structure
    struct sockaddr_in serverAddress = {0};

    // Specify the IPv4 address family
    serverAddress.sin_family = AF_INET;

    // Set the destination port to 8080 
    serverAddress.sin_port = htons(8080);

    // Get the target host's IP address from the user
    // char serverIP[16] = take_ip();

    // Convert the IP address from text to binary format
    inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);

    // Attempt to establish a TCP connection with the host
    int connection_pool = connect(TcpConnectionSocket, (SOCKADDR *)&serverAddress, sizeof(serverAddress));

    // Check whether the connection attempt failed
    if (connection_pool == SOCKET_ERROR) {
        printf("Something went wrong while connecting to the host.\n");
    }

    // Close the client socket
    closesocket(TcpConnectionSocket);

    // Clean up Winsock resources
    WSACleanup();
}

// char take_ip(){
//     char user_input[16]; 
    
//     // Ask the user to enter the host's IPv4 address
//     printf("Enter the host IP address: ");
//     scanf("%15s", user_input);
    
//     return user_input;
// }