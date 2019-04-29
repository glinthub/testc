#include <stdio.h>

int main(int argc, char *argv[])
{
    const int ci = 0;
    int const ic = 0;
    //ci ++; error
    //ic ++; error
    int *pi;
    pi = &ci;
    (*pi) ++;
    pi = &ic;
    (*pi) ++;
    pi++;

    const int* p2;
    p2++;
    //(*p2) ++; error

    int * const * p3;
    p3++;
    //(*p3)++; error
    (**p3)++;
    printf("ci = %d, ic = %d\n", ci, ic);
    return 0;
}
