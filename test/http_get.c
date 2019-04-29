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

/*运算符优先级：* > +, & > +*/
void mem_dump(void* buf_ptr, int len) {
	printf("memory of address 0x%X to 0x%X, %d bytes\n", (int)buf_ptr, (int)((char*)buf_ptr + len - 1), len);
	char* cp1 = (char*)((int)((char*)buf_ptr) / 16 * 16);
	char* cp2 = (char*)((int)((char*)buf_ptr + len - 1) / 16 * 16 + 15);
	int rows = (cp2 - cp1 + 1) / 16;
	int i, j;
	for(i = 0; i < rows; i++) {
		printf("0x%08X\t", (int)(cp1 + 16*i));
		for(j = 0; j < 16; j++) {
			printf("%02x", *(cp1 + 16*i + j));
			if(!((j+1)%4)) printf(" ");
		}
		printf("\t");
		for(j = 0; j < 16; j++) {
			if(isgraph(*(cp1 + 16*i + j))) printf("%c", *(cp1 + 16*i + j));
			else  printf(".");
		}
		printf("\n");
	}
}

#define MAX_SIZE 1500

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

int http_get_main(int argc, char * argv[])
{
    int sockfd;
    char buffer[MAX_SIZE];
    int buf_len = 0;
	char url[512];
	char *cp1, *cp2;
	char str_hostname[64] = {0};
	char str_portnum[16] = {0};
	char str_path[256] = {0};
	struct sockaddr_in server_addr;
    struct hostent* host;
    int portnumber, i=0, nbytes;
	int content_length = 0;
	int received_length = 0;

	#ifdef _WIN32
	WSADATA WSAData;
	if( WSAStartup(0x0110/*WINSOCK_VERSION*/, &WSAData) ) {
		printf("##### WSA error");
		exit(-1);
	}
	#endif

    if(argc != 2)
    {
        fprintf(stderr, "Usage: %s <url>\a\n", argv[0]);
        exit(1);
    }

	//strcpy(url, argv[1]);
	strcpy(url, "http://www.cnolnic.com/cgi-bin1/domainregister/chinreg8.cgi?domainname=boda&cn=TRUE&comcn=TRUE");
	cp1 = url;
	if (!strnicmp("http://", url, 7))
		cp1 = url + 7;
	cp2 = strchr(cp1, '/');
	if (!cp2)
		cp2 = cp1 + strlen(cp1);
	strncpy(str_hostname, cp1, min((cp2-cp1), 64-1));
	strcpy(str_path, cp2);

	portnumber = 80;
	cp1 = cp2;
	if (*cp1 == ':')
	{
		cp2 = strchr(cp1, '/');
		if (!cp2)
			cp2 = cp1 + strlen(cp1);
		strncpy(str_portnum, cp1, min((cp2-cp1), 16-1));
		portnumber = atoi(str_portnum);
	}
	printf(" url: %s\n hostname: %s\n port: %d\n path: %s\n", url, str_hostname, portnumber, str_path);

    host = gethostbyname(str_hostname);

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

    /*连接成功了*/
    /*
    PINT(sockfd);
    PINT(buf_len);
    PINT(buffer);
    */

    memset(buffer, 0, MAX_SIZE);
    fff(&buf_len, MAX_SIZE, buffer, "GET ");
    fff(&buf_len, MAX_SIZE, buffer, str_path);
    fff1(&buf_len, MAX_SIZE, buffer, " HTTP/1.1"); 
//  fff1(&buf_len, MAX_SIZE, buffer, "Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, */*");
    fff1(&buf_len, MAX_SIZE, buffer, "Accept: */*");
    /*
    fff(&buf_len, MAX_SIZE, buffer, "Referer: ");
	fff1(&buf_len, MAX_SIZE, buffer, url);
	*/
    fff1(&buf_len, MAX_SIZE, buffer, "Accept-Language: zh-cn");
	/*
    fff1(&buf_len, MAX_SIZE, buffer, "Content-Type: application/x-www-form-urlencoded");
    */
    fff1(&buf_len, MAX_SIZE, buffer, "Accept-Encoding: gzip, deflate");
    fff1(&buf_len, MAX_SIZE, buffer, "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0)");
    fff(&buf_len, MAX_SIZE, buffer, "Host: ");
	fff1(&buf_len, MAX_SIZE, buffer, str_hostname);
    /*fff1(&buf_len, MAX_SIZE, buffer, "Content-Length: 386");*/
    fff1(&buf_len, MAX_SIZE, buffer, "Connection: Keep-Alive");
    /*fff1(&buf_len, MAX_SIZE, buffer, "Cache-Control: no-cache");*/
	/*fff1(&buf_len, MAX_SIZE, buffer, "Cookie: ASPSESSIONIDSCAQRDST=BLAHFPJDEFNKNFOADNEHAOJP; __support_check=1");*/
    fff1(&buf_len, MAX_SIZE, buffer, NULL);

	/*
    fff(&buf_len, MAX_SIZE, buffer, "strItem=no04");
    fff(&buf_len, MAX_SIZE, buffer, "&strTitle=[五花八门]现在的套套广告~~~~~~哎(转载)");
    fff(&buf_len, MAX_SIZE, buffer, "&idArticle=365147");
    fff(&buf_len, MAX_SIZE, buffer, "&flag=1");
    fff(&buf_len, MAX_SIZE, buffer, "&idSign=1");
    fff(&buf_len, MAX_SIZE, buffer, "&strWriter=apache2002");
    fff(&buf_len, MAX_SIZE, buffer, "&strPassword=kkkkkk");
    fff(&buf_len, MAX_SIZE, buffer, "&Submit=Response");
    fff(&buf_len, MAX_SIZE, buffer, "&strContent=%20");
    fff(&buf_len, MAX_SIZE, buffer, "&strPicURL=http://dreamstep.51.net/windseason/ip1.php");
    */

    /*
    PINT(sockfd);
    PINT(buf_len);
    PINT(buffer);
    printf("%s\n", buffer);
    */
    
    /*请求页面*/
    if(send(sockfd, buffer, buf_len, 0) == -1)
    {
        fprintf(stderr, "##### Write error: %s \n", strerror(errno));
        exit(1);
    }
	printf("##### sent!\n");
    
    /*读server返回的页面数据*/
    while (1) {
        memset(buffer, 0, MAX_SIZE);
		nbytes = recv(sockfd, buffer, MAX_SIZE, 0);
        if (nbytes > 0)
		{
			if (!content_length)
			{
				content_length = get_content_length(buffer);
				printf("##### %d: Content-Length: %d\n", i, content_length);
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

            printf("##### %d: %d bytes received(content %d/%d):\n%s\n", i, nbytes, received_length, content_length, buffer);
			if (received_length >= content_length)
				break;
        }
        else
        {
			printf("##### %d: Error: recv return %d!(%s)\n", i, nbytes, strerror(errno));
			break;
        }
		i++;
    }
    

    /*结束通讯*/
    //sleep(10000);
    //close(sockfd);
    printf("##### Disconnect.\n");
    exit(0);
}

