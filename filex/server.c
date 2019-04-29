#include "filex.h"

typedef enum {
    x_idle,
    x_start,
    x_x,
    x_end
} x_state;

typedef struct {
    int sock;
    x_state state;
    uint8_t *rxBuf;
    uint8_t *txBuf;

    uint8_t     filename[128];
    uint64_t    filelen;
    FILE*       fp;
    uint16_t    blocklen;
} handler;

handler *handler_alloc()
{
    handler * h = (handler *)malloc(sizeof(handler));
    if (!h)
        return NULL;
    h->rxBuf = malloc(BLOCK_SIZE+4);
    h->txBuf = malloc(BLOCK_SIZE+4);
    if (!h->rxBuf || !h->txBuf)
        return NULL;
    h->state = x_idle;
    return h;
}

int server_filex_start(handler *h)
{
    EXIT_ON_ERROR(h->state == x_idle);
    h->state = x_x;

    packet_start *p = (packet_start *)h->rxBuf;
    char *filename = p->filename;
    filename[p->filenameLen] = '\0';
    if (strrchr(filename, '/'))
        filename = strrchr(filename, '/') + 1;
    h->fp = fopen(filename, "w+");
    EXIT_ON_ERROR(h->fp);

    h->blocklen = ntohs(p->blockLen);
    h->txBuf[0] = 0;
    send(h->sock, h->txBuf, 1, 0);

    return 0;
}

int server_filex_xfer(handler *h)
{
    EXIT_ON_ERROR(h->state == x_x);

    packet_xfer *p = (packet_xfer *)h->rxBuf;
    printf("Received block %d\n", ntohs(p->blockSeq));
    int len = ntohs(p->gen.len) - sizeof(packet_xfer);
    fwrite(p->block, len, 1, h->fp);

    h->txBuf[0] = 0;
    send(h->sock, h->txBuf, 1, 0);

    return 0;
}

int server_filex_stop(handler *h)
{
    EXIT_ON_ERROR(h->state == x_x);

    fclose(h->fp);
    h->state = x_idle;

    h->txBuf[0] = 0;
    send(h->sock, h->txBuf, 1, 0);

    return 0;
}

void server_receive_thread(void *arg)
{
    handler * h = (handler *)arg;
    size_t len;
    ssize_t sz;
    while (1)
    {
        len = sizeof(packet_gen);
        sz = recv(h->sock, h->rxBuf, len, MSG_WAITALL);
        EXIT_ON_ERROR(sz == len);
        printf("Generic header received\n");
        len = ntohs(((packet_gen *)(h->rxBuf))->len) - sizeof(packet_gen);
        if (len)
        {
            sz = recv(h->sock, h->rxBuf + sizeof(packet_gen), len, MSG_WAITALL);
            EXIT_ON_ERROR(sz == len);
            printf("data received\n");
        }

        packet_gen *pktGen = (packet_gen *)h->rxBuf;
        if (pktGen->op == OP_START)
            server_filex_start(h);
        else if (pktGen->op == OP_XFER)
            server_filex_xfer(h);
        else if (pktGen->op == OP_STOP)
            server_filex_stop(h);
    }
}

int main (int argc, char **argv){
	int sock;
	int done = 0;	// Server exit.
	socklen_t addr_len;
	pthread_t t_id;
	struct sockaddr_in server;
	unsigned short port = SERVER_PORT;

    if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
		printf("Server socket could not be created.\n");
		return 0;
	}

    int val = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);

	if (bind(sock, (struct sockaddr *) &server, sizeof(server)) < 0){
		printf("Server bind failed. Server already running? Proper permissions?\n");
		return 2;
	}
	
	printf("Server started at 0.0.0.0:%d.\n", port);
	
    if (listen(sock, 2) != 0)
    {
        printf("Server listen failed. \n");
        return 2;
    }

    struct sockaddr client;
    socklen_t addrlen = sizeof(client);
    while (1)
    {
        int sock_cli = accept(sock, &client, &addrlen);
        if (sock_cli > 0)
        {
            pthread_t tid;
            handler *h = handler_alloc();
            if (!h)
            {
                return -1;
            }
            h->sock = sock_cli;
            pthread_create(&tid, NULL, (void *)server_receive_thread, (void*)h);
        }
    }
    return 0;
}
