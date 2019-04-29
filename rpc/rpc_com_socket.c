#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#include "rpc_types.h"
#include "rpc_msg.h"
#include "rpc_ctx.h"
#include "rpc_com_socket.h"

int _rpc_com_socket_connect(Rpc_Com_Node_t *dest)
{
    if (dest->sock.sockFd != 0)
        return 0;
    dest->sock.sockFd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(dest->sock.ipAddr);
    sin.sin_port = htons(dest->sock.port);

    unsigned long ul=1;
    int ret = ioctl(dest->sock.sockFd, FIONBIO, (unsigned long*)&ul);
    RPC_EXPECT(0==ret, RPC_E_NOT_OK);

    fd_set fs;
    FD_ZERO(&fs);
    FD_SET(dest->sock.sockFd, &fs);

    struct timeval tv;
    tv.tv_sec = 8;
    tv.tv_usec = 0;

    ret = select(dest->sock.sockFd+1, 0, &fs, 0, &tv);
    RPC_EXPECT(1==ret, RPC_E_NOT_OK);

    int er;
    socklen_t len = sizeof(er);
    ret = getsockopt(dest->sock.sockFd, SOL_SOCKET, SO_ERROR, (char *)&er, &len);
    RPC_EXPECT(0==ret, RPC_E_NOT_OK);
    RPC_EXPECT(0==er, RPC_E_NOT_OK);

    int nodelay = 1;
    ret = setsockopt(dest->sock.sockFd, SOL_SOCKET, TCP_NODELAY, &nodelay, sizeof(nodelay));
    RPC_EXPECT(0==ret, RPC_E_NOT_OK);

    return 0;
}

int rpc_com_socket_send(Rpc_Com_Node_t *dest, Rpc_Pdu_t *pdu)
{
    _rpc_com_socket_connect(dest);
    send(dest->sock.sockFd, pdu, ntohs(pdu->len), 0);
    return 0;
}

static uint8_t recvbuf[20000];
void rpc_com_socket_server_thread(void *arg)
{
    Rpc_Com_Node_t *self = (Rpc_Com_Node_t *)arg;

    static pthread_t tid_recv;
    if (self->sock.sockFd == 0)
        return;

    socklen_t socklen = sizeof(struct sockaddr_in);
    self->sock.sockFd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(self->sock.ipAddr);
    sin.sin_port = htons(self->sock.port);

    int ret = bind(self->sock.sockFd, (struct sockaddr *)&sin, socklen);
    RPC_EXPECT_VOID(0==ret);

    ret = listen(self->sock.sockFd, 1);
    RPC_EXPECT_VOID(0==ret);

    while (1)
    {
        struct sockaddr_in sinc;

        int sock = accept(self->sock.sockFd, (struct sockaddr *)&sinc, &socklen);
        RPC_EXPECT_VOID(sock > 0);

        ssize_t recvSz;
        while(1)
        {
            uint16_t headSz = sizeof(Rpc_Pdu_t);
            recvSz = recv(sock, recvbuf, headSz, MSG_WAITALL);
            RPC_EXPECT_VOID(recvSz==headSz);

            Rpc_Pdu_t *pdu = (Rpc_Pdu_t *)recvbuf;
            recvSz = recv(sock, recvbuf + headSz, ntohs(pdu->len) - headSz, MSG_WAITALL);
            RPC_EXPECT_VOID(recvSz==ntohs(pdu->len) - headSz);
            RPC_EXPECT_VOID(pdu->type==Rpc_Pdu_Request);

            Rpc_Context_Scale_t s = Rpc_Ctx_Scale_Normal;
            Rpc_Context_t *ctx = rpc_ctx_alloc(s);
            memcpy(ctx->rpcMsgReq->pdu, recvbuf, ntohs(pdu->len));
            rpc_ctx_handle(ctx);
        }
    }
}

int rpc_com_socket_init(Rpc_Com_Node_t *self)
{
    if (self->sock.sockFd == 0)
        return 0;
    pthread_t tid;
    pthread_create(&tid, NULL, (void *)rpc_com_socket_server_thread, self);
}

