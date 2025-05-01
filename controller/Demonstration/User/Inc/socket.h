#ifndef SOCKET_H
#define SOCKET_H

#include "webserver_main.h"

WebServer_StatusTypeDef socket_server_start(void);

void handle_client(int32_t conn);
WebServer_StatusTypeDef treat_command(uint8_t *recv_buffer);
WebServer_StatusTypeDef socket_send(int32_t socket, uint8_t* buffer, uint32_t frame_size);

#endif
