#include <stdlib.h>

#include "rpc_msg.h"
#include "rpc_types.h"

#define RPC_BUF_LEVEL 4
static uint16_t rpcmsgsizeList[RPC_BUF_LEVEL] = {256, 1024, 4096, 16384};
static uint16_t rpcMsgCntList[RPC_BUF_LEVEL] = {16, 8, 4, 2};

static struct rpc_msg_pool {
    Rpc_Message_t *msgList[RPC_BUF_LEVEL];
    Rpc_Message_t *freeMsgList[RPC_BUF_LEVEL];
} rpc_msg_pools;

int _rpc_get_msg_scale(uint32_t sz)
{
    int i;
    for (i=0; i<RPC_BUF_LEVEL; i++)
    {
        if (sz < rpcmsgsizeList[i])
            return i;
    }

    return -1;
}

int rpc_msg_init()
{
    static boolean rpc_msg_initiated = false;
    if (rpc_msg_initiated == true)
        return RPC_E_REPEATED;
    int i;
    for (i=0; i<RPC_BUF_LEVEL; i++)
    {
        Rpc_Pdu_t *pdus = (Rpc_Pdu_t *)malloc((sizeof(Rpc_Pdu_t) + rpcmsgsizeList[i]) * (rpcMsgCntList[i]));
        Rpc_Message_t  *msgs = (Rpc_Message_t *)malloc((sizeof(Rpc_Message_t)) * rpcMsgCntList[i]);
        int j;
        for (j=0; j<rpcMsgCntList[i]; j++)
        {
            if (j<rpcMsgCntList[i]-1)
                msgs[j].nextMsg = &msgs[j+1];
            else
                msgs[j].nextMsg = NULL;
            msgs[j].nextFreeMsg = msgs[j].nextMsg;
            msgs[j].pdu = &pdus[j];
            msgs[j].codeOffset = 0;
            msgs[j].length = rpcmsgsizeList[i];
        }
        rpc_msg_pools.msgList[i] = msgs;
        rpc_msg_pools.freeMsgList[i] = msgs;
    }
    rpc_msg_initiated = true;
    return 0;
}

Rpc_Message_t *rpc_msg_alloc(uint16_t sz)
{
    int level = _rpc_get_msg_scale(sz);
    for (; level<RPC_BUF_LEVEL; level++)
    {
        Rpc_Message_t *rpcMsg = rpc_msg_pools.freeMsgList[level];
        if (rpcMsg != NULL)
        {
            rpc_msg_pools.freeMsgList[level] =
                    rpc_msg_pools.freeMsgList[level]->nextFreeMsg;
            rpcMsg->nextFreeMsg = NULL;
            return rpcMsg;
        }
    }
    return NULL;
}

void rpc_msg_free(Rpc_Message_t *rpcMsg)
{
    int level = _rpc_get_msg_scale(rpcMsg->length);
    rpcMsg->nextFreeMsg = rpc_msg_pools.freeMsgList[level];
    rpcMsg->codeOffset = 0;
    rpc_msg_pools.freeMsgList[level] = rpcMsg;
}
