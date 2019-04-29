#ifndef RPC_COM_SOCKET_H
#define RPC_COM_SOCKET_H

#include "rpc_com.h"
#include "rpc_msg.h"

int rpc_com_socket_send(Rpc_Com_Node_t *dest, Rpc_Pdu_t *pdu);
int rpc_com_socket_init(Rpc_Com_Node_t *self);

#endif // RPC_COM_SOCKET_H
