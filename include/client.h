#ifndef CLIENT_H
#define CLIENT_H

#include <winsock2.h>

char *take_ip(void);
SOCKET client_side_handler(void);
int connect_to_host(SOCKET client);
SOCKET client_session();
int initialize_winsock_client(void);
void client_side_menu(void);

#endif