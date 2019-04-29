#ifndef RPC_MSG_H
#define RPC_MSG_H

#include "rpc_types.h"

typedef enum {
    Rpc_Pdu_Request,
    Rpc_Pdu_Response
} Rpc_Pdu_Type_t;

typedef struct _rpc_pdu {
    uint16_t    len;
    uint8_t     type;
    uint8_t     errCode;
    uint16_t    sid;
    uint16_t    seq;
    uint8_t     buf[0];
} Rpc_Pdu_t;

typedef struct _rpc_msg {
    struct _rpc_msg *nextFreeMsg;
    struct _rpc_msg *nextMsg;
    uint32_t        length;
    uint32_t        codeOffset;
    Rpc_Pdu_t   *pdu;
} Rpc_Message_t;

int             rpc_msg_init();
Rpc_Message_t   *rpc_msg_alloc(uint16_t sz);
void            rpc_msg_free(Rpc_Message_t *rpcMsg);

#endif  //RPC_MSG_H
