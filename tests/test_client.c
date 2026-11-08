
#include <stdio.h>
#include <assert.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "client.h"

void test_host_tests(void);

// Test 1: IP validation
static void test_validate_ip_address(void)
{
    printf("\n[TEST] IP Validation\n");

    assert(validate_ip_address("127.0.0.1") == 0);
    assert(validate_ip_address("192.168.1.10") == 0);

    assert(validate_ip_address("999.1.1.1") == 1);
    assert(validate_ip_address("abc.def.ghi.jkl") == 1);
    assert(validate_ip_address("192.168.1") == 1);

    printf("[PASS] IP validation\n");
}

// Test 2: Winsock initialization
static void test_initialize_winsock(void)
{
    printf("\n[TEST] Winsock Initialization\n");

    int result = initialize_winsock_client();

    assert(result == 0);

    printf("[PASS] Winsock initialization\n");
}

// Test 3: Socket creation
static void test_client_session(void)
{
    printf("\n[TEST] Client Socket Creation\n");

    SOCKET client = client_session();

    assert(client != INVALID_SOCKET);

    closesocket(client);

    printf("[PASS] Client socket creation\n");
}

int main(void)
{
    printf("========== LabShare Tests ==========\n");

    test_host_tests();
    test_validate_ip_address();

    test_initialize_winsock();

    test_client_session();

    WSACleanup();

    printf("\n========== Tests Completed ==========\n");

    return 0;
}
