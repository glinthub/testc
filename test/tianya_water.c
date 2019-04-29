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

#ifdef _WIN32
#include <winsock2.h>
#include <wininet.h>
#else
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#endif

#include <zws/global.h>
#include "test.h"

#define uint8 unsigned char
#define uint16 unsigned short
#define uint32 unsigned long
#define uint64 unsigned long long

#define PADDR(val) printf(#val" addrs:\t0x%08x\n", &(val));
#define PINT(val) printf(#val" value:\t0x%08x\n", (val))

#define MAX_SIZE 1500
int water_main(int argc, char * argv[]) {
    int sockfd;
    char buffer[MAX_SIZE];
    int buf_len = 0;
    struct sockaddr_in server_addr;
    struct hostent* host;
    int portnumber, i, nbytes;

#ifdef _WIN32
    WSADATA WSAData;
    if( WSAStartup(0x0110, &WSAData) ) {
        //if( WSAStartup(WINSOCK_VERSION, &WSAData) ) {
        printf("WSA error");
        exit(-1);
    }
#endif

    /*
     if(argc != 3)
     {
     fprintf(stderr, "Usage: %s hostname portnumber \a\n", argv[0]);
     exit(1);
     }

     if((host = gethostbyname(argv[1])) == NULL)
     {
     fprintf(stderr, "Gethostname error\n");
     exit(1);
     }

     if((portnumber = atoi(argv[2]))<0)
     {
     fprintf(stderr, "Usage: %s hostname portnumber \a\n", argv[0]);
     exit(1);
     }
     */
    portnumber = 80;
    host = gethostbyname("www4.tianyaclub.com");

    /*客户程序开始建立sockfd描述符*/
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "Socket Error: %s \a\n", strerror(errno));
        //exit(1);
    }

    /*客户程序填充服务端的资料*/
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portnumber);
    server_addr.sin_addr = *((struct in_addr*) host->h_addr);

    /*客户程序发起连接请求*/
    if (connect(sockfd, (struct sockaddr*) (&server_addr),
            sizeof(struct sockaddr)) == -1) {
        fprintf(stderr, "Connect Error: %s \a\n", strerror(errno));
        exit(1);
    }

    /*连接成功了*/
    /*
     PINT(sockfd);
     PINT(buf_len);
     PINT(buffer);
     printf("%s\n", buffer);
     */

    memset(buffer, 0, MAX_SIZE);
    fff(&buf_len, MAX_SIZE, buffer, "POST");
    fff(&buf_len, MAX_SIZE, buffer,
            " /new/Publicforum/Content.asp?idWriter=0&Key=0&flag=1");
    fff1(&buf_len, MAX_SIZE, buffer, " HTTP/1.1");
//    fff1(&buf_len, MAX_SIZE, buffer, "Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, */*");
    fff1(&buf_len, MAX_SIZE, buffer, "Accept: */*");
    fff1(&buf_len, MAX_SIZE, buffer,
            "Referer: http://www.tianyaclub.com/new/Publicforum/Content.asp?idWriter=1536851&Key=0&idArticle=365147&strItem=no04&flag=1&#Bottom");
    fff1(&buf_len, MAX_SIZE, buffer, "Accept-Language: zh-cn");
    fff1(&buf_len, MAX_SIZE, buffer,
            "Content-Type: application/x-www-form-urlencoded");
    fff1(&buf_len, MAX_SIZE, buffer, "Accept-Encoding: gzip, deflate");
    fff1(&buf_len, MAX_SIZE, buffer,
            "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0)");
    fff1(&buf_len, MAX_SIZE, buffer, "Host: www.tianyaclub.com");
    fff1(&buf_len, MAX_SIZE, buffer, "Content-Length: 386");
    fff1(&buf_len, MAX_SIZE, buffer, "Connection: Keep-Alive");
    fff1(&buf_len, MAX_SIZE, buffer, "Cache-Control: no-cache");
//    fff1(&buf_len, MAX_SIZE, buffer, "Cookie: ASPSESSIONIDSCAQRDST=BLAHFPJDEFNKNFOADNEHAOJP; __support_check=1");
    fff1(&buf_len, MAX_SIZE, buffer, NULL);

    fff(&buf_len, MAX_SIZE, buffer, "strItem=no04");
    fff(&buf_len, MAX_SIZE, buffer,
            "&strTitle=[五花八门]现在的套套广告~~~~~~哎(转载)");
    fff(&buf_len, MAX_SIZE, buffer, "&idArticle=365147");
    fff(&buf_len, MAX_SIZE, buffer, "&flag=1");
    fff(&buf_len, MAX_SIZE, buffer, "&idSign=1");
    fff(&buf_len, MAX_SIZE, buffer, "&strWriter=apache2002");
    fff(&buf_len, MAX_SIZE, buffer, "&strPassword=kkkkkk");
    fff(&buf_len, MAX_SIZE, buffer, "&Submit=Response");
    fff(&buf_len, MAX_SIZE, buffer, "&strContent=%20");
    fff(&buf_len, MAX_SIZE, buffer,
            "&strPicURL=http://dreamstep.51.net/windseason/ip1.php");

    /*
     PINT(sockfd);
     PINT(buf_len);
     PINT(buffer);
     printf("%s\n", buffer);
     */

    /*请求页面*/
    if (send(sockfd, buffer, buf_len, 0) == -1) {
        fprintf(stderr, "Write error: %s \n", strerror(errno));
        exit(1);
    }

    /*读server返回的页面数据*/
    for (i = 0; i < 2; i++) {
        bzero(buffer, MAX_SIZE);
        if ((nbytes = recv(sockfd, buffer, MAX_SIZE, 0)) > 0) {
            printf("#%d: %d bytes received:\n%s\n", i, nbytes, buffer);
        } else
            printf("#%d: Error: recv return %d!(%s)\n", i, nbytes,
                    strerror(errno));
    }

    /*结束通讯*/
    //sleep(10000);
    //close(sockfd);
    printf("Disconnect.\n");
    exit(0);
}

void fff(int* cur_len, int max_len, char* dst, char* src) {
    if (src == NULL || (*cur_len) + strlen(src) > max_len)
        return;

    memcpy(dst + (*cur_len), src, strlen(src));
    (*cur_len) += strlen(src);

    return;
}

void fff1(int* cur_len, int max_len, char* dst, char* src) {
    fff(cur_len, max_len, dst, src);
    dst[(*cur_len)++] = 0x0D;
    dst[(*cur_len)++] = 0x0A;
    /*
     */

    return;
}
