
#include "filex.h"


int client_get_filesize(char *filename, uint64_t *size)
{
    struct stat statbuf;
    int ret = stat(filename, &statbuf);
    if (ret != 0)
    {
        printf("Failed to get file length (%s)\n", strerror(errno));
        return ret;
    }
    *size = statbuf.st_size;

    return ret;
}

int main(int argc, char **argv){
	char *local_file;
	char *server_ip;
	
	if(argc < 2){
		printf("Usage: %s server_ip filename\n", argv[0]);
		return 0;
	}
	
	server_ip = argv[1];
    local_file = argv[2];
    printf("Connect to server at %s:%d", server_ip, local_file);
	
    FILE* fp = fopen(local_file, "r");
    EXIT_ON_ERROR(fp);

    // Socket fd this client use.
    int sock;
    // Server address.
    if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
		printf("Server socket could not be created.\n");
		return 0;
	}
	// Initialize server address
    struct sockaddr_in server;
	server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, server_ip, &(server.sin_addr.s_addr));

    if (connect(sock, (struct sockaddr *)&server, sizeof(struct sockaddr)) != 0)
    {
        EXIT_ON_ERROR(0);
    }

    uint8_t *rxBuf = malloc(sizeof(packet_xfer)+BLOCK_SIZE);
    uint8_t *txBuf = malloc(sizeof(packet_xfer)+BLOCK_SIZE);
    if (!rxBuf || !txBuf)
        EXIT_ON_ERROR(0);

    uint16_t txLen;

    packet_start *pktStart = (packet_start *)txBuf;
    pktStart->gen.op = OP_START;
    pktStart->blockLen = htons(BLOCK_SIZE);
    pktStart->filenameLen = htons(strlen(local_file));
    strcpy(pktStart->filename, local_file);
    txLen = sizeof(packet_start) + strlen(local_file);
    pktStart->gen.len = htons(txLen);
    send(sock, txBuf, txLen, 0);
    recv(sock, rxBuf, 1, MSG_WAITALL);

    uint16_t blockseq = 0;
	while(1){
        packet_xfer *pktXfer = (packet_xfer *)txBuf;
        int len = fread(pktXfer->block, 1, BLOCK_SIZE, fp);
        if (len == 0)
            break;
        txLen = sizeof(packet_xfer) + len;
        pktXfer->blockSeq = blockseq++;
        pktXfer->gen.op = OP_XFER;
        pktXfer->gen.len = htons(txLen);

        printf("Sending block %d\n", blockseq);
        send(sock, txBuf, txLen, 0);
        recv(sock, rxBuf, 1, MSG_WAITALL);
    }

    txLen = sizeof(packet_stop);
    packet_stop *pktStop = (packet_stop *)txBuf;
    pktStop->gen.op = OP_STOP;
    pktStop->gen.len = htons(txLen);
    send(sock, txBuf, txLen, 0);
    recv(sock, rxBuf, 1, MSG_WAITALL);

	return 0;
}
