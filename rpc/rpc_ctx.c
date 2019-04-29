#include <arpa/inet.h>

#include "rpc_types.h"
#include "rpc_ctx.h"
#include "rpc_com.h"
#include "rpc_agent.h"

#define RPC_CTX_MAX_NUM 8
static Rpc_Context_t rpc_ctx_pools[RPC_CTX_MAX_NUM];
static Rpc_Context_t *rpcFreeCtxList = rpc_ctx_pools;

int rpc_ctx_init()
{
    static boolean rpc_ctx_initiated = false;
    if (rpc_ctx_initiated == true)
        return RPC_E_REPEATED;

    int i;
    for (i=0; i<RPC_CTX_MAX_NUM; i++)
    {
        Rpc_Context_t *ctx = &rpc_ctx_pools[i];
        ctx->ctxID = (uint16_t)i;
        ctx->rpcMsgReq = NULL;
        ctx->rpcMsgResp = NULL;
        ctx->nextFreeCtx = &rpc_ctx_pools[i+1];
        if (i == RPC_CTX_MAX_NUM-1)
            ctx->nextFreeCtx = NULL;
    }
    rpc_ctx_initiated = true;
    return RPC_E_OK;
}

Rpc_Context_t *rpc_ctx_alloc(Rpc_Context_Scale_t scale)
{
    Rpc_Context_t *ctx = rpcFreeCtxList;
    int bufSz = 256;
    if (scale == Rpc_Ctx_Scale_Large)
        bufSz = 16384;
    if (ctx != NULL)
    {
        rpcFreeCtxList = ctx->nextFreeCtx;
        ctx->nextFreeCtx = NULL;
        ctx->rpcMsgReq = rpc_msg_alloc(bufSz);
        ctx->rpcMsgReq = rpc_msg_alloc(bufSz);
        return ctx;
    }
    return NULL;
}

void rpc_ctx_free(Rpc_Context_t *ctx)
{
    ctx->nextFreeCtx = rpcFreeCtxList;
    rpc_msg_free(ctx->rpcMsgReq);
    ctx->rpcMsgReq = NULL;
    rpc_msg_free(ctx->rpcMsgResp);
    ctx->rpcMsgResp = NULL;
    rpcFreeCtxList = ctx;
}

int rpc_ctx_req(Rpc_Context_t *c)
{
    rpc_com_send(c->destProvider, c->rpcMsgReq->pdu);
    return 0;
}

int rpc_ctx_wait_resp(Rpc_Context_t *c)
{
    rpc_com_send(c->destProvider, c->rpcMsgReq->pdu);
    //TODO: wait response
    return 0;
}

int _rpc_ctx_resp(Rpc_Context_t *c)
{
    rpc_com_send(c->destProvider, c->rpcMsgResp->pdu);
    return 0;
}

int rpc_ctx_handle(Rpc_Context_t *c)
{
    uint16_t sid = ntohs(c->rpcMsgReq->pdu->sid);
    RpcHandler_t handler = rpc_handlers[sid];
    handler(c);
    _rpc_ctx_resp(c);
    return 0;
}
