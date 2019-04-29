#include <arpa/inet.h>

#include "rpc_types.h"
#include "rpc_msg.h"
#include "rpc_enc.h"

boolean _rpc_endian_toggle()
{
    return (0x1234 != htons(0x1234));
}

int rpc_encode(Rpc_Message_t *rpcMsg, uint16_t type, int len, void *data)
{
    int i;
    if (type == Rpc_Encoding_Type_Integer)
    {
        for (i=0; i<len; i++)
        {
            uint8_t *bytes = (uint8_t *)data;
            if (_rpc_endian_toggle())
                rpcMsg->pdu->buf[rpcMsg->codeOffset++] = bytes[len-1-i];
            else
                rpcMsg->pdu->buf[rpcMsg->codeOffset++] = bytes[i];
        }
    }
    else if (type == Rpc_Encoding_Type_Buffer)
    {
        for (i=0; i<len; i++)
        {
            uint8_t *bytes = (uint8_t *)data;
            rpcMsg->pdu->buf[rpcMsg->codeOffset++] = bytes[i];
        }
    }
    return RPC_E_OK;
}

int rpc_decode(Rpc_Message_t *rpcMsg, void *data)
{
    int i;
    uint16_t type = *(uint16_t*)(&rpcMsg->pdu->buf[rpcMsg->codeOffset]);
    int len = *(uint16_t*)(&rpcMsg->pdu->buf[rpcMsg->codeOffset + 2]);
    rpcMsg->codeOffset += 4;
    if (type == Rpc_Encoding_Type_Integer)
    {
        for (i=0; i<len; i++)
        {
            uint8_t *bytes = (uint8_t *)data;
            if (_rpc_endian_toggle())
                bytes[len-1-i] = rpcMsg->pdu->buf[rpcMsg->codeOffset++];
            else
                bytes[i] = rpcMsg->pdu->buf[rpcMsg->codeOffset++];
        }
    }
    else if (type == Rpc_Encoding_Type_Buffer)
    {
        for (i=0; i<len; i++)
        {
            uint8_t *bytes = (uint8_t *)data;
            bytes[i] = rpcMsg->pdu->buf[rpcMsg->codeOffset++];
        }
    }
    return RPC_E_OK;
}
