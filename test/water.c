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

typedef struct user {
	char* username;
	char* password;
	char* email;
	pthread_t tid;
} user_t;

user_t users[] = {
	{"r00001",	 		"kkkkkk",	"dreamview@163.com",	0},
	{"猪也是这么想的",	"kkkkkk",	"pigthink@21cn.com",	0},
	{"护球像亨利",		"kkkkkk",	"pigthink@21cn.com",	0},
	{"小人物上篮", 		"kkkkkk",	"dreamview@163.com",	0},
	{"郁君", 			"kkkkkk",	"pigthink@21cn.com",	0}
	/*
	{"apache2002", 		"kkkkkk",	"apache2002@163.net",	0},
	*/

	/* blocked
	{"大桥六线", 		"kkkkkk",	"802dot1x@21cn.com",	0},
	{"稀饭小菜",		"kkkkkk",	"xiaocaixifan@21cn.com",	0},
	{"wanersini",		"kkkkkk",	"xifanxiaocai@21cn.com",	0},
	{"洞房之猪",		"kkkkkk",	"dongfangpig@21cn.com",	0},
	{"南海浮云",		"kkkkkk",	"dongfangpig@21cn.com",	0},
	{"花开在天涯",		"kkkkkk",	"dongfangpig@21cn.com",	0},
	{"安史之绿",		"kkkkkk",	"eastgreen@21cn.com",	0},
	{"开始玩自己",		"kkkkkk",	"eastgreen@21cn.com",	0},
	{"借你我的肩膀",	"kkkkkk",	"eastgreen@21cn.com",	0},
	{"谜城",			"kkkkkk",	"enigmacity@21cn.com",	0},
	{"盗墓迷城",		"kkkkkk",	"enigmacity@21cn.com",	0},
	{"盗墓谜城", 		"kkkkkk",	"enigmacity@21cn.com",	0},
	{"不一般的酒佬爷", 	"kkkkkk",	"@",	0},
	{"小菜稀饭",		"kkkkkk",	"xiaocaixifan@21cn.com",	0},
	{"酒佬爷",			"kkkkkk",	"@",	0},
	{"英明神武一统江湖","kkkkkk",	"dreamview@163.com",	0},
	{"一统江湖英明神武","kkkkkk",	"@",	0},
	{"英明神武",		"kkkkkk",	"?",	0},
	*/
};

char poems[102][64];
int poem_num;
int load_poem()
{
	int i = 0;
	FILE* fp = fopen("poem.txt", "r");

	while(fgets(poems[i], 64, fp))
	{
		poems[i][strlen(poems[i])-1] = '\0';
		/*
		printf("%p, %p\n", poems[i], cpp);
		cpp++;
		*/
		i++;
	}
	fclose(fp);

	return i;
}

void water(char* username)
{
    int sockfd;
    char buffer[1024];
    char tmp[64];
    struct sockaddr_in server_addr;
    struct hostent* host;
    int portnumber, nbytes;
    uint8 http_data[1024];
    uint8 http_data1[1024];
    char strTitle[64] = {0};
    char content_length[5] = {0};
    time_t tm;
    int error;
    uint8 *cp, *cp1;
    uint32 score = 0;

	cp1 = http_data1;
	cp1 += sprintf(cp1, 
		"idItem=41&idArticle=629287&strTitle=%s"
		"&idSign=1&strWriter=%s"
		"&strPassword=kkkkkk&Submit=Response&strContent=hahaha&intSign=0&strPicURL=",
		/*strTitle*/"water.c:271: warning:", username
		);
	
    cp = http_data;
    cp += sprintf(cp, 
    	"POST /new/techforum/content.asp?idwriter=0&key=0 HTTP/1.1\n"
    	"Accept: */*\n"
    	"Referer: http://www1.tianya.cn/new/TechForum/Content.asp?idWriter=0&Key=0&idItem=41&idArticle=629287\n"
    	"Accept-Language: zh-cn\n"
    	"Content-Type: application/x-www-form-urlencoded\n"
    	"Accept-Encoding: gzip, deflate\n"
    	"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0)\n"
    	"Host: www1.tianya.cn\n"
    	"Connection: Keep-Alive\n"
    	"Cache-Control: no-cache\n"
 		"Content-Length: %d\n"
 		"\n",
		cp1-http_data1
   		);
   	memcpy(cp, http_data1, cp1-http_data1);
   	cp += (cp1-http_data1);
   	*cp = '\0';

	while(1)
	{
		error = 0;

	    srand(time(NULL));
		strcpy(strTitle, poems[rand()%poem_num]);
		sprintf(content_length, "%d", 125 + strlen(strTitle) + strlen(username));

	    /*客户程序开始建立sockfd描述符*/
	    while ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	    {
	        sprintf(strTitle, "Socket Error: %s", strerror(errno));
	        sleep(100);
	    }
	    //PINT(sockfd);

	    if((host = gethostbyname("www1.tianya.cn")) == NULL)
	    {
	        sprintf(strTitle, "Gethostname error");
	        error = 1;
	        goto water_error;
	    }

	    portnumber = 80;
	    
	    /*客户程序填充服务端的资料*/
	    bzero(&server_addr, sizeof(server_addr));
	    server_addr.sin_family = AF_INET;
	    server_addr.sin_port = htons(portnumber);
	    server_addr.sin_addr = *((struct in_addr*)host->h_addr);
	    
	    /*
	    PINT(server_addr.sin_addr.S_un.S_addr);
	    server_addr.sin_port = htons(23);
	    server_addr.sin_addr.S_un.S_addr = htonl((192<<24) + (168<<16) + (20<<8) + (69));
	    */

		if(connect(sockfd, (struct sockaddr*)(&server_addr), sizeof(struct sockaddr)) == -1)
	    {
	        sprintf(strTitle, "Connect Error: %s", strerror(errno));
	        error = 1;
	        goto water_error;
	    }

		{
		/*
		LINGER m_sLinger;
		m_sLinger.l_onoff = 1;  // (在closesocket()调用,但是还有数据没发送完毕的时候容许逗留)
		m_sLinger.l_linger = 0; // (容许逗留的时间为0秒)
		setsockopt(sockfd,
			SOL_SOCKET,
			SO_LINGER,
			(const char*)&m_sLinger,
			sizeof(LINGER));
		*/
		}
 
		/**/
		if((nbytes = send(sockfd, http_data, cp-http_data, 0)) <= 0)
		{
			sprintf(strTitle, "Send error: %s", strerror(errno));
	        error = 1;
	        goto water_error;
		}

	    if((nbytes = recv(sockfd, buffer, 1024, 0)) == -1)
	    {
	        sprintf(strTitle, "Recv Error: %s", strerror(errno));
	        error = 1;
	        goto water_error;
	    }

	    /*
	    */
	    buffer[nbytes] = '\0';
	    printf("I have received---------------------------\n%s ", buffer);

water_error:
	    tm = time(NULL);
	    strcpy(tmp, ctime(&tm));
	    tmp[strlen(tmp)-1] = '\0';
    	//sprintf(buffer, "%s %s: %s\n", tmp, username, strTitle);
    	//printf("%s", buffer);
    	printf("%s %s(%d): %s\n", tmp, username, score, strTitle);
    	//mem_dump(buffer, strlen(buffer));

	    /*结束通讯*/
		if(sockfd)
	    {
			//closesocket(sockfd);	/*winsock uses closesocket instead of close */
			close(sockfd);
		}

		if(error)
		{
			sleep(10000);
		}
		else
		{
			score += 3;
			sleep(60000);
		}
	}

}

int water_main(int argc, char** argv)
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

	poem_num = load_poem();
	/*user_num = sizeof(users) / sizeof(user_t);*/
	user_num = 1;

	printf("%d user(s) total.\n", user_num);
	for(i=0; i<user_num; i++)
	{
		pthread_create(&(users[i].tid), NULL, (void *)water, users[i].username);
		printf("pthread_create: %s\n", users[i].username);
		/*sleep(60000/user_num);*/
	}

	while(1)
	{
		sleep(120000);
	}

} 

