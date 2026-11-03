#ifndef TRANSFER_H
#define TRANSFER_H

char *file_selecter(int user_input);
int send_file(SOCKET connection_session, const char *input_buffer);
int receive_message(SOCKET connection_session, char *buffer, int buffer_size);

#endif