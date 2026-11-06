
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "transfer.h"


void create_test_connection(SOCKET *client, SOCKET *server)
{
    SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    assert(listener != INVALID_SOCKET);

    struct sockaddr_in address = {0};

    address.sin_family = AF_INET;
    address.sin_port = htons(0);
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    assert(bind(
        listener,
        (struct sockaddr *)&address,
        sizeof(address)
    ) == 0);

    assert(listen(listener, SOMAXCONN) == 0);

    int address_length = sizeof(address);

    assert(getsockname(
        listener,
        (struct sockaddr *)&address,
        &address_length
    ) == 0);

    *client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    assert(*client != INVALID_SOCKET);

    assert(connect(
        *client,
        (struct sockaddr *)&address,
        sizeof(address)
    ) == 0);

    *server = accept(listener, NULL, NULL);

    assert(*server != INVALID_SOCKET);

    closesocket(listener);
}


void test_send_and_receive_message(void)
{
    printf("\n[TEST] Message Transfer\n");

    SOCKET client;
    SOCKET server;

    create_test_connection(&client, &server);

    char received_message[1024];

    // Client sends a message.
    assert(send_file(client, "Hello from client!\n") == 0);

    assert(receive_message(
        server,
        received_message,
        sizeof(received_message)
    ) == 0);

    assert(strcmp(received_message, "Hello from client!") == 0);

    // Server sends a message.
    assert(send_file(server, "Hello from server!\n") == 0);

    assert(receive_message(
        client,
        received_message,
        sizeof(received_message)
    ) == 0);

    assert(strcmp(received_message, "Hello from server!") == 0);

    closesocket(client);
    closesocket(server);

    printf("[PASS] Message Transfer\n");
}


void test_client_disconnect(void)
{
    printf("\n[TEST] Client Disconnection\n");

    SOCKET client;
    SOCKET server;

    create_test_connection(&client, &server);

    closesocket(client);

    char received_message[1024];

    int result = receive_message(
        server,
        received_message,
        sizeof(received_message)
    );

    assert(result == 2);

    closesocket(server);

    printf("[PASS] Client Disconnection\n");
}


int main(void)
{
    printf("\n========== TRANSFER TESTS ==========\n");

    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("[FAIL] Winsock initialization\n");
        return 1;
    }

    test_send_and_receive_message();
    test_client_disconnect();

    WSACleanup();

    printf("\n========== TRANSFER TESTS PASSED ==========\n");

    return 0;
}
