#include <stdio.h>

#include "rpc_com.h"
#include "rpc_com_socket.h"
#include "rpc_msg.h"
#include "rpc_cfg.h"
#include "rpc_provider.h"

int rpc_com_send(int comId, Rpc_Pdu_t *pdu)
{
    Rpc_Com_Node_t *dest = rpc_cfg_get_com(comId);
    if (dest->type == Rpc_Com_Type_Socket)
    {
        return rpc_com_socket_send(dest, pdu);
    }
    else
    {
        printf("unimplemented com type (%d)\n", dest->type);
        return -1;
    }
}

int rpc_com_init()
{
    Rpc_Com_Node_t *self = rpc_cfg_get_com(myProviderId);
    if (self == NULL)
    {
        printf("no RPC server needed for current process\n");
        return 0;
    }

    if (self->type == Rpc_Com_Type_Socket)
    {
        return rpc_com_socket_init(self);
    }
    else
    {
        printf("unimplemented com type (%d)\n", self->type);
        return -1;
    }

    return 0;
}
