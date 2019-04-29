#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <fcntl.h>

#include "common.h"

#define IPC_KEY 0123

struct msgbuf {
   long mtype;       /* message type, must be > 0 */
   char mtext[10];    /* message data */
};

int test_proc1()
{
    int ret = 0;
    int qid = msgget(IPC_KEY, IPC_CREAT|0666);
    EXIT_ON_ERR(qid!=-1);


    int i=0;
    char buf[100];
    struct msgbuf mb;
    while(1)
    {
        sprintf(buf, "seq %d\n", i++);
        printf("proc1 send: %s\n", buf);
        mb.mtype = 1;
        strcpy(mb.mtext, buf);
        ret = msgsnd(qid, (void*)&mb, strlen(buf), 0);
        EXIT_ON_ERR(ret==0);
        sleep(1);
    }
}

int test_proc2()
{
    int ret = 0;
    int qid = msgget(IPC_KEY, 0);
    EXIT_ON_ERR(qid!=-1);


    int i=0;
    char buf[100];
    struct msgbuf mb;
    while(1)
    {
        ret = msgrcv(qid, (void*)&mb, 100, 1, 0);
        EXIT_ON_ERR(ret>0);
        printf("proc2 received: %s\n", mb.mtext);
    }
}

int main(int argc, char *argv[])
{
    if (argc > 1 && argv[1][0] == '2')
        test_proc2();
    else {
        test_proc1();
    }
    return 0;
}
