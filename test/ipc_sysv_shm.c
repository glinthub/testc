#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <fcntl.h>

#include "common.h"

char *ipc_path = "/tmp/sysv_ipc_test";
int test_proc1()
{
    int ret = 0;
    key_t key = ftok(ipc_path, 0x3);
    //zws: if permission not specified, only root can attach the shared memory
    int shm_id = shmget(key, 123, IPC_CREAT|0666);
    EXIT_ON_ERR(shm_id!=-1);

    void *m = shmat(shm_id, NULL, 0);
    EXIT_ON_ERR(m != (void*)-1);

    char *buf = (char *)m;
    int i=0;
    while(1)
    {
        sprintf(buf, "seq %d\n", i++);
        printf("proc1: %s\n", buf);
        sleep(1);
    }
}

int test_proc2()
{
    int ret = 0;
    key_t key = ftok(ipc_path, 0x3);
    int shm_id = shmget(key, 123, 0);
    EXIT_ON_ERR(shm_id!=-1);

    char *buf = (char *)shmat(shm_id, NULL, SHM_RDONLY);
    EXIT_ON_ERR(buf != (char *)-1);

    while(1)
    {
        printf("proc2: %s\n", buf);
        sleep(3);
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
