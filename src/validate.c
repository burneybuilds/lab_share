#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

int input_validater(int user_input);
int validate_ip_address(const char *ip);

int input_validater(int user_input)
{
    // Check the input stream for any errors.
    if (user_input == 0)
    {
        // Clear invalid input from the input buffer.
        int ch;

        // getchar() reads a character from the input stream and returns its integer value (ASCII value on ASCII-compatible systems).
        // This loop runs until a newline or EOF is encountered, clearing the remaining input.
        while ((ch = getchar()) != '\n' && ch != EOF);

        return 1;
    }
    return 0;
}


/// @brief Checks whether the given IP address is a valid IPv4 address.
/// @param ip The IPv4 address string to validate.
/// @return 0 if the IP address is valid, 1 if it is invalid.
int validate_ip_address(const char *ip)
{
    struct in_addr address;

    // Convert the IP address from string format to binary format.
    int flag = inet_pton(AF_INET, ip, &address);

    // Check if the conversion failed or the IP address is invalid.
    if (flag != 1) {
        return 1;
    }

    // IP address is valid.
    return 0;
}