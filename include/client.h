#ifndef CLIENT_H
#define CLIENT_H

char *take_ip(void);
void client_side_handler(void);
int connect_to_host(SOCKET client);
SOCKET client_session();
int initialize_winsock_client(void);

#endif