#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <assert.h>
#include <semaphore.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/types.h>

#include <time.h>
#include <locale.h>

#ifdef _WIN32
#include <winsock2.h>
#include <wininet.h>
#else
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#endif

#include "test.h"

#define uint8 unsigned char
#define uint16 unsigned short
#define uint32 unsigned int
#define uint64 unsigned long long

#define int8 char
#define int16 short
#define int32 int
#define int64 long long

#define bzero(addr, count) memset(addr, 0, count)

#define PADDR(val)	printf(#val" addrs:\t0x%08x\n", (uint32)&(val));
#define PINT(val)	printf(#val" value:\t0x%08x\n", (uint32)(val))
#define PVAL(val)	printf(#val" value:\t%d\n", val)

void f()
{
    int sockfd;
    char buf[66666];
    char tmp[64];
    struct sockaddr_in server_addr;
    struct hostent* host;

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("Socket Error: %s", strerror(errno));
        exit(1);
    }
    //PINT(sockfd);

    if((host = gethostbyname("www.tianyaclub.com")) == NULL)
    {
        printf("Gethostname error");
        exit(1);
    }

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80);
    server_addr.sin_addr = *((struct in_addr*)host->h_addr);
    
	if(connect(sockfd, (struct sockaddr*)(&server_addr), sizeof(struct sockaddr)) == -1)
    {
        printf("Connect Error: %s", strerror(errno));
        exit(1);
    }

	/**/
	memset(buf, 'a', 66666);
	if(send(sockfd, buf, 66000, 0) <= 0)
	{
		printf("Send error: %s", strerror(errno));
        exit(1);
	}

	if(sockfd)
    {
		//closesocket(sockfd);	/*winsock uses closesocket instead of close */
		close(sockfd);
	}

}

int tcp_test(int argc, char** argv)
{
	int i, user_num;

	#ifdef _WIN32
	WSADATA WSAData;
	if( WSAStartup(0x0110, &WSAData) ) {
	//if( WSAStartup(WINSOCK_VERSION, &WSAData) ) {
		printf("WSA error");
		exit(-1);
	}
	#endif

	f();
} 

