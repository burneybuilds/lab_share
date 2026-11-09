#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

void cli_options(void);
void waiting_for_connection(void);
void main_menu(void);
void error_message(void);
char *get_local_ip(void);
int code_generator(void);

void cli_options(void){
    printf("==================Lab Share=====================\n");
    printf("1. Send File. \n");
    printf("2. Back. \n");
    printf("================================================\n");
}

void waiting_for_connection(void){
    printf("==================Lab Share=====================\n");
    printf("Waiting for Connection\n");
    printf("Press 1 to Go back.\n");
    printf("================================================\n");
}

void main_menu(void){
    printf("===================Lab Share====================\n");
    printf("1. Host a Session\n");
    printf("2. Connect to a Session\n");
    printf("3. Settings.\n");
    printf("4. Exit.\n");
    printf("=================================================\n");
}

void error_message(void){
    printf("===================Error=========================\n");
    printf("Invalid input! Please enter a number.\n");
    printf("Press Enter to Continue...\n");
    printf("=================================================\n");
}

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


// Generate a 4 Digits Random Code.
int code_generator(void)
{
    int number = 1000 + rand() % 9000;
    return number;
}