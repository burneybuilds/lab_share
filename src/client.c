#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>


// Takes user_input to bind with the Ip / Host
char *take_ip(void){
    static char arr[16];
    printf("Enter the IPv4:  ");
    scanf("%15s", arr);
    return arr;
}

void join_connection(void)
{
    WSADATA wsaData;
    SOCKET TcpConnectionSocket;

    // Initialize the Winsock API
    int flag = WSAStartup(MAKEWORD(2, 2), &wsaData);
    
    if (flag != 0)
    {
        printf("Winsock initialization failed.\n");
        return;
    }
    
    // Create a TCP socket for the client
    TcpConnectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Initialize the server's socket address structure
    struct sockaddr_in serverAddress = {0};

    // Specify the IPv4 address family
    serverAddress.sin_family = AF_INET;

    // Set the destination port to 8080 
    serverAddress.sin_port = htons(8080);

    // Get the target host's IP address from the user
    char *serverIP = take_ip();

    // Convert the IP address from text to binary format
    if (inet_pton(AF_INET, serverIP, &serverAddress.sin_addr) != 1) {
        printf("Invalid IPv4 address.\n");
        return;
    }
    
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

