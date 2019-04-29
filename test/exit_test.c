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
		printf("1111\n");
		sleep(1);
        _exit(1);
	}
}

void thread_2(void *arg) {
	int i = 0;
	while (1) {
		printf("2222\n");
		sleep(1);
	}
}

int main(int argc, char* argv[]) {
	pthread_t tid;
	int id = 1;
	pthread_create(&tid, NULL, (void *)thread_1, &id);
	pthread_create(&tid, NULL, (void *)thread_2, &id);

	int i = 0;
	while (1) {
		printf("main:%d\n", i++);
		sleep(4);
	}
	return 0;
}
