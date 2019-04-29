#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>
#include <sys/types.h>

#ifdef _WIN32
#include <winsock2.h>
#include <wininet.h>
#else
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#endif

#define uint8 unsigned char
#define uint16 unsigned short
#define uint32 unsigned long
#define uint64 unsigned long long

#define PADDR(val) printf(#val" addrs:\t0x%08x\n", &(val));
#define PINT(val) printf(#val" value:\t0x%08x\n", (val))

#define MAX_SIZE 1500

#define debug_printf if (debug) printf

static uint32 debug;

void fff(int* cur_len, int max_len, char* dst, char* src)
{
    if( src == NULL  ||
        (*cur_len) + strlen(src) > max_len )
        return;

    memcpy(dst + (*cur_len), src, strlen(src));
    (*cur_len) += strlen(src);

    return;
}

void fff1(int* cur_len, int max_len, char* dst, char* src)
{
    fff(cur_len, max_len, dst, src);
    dst[(*cur_len)++] = 0x0D; 
    dst[(*cur_len)++] = 0x0A;
    /*
    */

    return;
}

int get_content_length(char *buf)
{
	char *key = "\r\nContent-Length: ";
	char *cp1, *cp2;
	char str_content_length[8];
	cp1 = strstr(buf, key);
	if (!cp1)
		return 0;
	cp1 += strlen(key);
	cp2 = strchr(cp1, '\r');
	if (!cp2)
		return 0;
	strncpy(str_content_length, cp1, min(cp2-cp1, 8-1));
	return atoi(str_content_length);
}

int read_page(int sockfd, char* path)
{
    char buffer[MAX_SIZE];
	int nbytes, content_length = 0, received_length = 0;
	char *cp1, *cp2;
	char output_buf[128] = {0};
    int buf_len = 0;
	int i;
	
    memset(buffer, 0, MAX_SIZE);
    fff(&buf_len, MAX_SIZE, buffer, "GET ");
    fff(&buf_len, MAX_SIZE, buffer, path);
    fff1(&buf_len, MAX_SIZE, buffer, " HTTP/1.1"); 
    fff1(&buf_len, MAX_SIZE, buffer, "Accept: */*");
    fff1(&buf_len, MAX_SIZE, buffer, "Accept-Language: zh-cn");
    fff1(&buf_len, MAX_SIZE, buffer, "Accept-Encoding: gzip, deflate");
    fff1(&buf_len, MAX_SIZE, buffer, "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0)");
    fff(&buf_len, MAX_SIZE, buffer, "Host: ");
	fff1(&buf_len, MAX_SIZE, buffer, "www.cnolnic.com");
    fff1(&buf_len, MAX_SIZE, buffer, "Connection: Keep-Alive");
    fff1(&buf_len, MAX_SIZE, buffer, NULL);

    if(send(sockfd, buffer, buf_len, 0) == -1)
    {
        fprintf(stderr, "##### Write error: %s \n", strerror(errno));
        return -1;
    }
	debug_printf("##### sent!\n");
    
    /*读server返回的页面数据*/
    while (1) {
        memset(buffer, 0, MAX_SIZE);
		nbytes = recv(sockfd, buffer, MAX_SIZE, 0);
        if (nbytes > 0)
		{
			if (!content_length)
			{
				content_length = get_content_length(buffer);
				debug_printf("##### %d: Content-Length: %d\n", i, content_length);
				if (!content_length)
					break;
			}
			if (!received_length)
			{
				cp1 = strstr(buffer, "\r\n\r\n");
				if (cp1)
				{
					received_length = nbytes - (cp1 + 4 - buffer);
				}
			}
			else
				received_length += nbytes;

            debug_printf("##### %d: %d bytes received(content %d/%d):\n%s\n", i, nbytes, received_length, content_length, buffer);
            cp1 = strchr(buffer, '[');
			while (cp1)
			{
				cp2 = strchr(cp1, '<');
				if (cp2)
				{
					strncpy(output_buf, cp1, min(cp2-cp1, 128-1));
					output_buf[min(cp2-cp1, 128-1)] = '\0';
					printf("%s\n", output_buf);
					cp1 = strchr(cp2, '[');
				}
				else
					cp1 = NULL;
			}
			if (received_length >= content_length)
				break;
        }
        else
        {
			printf("##### %d: Error: recv return %d!(%s)\n", i, nbytes, strerror(errno));
			return -2;
        }
		i++;
    }
	return 0;
}

int get_char_index(char ch)
{
	if (ch >= '0' && ch <= '9')
		return ch - '0';
	if (ch > 'a' && ch <='z')
		return ch - 'a' + 10;
	if (ch > 'A' && ch <='Z')
		return ch - 'A' + 10;
	return 0;
}

int scan(char* start)
{
    int sockfd;
	int i, j, k;
	char hostname[64] = "www.cnolnic.com";
	char path[256] = {0};
	struct sockaddr_in server_addr;
    struct hostent* host;
    int portnumber = 80;
	char ch[] = "0123456789abcdefghijklmnopqrstuvwxyz";

	#ifdef _WIN32
	WSADATA WSAData;
	if( WSAStartup(0x0110/*WINSOCK_VERSION*/, &WSAData) ) {
		printf("##### WSA error");
		exit(-1);
	}
	#endif

	i = j = k = -1;
	if (start)
	{
		int len = strlen(start);
		k = start[len - 1];
		if (len > 1)
			j = start[len - 2];
		if (len > 2)
			i = start[len-3];
	}
    host = gethostbyname(hostname);

    /*客户程序开始建立sockfd描述符*/
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Socket Error: %s \a\n", strerror(errno));
        //exit(1);
    }

    /*客户程序填充服务端的资料*/
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portnumber);
    server_addr.sin_addr = *((struct in_addr*)host->h_addr);

    /*客户程序发起连接请求*/
    if(connect(sockfd, (struct sockaddr*)(&server_addr), sizeof(struct sockaddr)) == -1)
    {
        fprintf(stderr, "Connect Error: %s \a\n", strerror(errno));
        exit(1);
    }

	debug_printf();
    /*连接成功了*/
    /*
    PINT(sockfd);
    PINT(buf_len);
    PINT(buffer);
    */

	for (; i < 36; i++)
	{
		for (; j < 36; j++)
		{
			for (; k < 36; j++)
			{
				if (i == -1 && j == -1)
					sprintf(path, "/cgi-bin1/domainregister/chinreg8.cgi?domainname=%c&cn=TRUE&comcn=TRUE", ch[k]);
				else if (i == -1)
					sprintf(path, "/cgi-bin1/domainregister/chinreg8.cgi?domainname=%c%c&cn=TRUE&comcn=TRUE", ch[j], ch[k]);
				else
					sprintf(path, "/cgi-bin1/domainregister/chinreg8.cgi?domainname=%c%c%c&cn=TRUE&comcn=TRUE", ch[i], ch[j], ch[k]);
				if (0 != read_page(sockfd, path))
					return -1;
				sleep(1000);
			}
			k = 0;
		}
		j = 0;
	}

    /*结束通讯*/
    //sleep(10000);
    //close(sockfd);
    debug_printf("##### Disconnect.\n");
    exit(0);
}

int ds_zzy_main(int argc, char * argv[])
{
	int i;
	
	for (i = 1; i<argc; i++)
	{
		if (!stricmp(argv[i], "-debug"))
		{
			debug = 1;
			break;
		}
	}

	return scan(argc > 1 ? argv[1] : NULL);
}

