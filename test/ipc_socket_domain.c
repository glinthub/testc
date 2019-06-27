
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/un.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <features.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "common.h"

char *pathServer = "/tmp/socket_test_server";
char *pathClient = "/tmp/socket_test_client";

int test_server() {
    //AF_xxx is same with PF_xxx, protocol could be set 0.
    int sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    int ret = 0;
    struct sockaddr_un sa, sa_remote;
    sa.sun_family = AF_UNIX;
    strcpy(sa.sun_path, pathServer);

    unlink(pathServer);

    //bind is optional for SOCK_DGRAM
    ret = bind(sock, (struct sockaddr *)&sa, sizeof(struct sockaddr_un));
    EXIT_ON_ERR(ret==0);

    //listen is not for SOCK_DGRAM
    //ret = listen(sock, 1);
    EXIT_ON_ERR(ret==0);

    while (1)
    {
        struct sockaddr_un sa_in;
        socklen_t len = sizeof(struct sockaddr_un);

        //accept is not for SOCK_DGRAM
        //ret = accept(sock, (struct sockaddr *)&sa_in, &len);
        EXIT_ON_ERR(ret==0);

        char buf[128];
        while (1)
        {
            ret = recvfrom(sock, buf, 128, 0, (struct sockaddr *)&sa_remote, &len);
            if (ret > 0)
            {
                buf[ret] = 0;
                printf("received from %s: %s\n", sa_remote.sun_path, buf);
                char *response = "hi, response from server";
                strcpy(buf, response);
                sendto(sock, buf, strlen(buf), 0, (struct sockaddr *)&sa_remote, len);
            }
            else
            {
                break;
            }
        }
    }
}

int test_client()
{
    int sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    int ret = 0;
    struct sockaddr_un sa, sa_remote;
    sa.sun_family = AF_UNIX;
    strcpy(sa.sun_path, pathClient);

    unlink(pathClient);

    //bind is optional for SOCK_DGRAM
    ret = bind(sock, (struct sockaddr *)&sa, sizeof(struct sockaddr_un));
    EXIT_ON_ERR(ret==0);

    socklen_t len = sizeof(struct sockaddr_un);

    char buf[128];
    int i = 0;
    while (1)
    {
        sa_remote.sun_family = AF_UNIX;
        strcpy(sa_remote.sun_path, pathServer);
        sprintf(buf, "hello, greeting from client, seq %d", i);
        printf("Send to %s: %s\n",
               sa_remote.sun_path,
               buf);
        sendto(sock, buf, strlen(buf), 0, (struct sockaddr *)&sa_remote, len);

        ret = recvfrom(sock, buf, 128, 0, (struct sockaddr *)&sa_remote, &len);
        if (ret > 0)
        {
            buf[ret] = 0;
            printf("recv from %s: %s\n",
                   sa_remote.sun_path,
                   buf);
        }
        else
        {
            break;
        }
        sleep(2);
        i++;
    }
}

int main(int argc, char *argv[])
{
    if (argc > 1 && argv[1][0] == 'c')
        test_client();
    else
        test_server();

    return 0;
}
