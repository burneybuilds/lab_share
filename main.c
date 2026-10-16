#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>


int cli_interface(){

    int user_input = 0;
    
    printf("===================Lab Share====================\n");
    printf("1. Connect Code.\n");
    printf("2. Generate Code.\n");
    printf("3. Settings.\n");
    printf("4. Exit.\n");
    printf("=================================================\n");
    
    printf("<Input>");
    scanf("%d", &user_input);

    
    return user_input;
}

// This fucntion is where file handling of setting is done. 
void settings(){
    FILE *fp;
    char username[50];
    int volume;
    int dark_mode;

    // Opens the file in "w" == write Mode.
    fp = fopen("settings.text", "r");

    // checks if the file is not there and return 1 exit code == Error.
    if (fp == NULL) {
        // printf("Error opening file!");
        
    }
    
    if (fscanf(fp, "username=%49[^\n]\n", username) != 1 ||
        fscanf(fp, "volume=%d\n", &volume) != 1 ||
        fscanf(fp, "dark_mode=%d", &dark_mode) != 1) {
        //printf("Invalid settings file.\n");
        fclose(fp);
    }

    // Use the loaded settings
    printf("===================Settings======================\n");
    printf("Username: %s\n", username);
    printf("Volume: %d\n", volume);
    printf("Dark mode: %s\n", dark_mode ? "ON" : "OFF");
    printf("=================================================\n");
    
    fclose(fp);
}

int join_room(){
    
}

int create_room(){
    return 0;
}

int create_connect(){
    return 0;
}
int main(void){

    int user_input = cli_interface();
    
    while (user_input  != 4){
        switch(user_input){
            case 1:

            case 2:
                
            case 3:
                system("clear");
                settings();
                break;
            case 4:
                printf("Have a great day.");
                return 0;
            // default:
            //     printf("Invalid Input. ");
            //     system("cls");    
            //     break;
        }
    }

    return 0;
}