#ifndef CLIENT_H
#define CLIENT_H

#include <winsock2.h>

char *take_ip(void);
int validate_ip_address(const char *ip);
int initialize_winsock_client(void);
SOCKET client_session(void);
int connect_to_host(SOCKET client, char *serverIP);
SOCKET client_side_handler(void);
void client_side_menu(void);
char *get_host_ip(void);
#endif