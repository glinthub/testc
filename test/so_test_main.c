#include <stdio.h>
#include <stdlib.h>

extern int so_f(int);

int main(int argc, char *argv[])
{
    printf("%d\n", so_f(1));
    return 0;
}

