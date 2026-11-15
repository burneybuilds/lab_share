
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "client.h"
#include "transfer.h"

void test_host_tests(void);

static int create_loopback_connection(SOCKET *server, SOCKET *client);
static void test_transfer_helpers(void);

// Test IPv4 validation.
static void test_validate_ip_address(void)
{
    printf("\n[TEST] IP Validation\n");

    assert(validate_ip_address("127.0.0.1") == 0);
    assert(validate_ip_address("192.168.1.10") == 0);
    assert(validate_ip_address("0.0.0.0") == 0);
    assert(validate_ip_address("255.255.255.255") == 0);

    assert(validate_ip_address("999.1.1.1") == 1);
    assert(validate_ip_address("abc.def.ghi.jkl") == 1);
    assert(validate_ip_address("192.168.1") == 1);
    assert(validate_ip_address("") == 1);
    assert(validate_ip_address(NULL) == 1);

    printf("[PASS] IP validation\n");
}

// Test Winsock initialization.
static void test_initialize_winsock(void)
{
    printf("\n[TEST] Winsock Initialization\n");

    int result = initialize_winsock_client();

    assert(result == 0);

    printf("[PASS] Winsock initialization\n");
}

// Test client socket creation.
static void test_client_session(void)
{
    printf("\n[TEST] Client Socket Creation\n");

    SOCKET client = client_session();

    assert(client != INVALID_SOCKET);

    closesocket(client);

    printf("[PASS] Client socket creation\n");
}

// Create a connected TCP pair on the local machine for transfer tests.
static int create_loopback_connection(SOCKET *server, SOCKET *client)
{
    struct sockaddr_in address = {0};
    int address_length = sizeof(address);

    *server = INVALID_SOCKET;
    *client = INVALID_SOCKET;

    *server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (*server == INVALID_SOCKET)
        return 1;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    address.sin_port = htons(0);

    if (bind(*server, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR)
        goto cleanup;

    if (listen(*server, 1) == SOCKET_ERROR)
        goto cleanup;

    if (getsockname(*server, (struct sockaddr *)&address, &address_length) == SOCKET_ERROR)
        goto cleanup;

    *client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (*client == INVALID_SOCKET)
        goto cleanup;

    if (connect(*client, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR)
        goto cleanup;

    SOCKET accepted = accept(*server, NULL, NULL);
    if (accepted == INVALID_SOCKET)
        goto cleanup;

    closesocket(*server);
    *server = accepted;
    return 0;

cleanup:
    if (*client != INVALID_SOCKET)
        closesocket(*client);
    if (*server != INVALID_SOCKET)
        closesocket(*server);
    *client = INVALID_SOCKET;
    *server = INVALID_SOCKET;
    return 1;
}

// Test newline-delimited message send/receive and error paths.
static void test_transfer_helpers(void)
{
    SOCKET server = INVALID_SOCKET;
    SOCKET client = INVALID_SOCKET;
    char buffer[32];

    printf("\n[TEST] Transfer Helpers\n");

    assert(receive_message(INVALID_SOCKET, NULL, 0) == 1);
    assert(send_message(INVALID_SOCKET, "not sent") == 1);

    assert(create_loopback_connection(&server, &client) == 0);
    assert(send_message(client, "hello") == 0);
    assert(receive_message(server, buffer, sizeof(buffer)) == 0);
    assert(strcmp(buffer, "hello") == 0);

    closesocket(client);
    client = INVALID_SOCKET;
    assert(receive_message(server, buffer, sizeof(buffer)) == 2);

    closesocket(server);
    server = INVALID_SOCKET;

    assert(create_loopback_connection(&server, &client) == 0);
    assert(send_message(client, "hello") == 0);
    assert(receive_message(server, buffer, 4) == 3);

    closesocket(client);
    closesocket(server);

    printf("[PASS] Transfer helpers\n");
}

int main(void)
{
    printf("========== LabShare Tests ==========\n");

    test_host_tests();
    test_validate_ip_address();

    test_initialize_winsock();

    test_client_session();
    test_transfer_helpers();

    WSACleanup();

    printf("\n========== Tests Completed ==========\n");

    return 0;
}
