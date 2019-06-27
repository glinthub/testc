#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "common.h"

char *fifo_path = "/tmp/fifo_test";
int test_proc1()
{
    int ret = unlink(fifo_path);
    //EXIT_ON_ERR(ret==0);
    ret = mkfifo(fifo_path, 0777);
    EXIT_ON_ERR(ret==0);
    int fd = open(fifo_path, O_RDONLY);
    EXIT_ON_ERR(fd>0);
    int i = 0;
    char msg[100];
    while(1)
    {
        ret = read(fd, msg, 100);
        if (ret > 0)
        {
            msg[ret] = 0;
            printf("%d bytes read: %s\n", ret, msg);
        }
        sleep(1);
    }
}

int main(int argc, char *argv[])
{
    test_proc1(); //write
    //for read, just cat the fifo file in linux shell.
    //if no reader, the writer will be blocked.
    //if reader/writer exit, the other site will receive the signal SIGPIPE.

    //for write, just echo xxx > fifo_file
    return 0;
}
