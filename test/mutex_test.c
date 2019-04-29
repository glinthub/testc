#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
void thread_p(void *arg)
{
    int id = (int)arg;
    while (1)
    {
        sleep(1);
        int ret = pthread_mutex_unlock(&m);
        printf("task %d unlock result %d\n", id, ret);
        if (ret != 0)
            printf("%s\n", strerror(errno));
    }
}

void thread_c(void *arg)
{
    int id = (int)arg;
    while (1)
    {
        int ret = pthread_mutex_lock(&m);
        printf("task %d lock result %d\n", id, ret);
        if (ret != 0)
            printf("%s\n", strerror(errno));
    }
}
    
void thread_c1(void *arg)
{
    int id = (int)arg;
    struct timespec ts;
    while (1)
    {
        clock_gettime(CLOCK_REALTIME, &ts);
        //printf("current time %d.%d\n", ts.tv_sec, ts.tv_nsec);
        //ts.tv_sec += 1;
        ts.tv_nsec += 50 * 1000000;
        if (ts.tv_nsec >= 1000000000)
        {
            ts.tv_nsec -= 1000000000;
            ts.tv_sec += 1;
        }
        int ret = pthread_mutex_timedlock(&m, &ts);
        printf("task %d timedlock result %d\n", id, ret);
    }
}

int main(int argc, char **argv)
{
    pthread_mutexattr_t a;
    int ret = pthread_mutexattr_init(&a);
    printf("attrinit result %d\n", ret);
    //pthread_mutexattr_settype(&a, PTHREAD_MUTEX_RECURSIVE);
    ret = pthread_mutex_init(&m, &a);
    printf("init result %d\n", ret);
    pthread_t tid;
    int i = 0;
    ret = pthread_create(&tid, NULL, (void *)thread_c, (void*)i);
    printf("create result %d\n", ret);
    i++;
    ret = pthread_create(&tid, NULL, (void *)thread_c, (void*)i);
    printf("create result %d\n", ret);
    i++;
    ret = pthread_create(&tid, NULL, (void *)thread_p, (void*)i);
    printf("create result %d\n", ret);
    i++;
    pthread_join(tid, NULL);
    return 0;
}

