
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <features.h>

int fork_test() {
	int a=0, b=0;
	printf("pid[%5d]: a=%d, b=%d, line %d\n", (int)getpid(), a, b, __LINE__);

	fork();
	a = 1;
	printf("pid[%5d]: a=%d, b=%d, line %d\n", (int)getpid(), a, b, __LINE__);

	fork();
	b = 1;
	printf("pid[%5d]: a=%d, b=%d, line %d\n", (int)getpid(), a, b, __LINE__);

	return 0;
}

int main(int argc, char *argv[])
{
    fork_test();
    return 0;
}
