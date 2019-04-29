#ifndef UTILS_MGNT_H
#define UTILS_MGNT_H

#include <stdint.h>

void mgnt_server_receiver_register(void (*recv)(int com_fd, uint8_t *buf, uint8_t len));
void mgnt_server_send(int com_fd, uint8_t *buf, uint8_t len);
int mgnt_server(uint8_t *filename);
int mgnt_client(uint8_t *filename, uint8_t *cmd);

#endif  //UTILS_MGNT_H
