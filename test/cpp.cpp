#include <iostream>
#include <memory>

using namespace std;
#define HERE cout << __LINE__ << endl

class A {
public:
    A() {cout << "constructor" << this << endl;}
    ~A() {cout << "destructor" << this << endl;}

    A(A& a) {cout << "copy constructor" << this << endl;}
    A(const A& a) {cout << "const copy constructor" << this << endl;}
};

int main()
{
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
    return 0;
}

