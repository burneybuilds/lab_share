#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// Prototypes.
char *take_ip(void);
int validate_ip_address(const char *ip);
int initialize_winsock_client(void);
SOCKET client_session();
int connect_to_host(SOCKET client);
void client_side_handler(void);

// Takes user_input to bind with the Ip / Host
char *take_ip(void){
    static char arr[16];
    printf("Enter the IPv4:  ");
    scanf("%15s", arr);
    return arr;
}


int validate_ip_address(const char *ip)
{
    struct in_addr address;

    int flag = inet_pton(AF_INET, ip, &address);

    if (flag != 1) {
        return 1; // Invalid IP. Don't worry, this isn't a red flag like your ex.
    }

    return 0; // Valid IP. Green flag!
}


int initialize_winsock_client(void){
    WSADATA wsaData;
    
    int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Check if the output of the WSA startup is not 0 .
    if (wsaResult != 0) {
        return 1;
    }
 
    return 0;
}

SOCKET client_session(){
    // Creates a socket.
    SOCKET TcpConnectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // checks if the socket is inalid and return.
    if (TcpConnectionSocket == INVALID_SOCKET){
        return INVALID_SOCKET;
    }
    
    return TcpConnectionSocket;
}


int connect_to_host(SOCKET client){
    
    int flag = 1;
    char *serverIP;
    while(flag == 1){
        serverIP = take_ip();

        if (validate_ip_address(serverIP) == 0) {
            flag = 0;
        }
    }

    struct sockaddr_in serverAddress = {0};   // Initialize the server's socket address structure.
    serverAddress.sin_family = AF_INET;       // Specify the IPv4 address family.
    serverAddress.sin_port = htons(8080);     // Set the destination port to 8080.
    inet_pton(AF_INET, serverIP, &serverAddress.sin_addr);
    
    int connection_pool = connect(client, (SOCKADDR *)&serverAddress, sizeof(serverAddress));
    if (connection_pool == SOCKET_ERROR){
        return 2;
    }

    return 0;
}


void client_side_handler(void)
{
    // Let's get this thing online.
    int flag = initialize_winsock_client();
    if (flag != 0)
    {
        printf("[-] Failed to initialize Winsock.\n");
        return;
    }

    // Time to make a socket. No socket, no talking.
    // Yeah, you're too alone, so you're now watching sockets talk to each other.
    SOCKET client = client_session();
    if (client == INVALID_SOCKET){
        printf("[-] Failed to create client socket.\n");
        WSACleanup();
        return;
    }

    // Let's see if the host picks up the phone because your crush isn't picking up the call...
    int connection_flag = connect_to_host(client);

    // Boom, the connection failed, just like your last relationship....
    if (connection_flag == 2) {
        printf("[-] Failed to connect to the host.\n");
        closesocket(client);
        WSACleanup();
        return;
    }
    // Boom, you got dumb...
    else if (connection_flag == 1){
        printf("[-] Invalid IPv4 address. Try again, Sherlock.\n");
        closesocket(client);
        WSACleanup();
        return;
    }
    else{
        printf("[+] Successfully connected to the host!\n");
        getchar();
        getchar();
    }

    // If you're wondering why 2 comes before 1, the answer is honestly, I don't know.....
    // getchar();
    // getchar();
    // All that work just to say hello. Classic.
    closesocket(client);
    WSACleanup();
}
