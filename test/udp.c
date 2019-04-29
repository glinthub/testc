#include <sys/errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/timeb.h>
#include <string.h>

uint16_t checksum(uint16_t *dp, uint16_t length)
{
    uint32_t sum = 0;
    uint16_t len = length >> 1;

    while(len-- > 0) {
		printf("while#%d,0x%08x + 0x%04x\n", len, sum, (*dp));
        sum += (*dp);
		dp++;
    }
    if(length & 1) {
        sum += (*dp & 0xFF00);
	    printf("if: 0x%x\n", sum);
    }

    sum = (sum >> 16) + (sum & 0xFFFF);
	printf("1: 0x%08x\n", sum);
    sum += (sum >> 16);
	printf("2: 0x%08x\n", sum);       
    len = sum & 0xFFFF;
	printf("result: 0x%04x->0x%04x\n", len, ~len);       
    return(~len);
}
int sockfd;

int txThread()
{
	char buf[66666];
    int len;
	//int sockfd;
    struct sockaddr_in addr;
    pthread_t tid = pthread_self();

    //sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd<0)
    {
        printf("Socket Error: %s\n", strerror(errno));
        exit(1);
    }
    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST | SO_REUSEADDR, &optval, sizeof(int));

    bzero(&addr, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
	inet_aton("255.255.255.255", &(addr.sin_addr));
    addr.sin_port = htons(8888);

    int seq = 0;
    while (1)
    {
        sprintf(buf, "hello, seq %08x", seq++);
        printf("sending %s\n", buf);
        len = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr*)&addr, sizeof(addr));
        if (len <= 0) {
            printf("send failed: %s\n", strerror(errno));
        }
        sleep(1);
    }

    close(sockfd);
}

int rxThread()
{
	char buf[66666];
    int len;
    int rc;
    struct sockaddr_in addr;
    struct sockaddr_in srcAddr;
    int addrLen = sizeof(srcAddr);

    //int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd<0)
    {
        printf("Socket Error: %s\n", strerror(errno));
        exit(1);
    }
    
    bzero(&addr, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    inet_aton("0.0.0.0", &addr.sin_addr);
    inet_aton("192.168.1.222", &addr.sin_addr);

    bind(sockfd, &addr, sizeof(addr));
    printf("c bind: %s\n", strerror(errno));

    while (1)
    {
        bzero(&srcAddr, sizeof(struct sockaddr_in));
        //len = recv(sockfd, buf, len, 0);
        len = recvfrom(sockfd, buf, 128, 0, &srcAddr, &addrLen);
        if (len > 0)
        {
            printf("received %d bytes from %s (len %d): %s\n", len, inet_ntoa(srcAddr.sin_addr), addrLen, buf);
        }
    }

    close(sockfd);
}

int main(int argc, char** argv)
{
    pthread_t rxTid;
    pthread_t txTid;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (argc < 2)
        return -1;

    if (strcmp(argv[1], "tx") == 0)
        txThread();
    else if (strcmp(argv[1], "rx") == 0)
        rxThread();
    else
    {
        pthread_create(&txTid, NULL, txThread, NULL);
        pthread_create(&rxTid, NULL, rxThread, NULL);
        pthread_join(rxTid);
        pthread_join(txTid);
    }
    return 0;
} 

