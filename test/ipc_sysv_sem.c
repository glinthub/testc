#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/sem.h>

#include "common.h"

#define IPC_KEY 0123

union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                (Linux-specific) */
};

int test_proc1()
{
    int ret = 0;
    key_t key = IPC_KEY;
    int sems = semget(key, 1, IPC_CREAT|0666);
    EXIT_ON_ERR(sems!=-1);
    printf("sems %d\n", sems);

    unsigned short array[] = {1};
    union semun u;
    u.array = array;
    semctl(sems, 1, SETALL, u);

    struct sembuf sops_wait[1] = {{0, -1, SEM_UNDO}};
    struct sembuf sops_wake[1] = {{0, 1, SEM_UNDO}};

    int i = 0;
    while (1)
    {
        ret = semop(sems, sops_wait, 1);
        EXIT_ON_ERR(ret==0);
        printf("proc1 %d\n", i++);
        sleep(1);
        semop(sems, sops_wake, 1);
    }
}

int test_proc2()
{
    int ret = 0;
    key_t key = IPC_KEY;
    int sems = semget(key, 0, IPC_CREAT | 0600);
    EXIT_ON_ERR(sems!=-1);

    struct sembuf sops_wait[1] = {{0, -1, 0}};
    struct sembuf sops_wake[1] = {{0, 1, 0}};

    int i = 0;
    while (1)
    {
        //ret = semop(sems, sops_wait, 1);
        EXIT_ON_ERR(ret==0);
        printf("proc2 %d\n", i++);
        //ret = semop(sems, sops_wake, 1);
        EXIT_ON_ERR(ret==0);
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
