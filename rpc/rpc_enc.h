#ifndef RPC_ENC_H
#define RPC_ENC_H
#endif  //RPC_ENC_H

#include "rpc_msg.h"

typedef enum {
    Rpc_Encoding_Type_Integer,
    Rpc_Encoding_Type_Buffer
} Rpc_Encoding_Type_t;

int rpc_encode(Rpc_Message_t *rpcMsg, uint16_t type, int len, void *data);
int rpc_decode(Rpc_Message_t *rpcMsg, void *data);

