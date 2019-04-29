#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/un.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>

#define MGNT_UNIX_DOMAIN "UNIX.domain"
#define MGNT_MAX_BUF_LEN 1024
/**************** Server ****************/
int mgnt_server_com_fd = 0;
void (*receiver)(int com_fd, uint8_t *buf, uint8_t len) = NULL;

void mgnt_server_receiver_register(void (*recv)(int com_fd, uint8_t *buf, uint8_t len))
{
    receiver = recv;
}

void mgnt_server_send(int com_fd, uint8_t *buf, uint8_t len)
{
    assert(mgnt_server_com_fd == com_fd);
    assert(len < MGNT_MAX_BUF_LEN);
    write(com_fd, buf, len);
}

int mgnt_server(uint8_t *filename)
{
    uint8_t *sockFileName = NULL;
    if (filename != NULL)
        sockFileName = filename;
    socklen_t clt_addr_len;
    int listen_fd;
    int com_fd;
    int ret;
    int i;
    static char rcv_buff[MGNT_MAX_BUF_LEN];
    int len;
    if (sockFileName)
    {
        listen_fd=socket(AF_UNIX,SOCK_STREAM,0);
    }
    else
    {
        listen_fd=socket(AF_INET, SOCK_STREAM, 0);
        int optValue = 1;
        setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optValue, (socklen_t)sizeof(optValue));
    }
    if(listen_fd<0){
        perror("Failed to creat communication socket");
    }

    // set srv_addr param
    struct sockaddr sa;
    if (sockFileName)
    {
        struct sockaddr_un *srv_addr = (struct sockaddr_un *)&sa;
        srv_addr->sun_family = AF_UNIX;
        strncpy(srv_addr->sun_path, sockFileName, sizeof(srv_addr->sun_path)-1);
        unlink(sockFileName);
    }
    else
    {
        struct sockaddr_in *srv_addr = (struct sockaddr_in *)&sa;
        srv_addr->sin_addr.s_addr = INADDR_ANY;
        srv_addr->sin_family = AF_INET;
        srv_addr->sin_port = htons(8888);
    }

    //bind sockfd&addr
    ret = bind(listen_fd, &sa, sizeof(struct sockaddr));
    if (ret < 0)
    {
        perror("cannot bind server socket");
        close(listen_fd);
        if (sockFileName)
            unlink(sockFileName);
        return -1;
    }

    //listen sockfd
    ret=listen(listen_fd,1);
    if (ret < 0){
        perror("cannot listen sockfd");
        close(listen_fd);
        if (sockFileName)
            unlink(sockFileName);
        return -1;
    }

    while (1)
    {
        //have connect requst use accept
        struct sockaddr clt_addr;
        len=sizeof(clt_addr);
        com_fd = accept(listen_fd,(struct sockaddr*)&clt_addr,&len);
        if (com_fd < 0) {
            perror("cannot accept requst");
            close(listen_fd);
            if (sockFileName)
                unlink(sockFileName);
            return -1;
        }
        mgnt_server_com_fd = com_fd;

        //read and printf client send info
        printf("## Management client connected.\n");
        while (1)
        {
            int num = read(com_fd, rcv_buff, sizeof(rcv_buff));
            if (num == 0)
            {
                printf("## Management client lost.\n");
                break;
            }
            rcv_buff[num] = 0;
            printf("received %d bytes from client: %s\n", num, rcv_buff);
            if (receiver != NULL)
                receiver(com_fd, rcv_buff, num);
        }
    }
    close(com_fd);
    close(listen_fd);
    if (sockFileName)
        unlink(sockFileName);
    return 0;

}

/**************** Client ****************/
int mgnt_client(uint8_t *filename, uint8_t *cmd)
{
    uint8_t *sockFileName = MGNT_UNIX_DOMAIN;
    if (filename != NULL)
        sockFileName = filename;
    int connect_fd;
    int ret;
    char buf[MGNT_MAX_BUF_LEN];
    int i;
    static struct sockaddr_un srv_addr;
    // creat unix socket
    connect_fd=socket(PF_UNIX,SOCK_STREAM,0);
    if(connect_fd<0){
        perror("cannot creat socket");
        return -1;
    }
    srv_addr.sun_family=AF_UNIX;
    strcpy(srv_addr.sun_path, sockFileName);
    //connect server
    ret=connect(connect_fd,(struct sockaddr*)&srv_addr,sizeof(srv_addr));
    if (ret<0){
        perror("cannot connect server");
        close(connect_fd);
        return -1;
    }
    memset(buf,0,MGNT_MAX_BUF_LEN);
    while (1)
    {
        printf("> ");

        if (cmd)
        {
            strcpy(buf, cmd);
            if (strlen(buf) < 1)
                break;
        }
        else
        {
            fgets(buf, MGNT_MAX_BUF_LEN, stdin);
            if (strlen(buf) <= 1)
                continue;
        }

        buf[strlen(buf)-1] = 0;
        int num = write(connect_fd, buf, strlen(buf));
        printf("%d bytes sent.\n", num);
        if (num == 0)
        {
            printf("0 bytes sent.\n");
            break;
        }

        num = read(connect_fd, buf,  sizeof(buf));
        printf("%d bytes received.\n", num);
        if (num == 0)
        {
            break;
        }
        buf[num] = 0;
        printf("%s\n", buf);

        if (cmd)
            break;
    }
    close(connect_fd);
    return 0;
}
