#ifndef RPC_AGENT_H
#define RPC_AGENT_H

#include "rpc_ctx.h"

typedef Rpc_ReturnType (*RpcHandler_t)(Rpc_Context_t *ctx);
extern RpcHandler_t rpc_handlers[];

#endif // RPC_AGENT_H
