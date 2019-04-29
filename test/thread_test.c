#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

//GDB related:
//Using "info thread" cmd to show threads
//Using "thread <x>" cmd to select a thread
//Using "scheduler-lock on/off" cmd for forbid/allow other threads running
//
//when threads switch happen, program will receive a SIG41 signal,
//which will by default stop the executing and let operator to do something.
//Using "handle SIG41 noprint" cmd to ignore such signal
//Using "info signal" cmd to view all signal settings

void thread_1(void *arg) {
	int i = 0;
	while (1) {
		printf("T1: 1111111111:%d\n", i++);
		usleep(40000);
	}
}

pthread_mutex_t dcmMainMutex_dummy;

void thread_2(void *arg)
{
    struct timespec ts;
    pthread_mutex_init(&dcmMainMutex_dummy, NULL);
    int i = 0;
    while (1)
    {
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += 2;
        int ret = pthread_mutex_timedlock(&dcmMainMutex_dummy, &ts);
        printf("T2: 22222222222222222222: %d %d\n", ret, i++);
    }
    pthread_mutex_destroy(&dcmMainMutex_dummy);
}

void thread_3(void *arg)
{
    struct timespec ts;
    pthread_mutex_init(&dcmMainMutex_dummy, NULL);
    int i = 0;
    while (1)
    {
        clock_gettime(CLOCK_REALTIME, &ts);
//        ts.tv_sec += 2;
        ts.tv_nsec += 10 * 1000000;
        if (ts.tv_nsec >= 1000000000)
        {
            ts.tv_nsec -= 1000000000;
            ts.tv_sec += 1;
        }
        int ret = pthread_mutex_timedlock(&dcmMainMutex_dummy, &ts);
        printf("T2: 22222222222222222222: %d %d\n", ret, i++);
    }
    pthread_mutex_destroy(&dcmMainMutex_dummy);
}

int main(int argc, char* argv[]) {
	pthread_t tid;
	int id = 1;
	pthread_create(&tid, NULL, (void *)thread_1, &id);
	pthread_create(&tid, NULL, (void *)thread_2, &id);

	int i = 0;
	while (1) {
		printf("main:%d\n", i++);
		usleep(40000);
	}
	return 0;
}
