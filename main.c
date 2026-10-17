#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "host.h"
#include "client.h"

int cli_interface(){

    int user_input = 0;
    
    printf("===================Lab Share====================\n");
    printf("1. Generate Code.\n");
    printf("2. Connect Code.\n");
    printf("3. Settings.\n");
    printf("4. Exit.\n");
    printf("=================================================\n");
    
    printf("<Input>");
    scanf("%d", &user_input);
    
    return user_input;
}

// This fucntion is where file handling of setting is done. 
int settings(){
    FILE *fp;
    char user_name[50];
    int port;
    float connection_timeout;
    int max_connections;

    // Opens the file in "w" == write Mode.
    fp = fopen("settings.text", "r");

    // checks if the file is not there and return 1 exit code == Error.
    if (fp == NULL) {
        printf("Error opening file!");
        return 1;
    }   
    
    // if (fscanf(fp, "username=%49[^\n]\n", user_name) != 1 ||
    //     fscanf(fp, "volume=%d\n", &port) != 1 ||
    //     fscanf(fp, "dark_mode=%d", &connection_timeout) != 1) {
    //     printf("Invalid settings file.\n");
    //     fclose(fp);
    // }

    // Use the loaded settings
    printf("===================Settings======================\n");
    printf("Username: %s\n", user_name);
    printf("Port No: %d\n", port);
    printf("Connection Timeout: %s\n", connection_timeout ? "ON" : "OFF");
    printf("Connection Timeout: %s\n", max_connections );
    printf("=================================================\n");
    
    fclose(fp);
    return 0;
}

int client(){
    return 0;
}

int host(){
    main_controller();
}

int main(void){

    int user_input = 0 ;
    
    while (user_input  != 4){
        user_input = cli_interface();
        
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
                system("clear");
                int flag = settings();
                break;
            case 4:
                printf("Have a great day.");
                return 0;
            default:
                printf("Invalid Input. ");
                system("cls");    
                break;
        }
    }

    return 0;
}