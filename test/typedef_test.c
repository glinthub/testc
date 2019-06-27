#include <stdio.h>

typedef int int_8[8];
typedef int *pint_8[8];
typedef int (*func)(int, char);

int f1(int i, char c) {
    printf("f1\n");
    return 0;
}

func f;
void reg_func(int (*func)(int, char))
{
    f = func;
}

int main(int argc, char *argv[]) {
    printf("%lu\n", sizeof(int_8));//32
    printf("%lu\n", sizeof(pint_8));//64

    int_8 i = {5,4,3,2,1};
    printf("%d\n", i[2]);

    int_8* p = &i;
    printf("%p, %p\n", p, p+1);//x, x+32

    int a[2];
    printf("%p, %p, %p\n", a, &a, (&a)+1);  //x, x, x+8

    reg_func(f1);
    f(1, '1');
    return 0;
}
