#include <iostream>
#include <memory>

using namespace std;
#define HERE cout << __LINE__ << endl

class A {
public:
    int i;
    A() {cout << "constructor " << this << endl;}
    ~A() {cout << "destructor " << this << endl;}

    A(A& a) {cout << "copy constructor " << this << endl;}
    A(const A& a) {cout << "const copy constructor " << this << endl;}
};

void f1(A a) {
    cout << &a << endl;
    cout << a.i << endl;
}

void test1()
{
    cout << "------------ test 1 -------------" << endl;
    int i = 0;
    int j = i;
    int &k = i;
    //    int &m = 3;   //error!
    const int &n = 3;   //pass! const lvalue could be assigned with a rvalue. while non-const lvalue could not.
    int &p = k;

    k = 100;
    cout << i << endl;
    cout << &i << endl;
    cout << &j << endl;
    cout << &k << endl;
    cout << &n << endl;
    cout << &p << endl;

    HERE;
    A a = A();
    HERE;
    a = A();
    HERE;
    f1(a);
    HERE;
}

class A2 {
public:
    virtual void f() {cout << "A2.f()" << endl;}
};

class B2 : public A2 {
public:
    virtual void f() {cout << "B2.f()" << endl;}
};

class C2 : public B2 {
public:
    virtual void f() {cout << "C2.f()" << endl;}
};

void test2()
{
    cout << "------------ test 2 -------------" << endl;
    A2 *a = new C2();
    a->f();
}

int main(int argc, char *argv[])
{
    test1();
    test2();
    return 0;
}


