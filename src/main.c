#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "host.h"
#include "client.h"
#include "utils.h"
#include "validate.h"

// Prototypes.
int cli_interface_handler(void);
void client();
void host();
void settings();

int cli_interface_handler(void)
{
    int user_input = 0;

    system("cls"); // IDK why I love clearing the screen so much. OMG...

    main_menu(); // This just prints the main menu, you can find it in the untils file.

    printf("<Input> ");
    scanf("%d", &user_input);

    // Check the input stream for any errors.
    int error_flag = input_validater(user_input);
    if (error_flag == 1){
        error_message();
        getchar();
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
    fp = fopen("../config/settings.txt", "r");

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
    
    printf("[+] Press Enter to close the session.\n");
    getchar();
    getchar();
    
    fclose(fp);
}


void client(){
    client_side_menu();
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
                break;
        }
    }

    return 0;
}