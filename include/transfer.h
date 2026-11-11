#ifndef TRANSFER_H
#define TRANSFER_H

int receive_message(SOCKET connection_session, char *buffer, int buffer_size);
int send_message(SOCKET connection_session, const char *input_buffer);

#endif