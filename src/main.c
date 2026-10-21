#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "host.h"
#include "client.h"



int cli_interface_handler(void)
{
    int user_input = 0;

    // IDK why I love clearing the screen so much. OMG...
    system("cls");

    printf("===================Lab Share====================\n");
    printf("1. Host a Session\n");
    printf("2. Connect to a Session\n");
    printf("3. Settings.\n");
    printf("4. Exit.\n");
    printf("=================================================\n");

    printf("<Input> ");
    // scanf("%d", &user_input);

    // Check the input stream for any errors.
    if (scanf("%d", &user_input) != 1)
    {
        printf("===================Error=========================\n");
        printf("Invalid input! Please enter a number.\n");
        printf("=================================================\n");

        // Clear invalid input from the input buffer.
        int ch;

        // getchar() reads a character from the input stream and returns its integer value (ASCII value on ASCII-compatible systems).
        // This loop runs until a newline or EOF is encountered, clearing the remaining input.
        while ((ch = getchar()) != '\n' && ch != EOF)
        {
        }

        return 0;
    }

    // If everything goes right (which might not always be the case), return the user's input.
    return user_input;
}


// This fucntion is where file handling of setting is done. 
void settings(){
    FILE *fp;
    char user_name[50];
    int port;
    int connection_timeout;
    int max_connections;

    // Opens the file in "w" == write Mode.
    fp = fopen("settings.text", "r");

    // checks if the file is not there and return 1 exit code == Error.
    if (fp == NULL) {
        printf("Error opening file!");
        // return 1;
    }   
    
    fscanf(fp, "username=%49[^\n]\n", user_name);
    fscanf(fp, "Port=%d\n", &port);
    fscanf(fp, "connection_timeout=%d", &connection_timeout);
    fscanf(fp, "max_connections=%d", &max_connections);


    // Use the loaded settings
    printf("===================Settings======================\n");
    printf("Username: %s\n", user_name);
    printf("Port No: %d\n", port);
    printf("Connection Timeout: %d\n", connection_timeout);
    printf("Connection Timeout: %d\n", max_connections );
    printf("=================================================\n");
    
    fclose(fp);
    // return 0;
}

void client(){
    join_connection();
}

void host(){
    host_menu_handler();
}

int main(void){

    int user_input = 0 ;
    
    while (user_input  != 4){
        user_input = cli_interface_handler();

        switch(user_input){
            case 1:
                system("cls"); 
                host();
                break;
            case 2:
                system("cls"); 
                client();
                break;
            case 3:
                system("cls");
                settings();
                break;
            case 4:
                printf("Have a great day.");
                return 0;
            default:
                // printf("Invalid Input. ");
                // system("cls");    
                break;
        }
    }

    return 0;
}