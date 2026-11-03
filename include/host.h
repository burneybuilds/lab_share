#ifndef HOST_H
#define HOST_H

#include <winsock2.h>
#include <windows.h>

// Store both sockets.
typedef struct {
    SOCKET client_socket;
    SOCKET listen_socket;
} HostConnection;

// Menu
void host_menu_handler(void);

// Room code (if you use it later).
int code_generator(void);

// Winsock initialization
int initialize_winsock(void);

// Server socket functions
SOCKET create_server_socket(void);
int bind_server_socket(SOCKET ListenSocket, unsigned short port);
int start_listening(SOCKET ListenSocket);
SOCKET accept_client(SOCKET ListenSocket);

// Connection management
HostConnection server_side_handler(void);
void close_connection(HostConnection connection);

#endif