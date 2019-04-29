#ifndef RPC_COM_H
#define RPC_COM_H

#include "rpc_types.h"
#include "rpc_msg.h"

typedef enum {
    Rpc_Com_Type_Socket,
    Rpc_Com_Type_SocketUnix,
    Rpc_Com_Type_Pipe,
    Rpc_Com_Type_Shm,
    Rpc_Com_Type_MsgQ
} Rpc_Com_Type_t;

typedef struct {
    uint32_t    ipAddr;
    uint16_t    port;
    uint16_t    proto;
    int         sockFd;
} Rpc_Com_Socket_t;

typedef struct {
    Rpc_Com_Type_t type;
    union {
        Rpc_Com_Socket_t sock;
    };
} Rpc_Com_Node_t;

int rpc_com_send(int comId, Rpc_Pdu_t *pdu);

#endif  //RPC_COM_H
