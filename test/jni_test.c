/*
javah JniTest
Javac JniTest.java
gcc -fPIC -shared -c jni_test.c -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux
gcc -fPIC -shared -o libjnitest.so jni_test.o
sudo cp libjnitest.so /usr/lib/
java JniTest
*/
#include "JniTest.h"

JNIEXPORT jint JNICALL Java_JniTest_c_1func
  (JNIEnv *e, jobject o, jint i)
{
    printf("100+%d=%d\n", i, i+100);
    return i+100;
}

