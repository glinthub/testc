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

uint32 debug = 0;
#define debug_printf if (debug) printf("####### ");printf

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

char *get_path(int server_id, char *domain_name, char *suffix)
{
	static char path[256] = {0};
	if (!stricmp(suffix, "cn"))
	{
		switch(server_id)
		{
			case 0:
				sprintf(path, "/cgi-bin/Check.cgi?queryType=0&domain1=%s&domain=%s&big5=n&sign=2&asia=&url=www.net.cn&cn=yes&image.x=23&image.y=14", domain_name, domain_name);
				break;
			case 1:
				sprintf(path, "http://100u.com/services/domain/whois/whois.asp?module=domainsearch&searchType=IntDomain&action=check&searchedDomainName=%s&suffix=.cn", domain_name);
				break;
			default:
				return NULL;
		}
	}
	else
		return NULL;
	return path;
}

char *get_host(int server_id)
{
	switch(server_id)
	{
		case 0:
			return "panda.www.net.cn";
			break;
		case 1:
			return "100u.com";
			break;
		default:
			return "xxx.xxx.xxx";
			return NULL;
	}
}

char *get_result(char *buffer, char *domain_name, int server_id)
{
	char *cp1;
	char *cp2;

	cp1 = cp2 = NULL;
	switch(server_id)
	{
		case 0:
		{
			if (strstr(buffer, "没有被注册"))
				return domain_name;
			break;
		}
		default:
			return NULL;
	}
	return NULL;
}

int read_page(int sockfd, int server_id, char *domain_name)
{
    char buffer[MAX_SIZE];
	int nbytes, content_length = 0, received_length = 0;
	char *cp1;
	char *output_buf;
    int buf_len = 0;
	int i = 0;
	char *path = get_path(server_id, domain_name, "cn");

	if (!path)
		return -1;

    memset(buffer, 0, MAX_SIZE);
    fff(&buf_len, MAX_SIZE, buffer, "GET ");
    fff(&buf_len, MAX_SIZE, buffer, path);
    fff1(&buf_len, MAX_SIZE, buffer, " HTTP/1.1"); 
    fff1(&buf_len, MAX_SIZE, buffer, "Accept: */*");
    fff1(&buf_len, MAX_SIZE, buffer, "Accept-Language: zh-cn");
    /*fff1(&buf_len, MAX_SIZE, buffer, "Accept-Encoding: gzip, deflate");*/
    fff1(&buf_len, MAX_SIZE, buffer, "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0)");
    fff(&buf_len, MAX_SIZE, buffer, "Host: ");
	fff1(&buf_len, MAX_SIZE, buffer, get_host(server_id));
    fff1(&buf_len, MAX_SIZE, buffer, "Connection: Keep-Alive");
    fff1(&buf_len, MAX_SIZE, buffer, NULL);

	nbytes = send(sockfd, buffer, buf_len, 0);
    if (nbytes == -1)
    {
        fprintf(stderr, "Write error: %s \n", strerror(errno));
        return -1;
    }
	debug_printf("sent(%d)!\n", nbytes);
    
    /*读server返回的页面数据*/
    while (1) {
        memset(buffer, 0, MAX_SIZE);
		nbytes = recv(sockfd, buffer, MAX_SIZE, 0);
        if (nbytes > 0)
		{
			if (!content_length)
			{
				content_length = get_content_length(buffer);
				debug_printf("%d: Content-Length: %d\n", i, content_length);
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

            debug_printf("%d: %d bytes received(content %d/%d):\n%s\n", i, nbytes, received_length, content_length, buffer);
            output_buf = get_result(buffer, domain_name, server_id);
            if (output_buf)
				printf(">>>>>>>>>%s\n", output_buf);
			else
				printf(":( %s\n", domain_name);
				
			if (received_length >= content_length)
				break;
        }
        else
        {
			printf("%d: Error: recv return %d!(%s)\n", i, nbytes, strerror(errno));
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

int scan(int line)
{
    int sockfd;
	FILE *fp;
	int i;
	int server_id = 1;
	char *hostname = get_host(server_id);
	struct sockaddr_in server_addr;
    struct hostent* host;
    int portnumber = 80;
	char domain_name[16] = {0};
	char *cp;

	#ifdef _WIN32
	WSADATA WSAData;
	if( WSAStartup(0x0110/*WINSOCK_VERSION*/, &WSAData) ) {
		printf("WSA error");
		exit(-1);
	}
	#endif

	fp = fopen("domain_list.txt", "r");
	if (!fp)
	{
		printf("domain_list.txt open fail. exit\n");
		return -1;
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

    /*连接成功了*/
	debug_printf("Connect success: sockfd %d\n", sockfd);

	while (fgets(domain_name, 16-1, fp))
	{
		cp = domain_name;
		while(*cp != '\n')
		{
			cp++;
		}
		*cp = '\0';
		if (++i < line)
			continue;
		debug_printf("read %s\n", domain_name);
		if (0 != read_page(sockfd, server_id, domain_name))
			break;
		sleep(1000);
	}

    /*结束通讯*/
    //sleep(10000);
    //close(sockfd);
    debug_printf("Disconnect.\n");
    return 0;
}

int ds_hichina_main(int argc, char * argv[])
{
	int i;

	for (i = 1; i < argc; i++)
	{
		if (!stricmp(argv[i], "--debug"))
		{
			debug = 1;
			break;
		}
	}
	return scan(0);
}

