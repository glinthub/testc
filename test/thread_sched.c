#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

char *policyStr[3] = {
    "SCHED_OTHER",
    "SCHED_FIFO",
    "SCHED_RR"
    };

//int policies[] = {SCHED_OTHER, SCHED_FIFO, SCHED_RR};
int policies[] = {SCHED_RR, SCHED_RR, SCHED_RR};
//int policies[] = {SCHED_FIFO, SCHED_FIFO, SCHED_FIFO};
int priorities[] = {10,40,70};

#define CHECK_RESULT(expect, res, action) \
    do { if (res != expect) { \
            printf("%s:%d: expect %d, but result %d (%s)\n", __FILE__, __LINE__, expect, res, strerror(errno)); \
    } } while(0)

void Thread(void *arg)
{
    int id = *(int*)arg;
    int ret;

    printf("thread %d begin...\n", id);
    struct sched_param param;
    param.sched_priority = priorities[id];
    ret = pthread_setschedparam(pthread_self(), policies[id], &param);
    CHECK_RESULT(0, ret, return 0);

    sleep(1);
    int i,j;
    int policy;
    ret = pthread_getschedparam(pthread_self(),&policy,&param);
    CHECK_RESULT(0, ret, return 0);
    printf("thread %d %s, prio %d\n", id, policyStr[policy], param.sched_priority);

    for(i=1;i<10;i++)
    {
        for(j=1;j<5000000;j++)
        {
        }
        printf("thread %d\n", id);
    }
    printf("Pthread %d exit\n", id);
}

int main()
{
    int ret;

    int i;
    i = getuid();
    if(i==0)
        printf("The current user is root\n");
    else
        printf("The current user is not root\n");

    pthread_t tid0,tid1,tid2;
    struct sched_param param;

    pthread_attr_t attr0,attr1,attr2;
  
    ret = pthread_attr_init(&attr0);
    CHECK_RESULT(0, ret, return 0);
    ret = pthread_attr_init(&attr1);
    CHECK_RESULT(0, ret, return 0);
    ret = pthread_attr_init(&attr2);
    CHECK_RESULT(0, ret, return 0);

/*
    param.sched_priority = 51;
    ret = pthread_attr_setschedpolicy(&attr1,SCHED_RR);
    CHECK_RESULT(0, ret, return 0);
    ret = pthread_attr_setschedparam(&attr1,&param);
    CHECK_RESULT(0, ret, return 0);
    ret = pthread_attr_setinheritsched(&attr1,PTHREAD_EXPLICIT_SCHED);//要使优先级其作用必须要有这句话
    CHECK_RESULT(0, ret, return 0);

    param.sched_priority = 21;
    ret = pthread_attr_setschedpolicy(&attr2,SCHED_RR);
    CHECK_RESULT(0, ret, return 0);
    ret = pthread_attr_setschedparam(&attr2,&param);
    CHECK_RESULT(0, ret, return 0);
    ret = pthread_attr_setinheritsched(&attr2,PTHREAD_EXPLICIT_SCHED);
    CHECK_RESULT(0, ret, return 0);
*/

    int i0,i1,i2;
    i0=0; i1=1; i2=2;

    ret = pthread_create(&tid2,&attr2,(void *)Thread,&i2);
    CHECK_RESULT(0, ret, return 0);
    ret = pthread_create(&tid1,&attr1,(void *)Thread,&i1);
    CHECK_RESULT(0, ret, return 0);
    ret = pthread_create(&tid0,&attr0,(void *)Thread,&i0);
    CHECK_RESULT(0, ret, return 0);

    ret = pthread_join(tid2,NULL);
    CHECK_RESULT(0, ret, return 0);
    ret = pthread_join(tid1,NULL);
    CHECK_RESULT(0, ret, return 0);
    ret = pthread_join(tid0,NULL);
    CHECK_RESULT(0, ret, return 0);

    ret = pthread_attr_destroy(&attr2);
    CHECK_RESULT(0, ret, return 0);
    ret = pthread_attr_destroy(&attr1);
    CHECK_RESULT(0, ret, return 0);
    ret = pthread_attr_destroy(&attr0);
    CHECK_RESULT(0, ret, return 0);

    return 0;
}
