#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <commdlg.h>

// Prototypes.
char *file_selecter(int user_input);


/**
 * @brief Opens the Windows file dialog to select a file.
 * @param user_input Opens the dialog when the value is 1.
 * @return Dynamically allocated file path on success, otherwise NULL.
 * @note The caller must free the returned pointer.
 */
char *file_selecter(int user_input)
{
    // File pointers point toward files, unlike your crush. :')
    char filePath[MAX_PATH] = {0};

    OPENFILENAMEA ofn = {0};

    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = "All Files (*.*)\0*.*\0";
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

    if (user_input == 1)
    {
        if (GetOpenFileNameA(&ofn))
        {
            char *selectedPath = malloc(strlen(filePath) + 1);

            if (selectedPath == NULL)
                return NULL;

            strcpy(selectedPath, filePath);

            // printf("Selected file: %s\n", selectedPath);
            return selectedPath;
        }
        else
        {
            printf("No file selected.\n");
            return NULL;
        }
    }

    return NULL;
}
// void send_file(){

// }

// void receive_file(){

// }