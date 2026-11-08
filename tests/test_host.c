
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "host.h"

static void test_initialize_winsock(void);
static void test_create_server_socket(void);
static void test_bind_server_socket(void);
static void test_start_listening(void);
static void test_accept_client(void);
static void test_code_generator(void);
void test_host_tests(void);

// Test Winsock initialization.
static void test_initialize_winsock(void)
{
    int result = initialize_winsock();

    assert(result == 0);

    printf("[PASS] Winsock initialization\n");
}

// Test socket creation.
static void test_create_server_socket(void)
{
    SOCKET server_socket = create_server_socket();

    assert(server_socket != INVALID_SOCKET);

    closesocket(server_socket);

    printf("[PASS] Server socket creation\n");
}

// Test socket binding.
static void test_bind_server_socket(void)
{
    SOCKET server_socket = create_server_socket();

    assert(server_socket != INVALID_SOCKET);

    // Port 0 lets Windows select an available port.
    int result = bind_server_socket(server_socket, 0);

    assert(result == 0);

    // Binding the same socket again should fail.
    result = bind_server_socket(server_socket, 8080);

    assert(result != 0);

    closesocket(server_socket);

    printf("[PASS] Server socket binding\n");
}

// Test listening.
static void test_start_listening(void)
{
    SOCKET server_socket = create_server_socket();

    assert(server_socket != INVALID_SOCKET);

    int bind_result = bind_server_socket(server_socket, 0);
    assert(bind_result == 0);

    int result = start_listening(server_socket);

    assert(result == 0);

    closesocket(server_socket);

    // Listening on an invalid socket should fail.
    result = start_listening(INVALID_SOCKET);

    assert(result != 0);

    printf("[PASS] Server listening\n");
}

// Test accepting clients.
static void test_accept_client(void)
{
    SOCKET client_socket = accept_client(INVALID_SOCKET);

    assert(client_socket == INVALID_SOCKET);

    printf("[PASS] Client acceptance error handling\n");
}

// Test room code generation.
static void test_code_generator(void)
{
    for (int i = 0; i < 1000; i++)
    {
        int code = code_generator();

        assert(code >= 1000);
        assert(code <= 9999);
    }

    printf("[PASS] Room code generation\n");
}

void test_host_tests(void)
{
    printf("\n========== HOST TESTS ==========\n");

    test_initialize_winsock();
    test_create_server_socket();
    test_bind_server_socket();
    test_start_listening();
    test_accept_client();
    test_code_generator();
    // test_input_handler();

    WSACleanup();

    printf("\n[+] All host tests passed.\n");

    return;
}
