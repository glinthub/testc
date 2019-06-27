#include <stdio.h>

int main(int argc, char *argv[])
{
    const int ci = 1;
    int const ic = 2;
    //ci ++; error
    //ic ++; error

    int *pi;
    pi = &ci;
    (*pi) ++;
    pi = &ic;
    (*pi) ++;
    pi++;
    printf("ci=%d, ic=%d\n", ci, ic);

    int * const p1 = &ci;
    //p1++; //error

    const int* p2;
    p2++;
    //(*p2) ++; error

    int * const * p3;
    p3++;
    //(*p3)++; error
    (**p3)++;

    const int * const p4 = &ic;
    //p4++;     //error
    //(*p4)++;  //error

    return 0;
}
