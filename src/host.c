#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "transfer.h"

void create_connection(int close_connection_signal) {
    
    printf("[/] Searching for Connection.....\n");
    
    WSADATA wsaData;
    SOCKET ListenSocket; 
    SOCKET AcceptSocket;

    int iResult = 0;  

    // Check if the input is != 0.
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("[-] Winsock initialization failed.\n");
        WSACleanup();
    }
    else{
        printf("[+] Winsock initializationed.\n");
    }

    ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    // Creates a socket using the socket() function and stores the returned socket in ListenSocket.
    if (ListenSocket == INVALID_SOCKET)
    {
        printf("[-] Socket creation failed.\n");
        closesocket(ListenSocket);
        WSACleanup();
    }
    else{
        printf("[+] Socket created.\n");
    }

    struct sockaddr_in service = {0};

    // Assigns the IP address type: we are using IPv4.
    service.sin_family = htonl(INADDR_ANY);
    // Assigns the port number.
    service.sin_port = htons(8080);
    // Assigns the IP address.
    service.sin_addr.s_addr = inet_addr(INADDR_ANY);

    // After assigning the address information, bind it to the socket we created.
    
    iResult = bind(ListenSocket, (struct sockaddr *)&service, sizeof(service));
    if (iResult == SOCKET_ERROR) {
        wprintf(L"bind failed with error %u\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
    }
    else
        wprintf(L"bind returned success\n");

    // Listen for incoming connection requests on the socket.
    // listen(ListenSocket, SOMAXCONN);
        
    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
        wprintf(L"listen function failed with error: %d\n", WSAGetLastError());


    iResult = closesocket(ListenSocket);
    if (iResult == SOCKET_ERROR) {
        wprintf(L"closesocket function failed with error %d\n", WSAGetLastError());
        WSACleanup();
        
    }

    // Accepts an incoming connection on the listening socket and stores the
    // returned connected socket in AcceptSocket.
    AcceptSocket = accept(ListenSocket, NULL, NULL);
    // int flag = validate_incoming_connection(ListenSocket, AcceptSocket);

    if (AcceptSocket == SOCKET_ERROR){
        printf("[-] A Socket creation failed.\n");
        closesocket(AcceptSocket);
        closesocket(ListenSocket);
        WSACleanup();
    }
    
    if (close_connection_signal ==  3){
    // Cleanup
        printf("[+] Connection Closed.\n");
        closesocket(AcceptSocket);
        closesocket(ListenSocket);
        WSACleanup();
    }
}

// int validate_incoming_connection(SOCKET Listen, SOCKET accept){
//     return 0;
// }

// Generate a 4 Digits Random Code.
int code_generator(void){    
    int number = 1000 + rand() % 9000;
    return number ;
}

void cli_options(void){
    printf("==================Lab Share=====================\n");
    printf("1. Send File. \n");
    printf("2. Receive File. \n");
    printf("3. Back. \n");
    printf("================================================\n");
}


void host_menu_handler(void){
   
    int user_input = 0;
    int code = code_generator();
    cli_options();
    create_connection(user_input);
    printf("IPV4 Addre: \n");
    printf("Room Code: %d\n", code);
    do
    {
        printf("<input>");
        
        // Check the input stream for any errors.
        if (scanf("%d", &user_input) != 1)
        {
            system("cls");
            cli_options();
            printf("===================Error=========================\n");
            printf("Invalid input! Please enter a number.\n");
            printf("=================================================\n");

            // Clear invalid input from the input buffer.
            int ch;

            // getchar() reads a character from the input stream and returns its integer value (ASCII value on ASCII-compatible systems).
            // This loop runs until a newline or EOF is encountered, clearing the remaining input.
            while ((ch = getchar()) != '\n' && ch != EOF)
            {
            }

        }

        if (user_input == 1)
        {
            char *path = file_selecter(1);

            if (path != NULL)
            {
                printf("File selected: %s\n", path);

                // Next step: pass the path to your file-transfer function.
                // send_file(path);

                free(path);
            }
            else
            {
                printf("File selection cancelled or failed.\n");
                
            }
        }
        else if (user_input == 2)
        {
            printf("Waiting for the file......\n");
            // Next step: implement file receiving.
        }
        else if (user_input != 3)
        {
            printf("Invalid option. Please choose 1, 2, or 3.\n");
        }

    } while (user_input != 3);
}