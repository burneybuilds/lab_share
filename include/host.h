#ifndef RANDOM_H
#define RANDOM_H

void host_menu_handler(void);

int code_generator(void);

int initialize_winsock(void);

SOCKET create_server_socket(void);

int bind_server_socket(SOCKET ListenSocket, unsigned short port);

int start_listening(SOCKET ListenSocket);

SOCKET accept_client(SOCKET ListenSocket);

int handle_client(SOCKET ClientSocket);

void close_connection(SOCKET ClientSocket);

void cleanup_server(SOCKET ListenSocket);

int run_server(void);
#endif