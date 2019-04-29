#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "common.h"

int main(int argc, char *argv[])
{
    /*
     * fd[0] is for read, fd[1] is for write.
     * reversing them will cause error.
     */
    int fd[2];
    int ret = 0;
    ret = pipe(fd);
    CHECK_ERR(ret == 0, exit(1));
    printf("fd: %d %d\n", fd[0], fd[1]);
    pid_t pid = fork();
    if (pid == 0) //child process
    {
        char *msg = "hi, i'm child.";
        ret = write(fd[1], msg, strlen(msg));
        CHECK_ERR(ret > 0, exit(1));
    }
    else //parent proces
    {
        char buf[100];
        ret = read(fd[0], buf, 100);
        CHECK_ERR(ret > 0, exit(1));
        buf[ret] = 0;
        printf("received: %s\n", buf);
    }
    return 0;
}
