#include <iostream>
#include <memory>

using namespace std;

class A {
public:
    int m_i;
    A() { cout << "construct A() " << endl; }
    A(int i) : m_i(i) { cout << "construct A(int) " << m_i << endl; }
    ~A() { cout << "destroy A " << m_i << "(" << this << ")" << endl; }
};

class B : public A {
    int m_i;
public:
    B(int i, int j=0) : m_i(i) { cout << "construct B " << m_i << endl; }
    ~B() { cout << "destroy B " << m_i << "(" << this << ")" << endl; }
};

void test1()
{
    typedef int (*iif_t)(int, int);
    iif_t iif;
    auto f = [=](int x, int y) -> int {return x+y;};
    //iif_t f = [=](int x, int y) -> int {return x+y;}; //error!
    //iif = f; //error!
    cout << f(1,2) << endl;
}

auto add (int x, int y) -> int { return x+y; }
auto add1 (int x, int y) { return x+y; }
void test2()
{
    cout << add(3,4) << endl;
}

void test3()
{
    A* p = new A(10);
    shared_ptr <A> sp1(p);
    shared_ptr <A> sp2(p); //another reference to p
    //shared_ptr <A> sp3(sp1); //same (as sp1) reference to p 
    A* p1;
    p1 = sp1.get(); cout << p1->m_i << endl;
    p1 = sp2.get(); cout << p1->m_i << endl;
    //p1 = sp3.get(); cout << p1->m_i << endl;
    cout << &sp1 << endl;
    cout << &sp2 << endl;
    //cout << &sp3 << endl;

    shared_ptr<A> spa(new A(123));
    shared_ptr<B> spb(new B(123));
    spa = dynamic_pointer_cast<A>(spb);
    cout << "after dyn cast" << endl;
    
}

void test4()
{
    int arr[] = {1,2,3,4,5};
    for(int& e : arr) 
    {
      e = e*e;
    }
    for (auto e : arr)
    {
        cout << e << endl;
    }
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    test1();
    test2();
    test3();
    test4();
    return 0;
}
