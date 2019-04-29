#ifndef RPC_ADDRESSING_H
#define RPC_ADDRESSING_H

typedef enum {
    Rpc_Provider_Type_Socket,
    Rpc_Provider_Type_SocketUnix,
    Rpc_Provider_Type_Pipe,
    Rpc_Provider_Type_Shm,
    Rpc_Provider_Type_MsgQ
} Rpc_Provider_Type_t;

int rpc_provider_get(int providerId);
extern int myProviderId;

#endif // RPC_ADDRESSING_H
