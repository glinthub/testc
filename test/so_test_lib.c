/*
gcc -fPIC -shared -c so_test_lib.c -DTEST_BASE=100
gcc -fPIC -shared -o libzws.so so_test_lib.o
gcc so_test_main.c -L. -lzws
#if both libzws.so and libzws.a exist, then libzws.so is took because dynamic lib is searched firstly.
*/

int so_f(int i)
{
    return i + TEST_BASE;
}
