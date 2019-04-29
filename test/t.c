#include<stdio.h>

#include<sys/ipc.h>

#include<sys/shm.h>

#include<stdlib.h>

#include<sys/stat.h>

#include<sys/sem.h>





#define SEM_KEY 6666



union semun {

    int setval;

    struct semid_ds *buf;

    unsigned short *array;

};





int main(int argc ,char *argv[])

{



    int  shmid;

    float   *addr;

    float h,w;

    shmid= shmget(ftok(".",1000),getpagesize(),IPC_CREAT  | 0666);//创建一个共享内存，权限为0666

    printf("shmid=%d\n",shmid);

    if(shmid==-1)

    {

       perror("shmget error:");

       exit(EXIT_FAILURE);

    }

    addr=shmat(shmid,0,0);//获取共享内存的起始地址，且为可读可写

    if(-1==*addr)

    {

       perror("shmat error:");

       exit(EXIT_FAILURE);

    }



    int semid;

    int ret;

    semid = semget(SEM_KEY, 2, IPC_CREAT | 0600);//创建2个信号量

    if (-1 == semid)

    {

       perror("semget");

       exit(EXIT_FAILURE);

    }

    printf("semid = %d\n", semid);

 //   初始化2个信号量

    union semun sem_args;

    unsigned short array[2]={1,1};

    sem_args.array = array;

    ret = semctl(semid, 1, SETALL, sem_args);//SETALL代表设置信号集中所有的信号量的值。1,代表2个，sem_args是具体初始化的值放在共用体中。

    if (-1 == ret)

    {

       perror("semctl");

       exit(EXIT_FAILURE);

    }

//对资源的使用处理操作

    struct sembuf sem_opt_wait1[1] = {0, -1, SEM_UNDO};

    struct sembuf sem_opt_wakeup1[1] = {0, 1, SEM_UNDO};

    struct sembuf sem_opt_wait2[1] = {1, -1, SEM_UNDO};

    struct sembuf sem_opt_wakeup2[1] = {1, 1, SEM_UNDO};



    while(1)

    {

       semop(semid, sem_opt_wait2, 1);//获取进程2的资源，让进程2等待

       printf(" enter you height(CM) and height(KG):\n");

       scanf("%f%f",addr,addr+1);

       semop(semid, sem_opt_wakeup1, 1);//唤醒进程1，即释放资源的使用权

       if(*addr==1||*(addr+1)==1 ) break;//如果输入身高是1或体重是1就退出

    }



    return 0;

}

