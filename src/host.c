#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "transfer.h"

// Prototypes.
int initialize_winsock(void);
SOCKET create_server_socket(void);
int bind_server_socket(SOCKET ListenSocket, unsigned short port);
int start_listening(SOCKET ListenSocket);
int server_side_handler(void);
int code_generator(void);
void cli_options(void);
int input_handler(int user_input);
void host_menu_handler(void);
void cli_options(void);
int input_handler(int user_input);
SOCKET accept_client(SOCKET ListenSocket);


int initialize_winsock(void){
    WSADATA wsaData;
    
    int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Check if the output of the WSA startup is not 0 .
    if (wsaResult != 0) {
        return 1;
    }
 
    return 0;
}

SOCKET create_server_socket(void){
    SOCKET ListenSocket = INVALID_SOCKET;
    
    ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (ListenSocket == INVALID_SOCKET) {
        return INVALID_SOCKET;
    }
    
    return ListenSocket;
}

int bind_server_socket(SOCKET ListenSocket, unsigned short port){
    struct sockaddr_in service = {0};

    service.sin_family = AF_INET;                // Assign the IP address type: IPv4.
    service.sin_port = htons(port);              // Assign the port number.
    service.sin_addr.s_addr = htonl(INADDR_ANY); // Accept connections on all local IPv4 interfaces.

    int error_flag = bind(ListenSocket, (struct sockaddr *)&service, sizeof(service));

    if (error_flag == SOCKET_ERROR) {
        return 1;
    }
    
    return 0;
}

int start_listening(SOCKET ListenSocket){
    
    int e_flag = listen(ListenSocket, SOMAXCONN);

    if (e_flag == SOCKET_ERROR) {
        return 1;
    }

    return 0;
}

SOCKET accept_client(SOCKET ListenSocket){
    SOCKET ClientSocket;
    
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        return INVALID_SOCKET;
    }

    return ClientSocket;
}

int server_side_handler(void){
    
    // 101 = WSA Failer.
    // 201 = Bind Failer. 
    // 301 || 302 = Socket Failer.
    // 401 = Listening Failer.

    int wsa_flag = initialize_winsock(); // initialize_winsock() Returns 1 if the WSA Fails.. 
    if (wsa_flag != 0){
        printf("[-] Winsock initialization failed.\n");
        return 101;
    }


    SOCKET ListenSocket = create_server_socket();
    if (ListenSocket ==  INVALID_SOCKET){
        printf("[-] Socket creation failed.\n");
        WSACleanup();
        return 301;
    }

    int bind_flag = bind_server_socket(ListenSocket, 8080);
    if (bind_flag != 0){
        printf("[-] bind failed with error.\n");
        closesocket(ListenSocket);
        WSACleanup();
        return 201;
    }


    int listen_flag = start_listening(ListenSocket);
    if (listen_flag != 0){
        printf("[-] listen failed with error.\n");
        closesocket(ListenSocket);
        WSACleanup();
        return 401;
    }

    printf("[+] Host Ready..\n[/]Waiting for a connection\n");

    SOCKET client_session = accept_client(ListenSocket);
    if (client_session == INVALID_SOCKET){
        printf("[-] accept failed with error.\n");
        closesocket(ListenSocket);
        WSACleanup();
        return 302;
    }
    
    printf("[+] Connection Establed.\n");

    // It just closed… just like your ex did. 
    closesocket(client_session);
    closesocket(ListenSocket);
    WSACleanup();


    return 0;
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


int input_handler(int user_input){        
    if (user_input != 1)
    {
        // Clear invalid input from the input buffer.
        int ch;

        // getchar() reads a character from the input stream and returns its integer value (ASCII value on ASCII-compatible systems).
        // This loop runs until a newline or EOF is encountered, clearing the remaining input.
        while ((ch = getchar()) != '\n' && ch != EOF)
        {
            // its empty..  i know i know 
        }
        return 1;
    }
    return 0;
}


void host_menu_handler(void){
   
    int user_input = 0;
    int room_code = code_generator();
    cli_options();
    server_side_handler();
    printf("IPV4 Addre: \n");
    printf("Room Code: %d\n", room_code);
    do
    {
        printf("<input> ");
        scanf("%d", &user_input);

        // Check the input stream for any errors.
        if ((input_handler(user_input)) == 1){
            printf("===================Error=========================\n");
            printf("Invalid input! Please enter a number.\n");
            printf("=================================================\n");
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