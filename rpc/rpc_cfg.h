#ifndef RPC_CFG_H
#define RPC_CFG_H

#include "rpc_com.h"

typedef struct {
    uint32_t    ipAddr;
    uint16_t    port;
    uint16_t    proto;
    int         sockFd;
} Rpc_Cfg_Node_Socket_t;

typedef struct {
    Rpc_Com_Type_t type;
    union {
        Rpc_Cfg_Node_Socket_t sock;
    };
} Rpc_Cfg_Node_t;

extern Rpc_Cfg_Node_t rpc_cfg_nodes[];
Rpc_Com_Node_t *rpc_cfg_get_com(int comId);

#endif // RPC_CFG_H
