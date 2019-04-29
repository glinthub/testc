#ifndef RPC_CTX_H
#define RPC_CTX_H

#include <stdint.h>

#include "rpc_msg.h"

typedef struct _rpc_context {
    struct _rpc_context *nextFreeCtx;
    Rpc_Message_t       *rpcMsgReq;
    Rpc_Message_t       *rpcMsgResp;
    uint16_t            ctxID;
    uint16_t            destProvider;
} Rpc_Context_t;

typedef enum {
    Rpc_Ctx_Scale_Little,
    Rpc_Ctx_Scale_Normal,
    Rpc_Ctx_Scale_Large
} Rpc_Context_Scale_t;

Rpc_Context_t *rpc_ctx_alloc(Rpc_Context_Scale_t scale);
void rpc_ctx_free(Rpc_Context_t *ctx);
int rpc_ctx_req(Rpc_Context_t *c);
int rpc_ctx_wait_resp(Rpc_Context_t *c);
int rpc_ctx_handle(Rpc_Context_t *c);

#endif  //RPC_CTX_H
