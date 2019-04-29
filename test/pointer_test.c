#include <stdio.h>

typedef int int_8[8]; //sizeof(int_8): 32
typedef int *pint_8[8]; //sizeof(int_8): 32

int main(int argc, char *argv[]) {
    printf("%d\n", sizeof(int_8));

    int_8 i = {5,4,3,2,1};
    printf("%d\n", i[2]);

    int_8* p = &i;
    printf("%p, %p\n", p, p+1);

    int a[2];
    printf("%p, %p, %p\n", a, &a, (&a)+1);

    printf("%d\n", sizeof(pint_8));

    return 0;
}
