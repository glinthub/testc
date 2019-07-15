#include <iostream>
#include <memory>
#include <unistd.h>

using namespace std;

void test1()
{
    cout << "------------ test 1: lambda ---------------- " << endl;
    typedef int (*iif_t)(int, int);
    iif_t iif;
    int i = 10, j = 20;
      auto f = [=](int x, int y) -> int {return x+y;};  // '-> int' is optional
    //auto g = [=](int x, int y) {i++; return "asdf";}; //error: i is read-only
      auto g = [&](int x, int y) {i++; return "asdf";}; //pass
    //auto h = [&, i](int x, int y) {i++; return "asdf";}; //error: i is read-only, other is rw
      auto h = [&, i](int x, int y) {j++; return "asdf";}; //pass
      auto m = [&i, &j](int x, int y) {j++; return "asdf";}; //pass i,j is rw
    //iif_t f = [=](int x, int y) -> int {return x+y;}; //error!
    //iif = f; //error!
    cout << f(1,2) << endl;
    cout << g(1,2) << endl;
}

auto f2(int x, int y) -> int           { return x+y; }
auto g2(int x, int y)                  { return x+y; } //not allowed in c++ 11, but allowed in c++ 14  (g++ -std=c++11)
auto h2(int x, int y) -> decltype(x+y) { return x+y; } //pass
void test2()
{
    cout << "------------ test 2: auto, decltype ---------------- " << endl;
    cout << f2(3,4) << endl;
    cout << g2(3,5) << endl;
    cout << h2(3,5) << endl;
}

class A {
public:
    int m_i;
    //A() { cout << "construct A() " << endl; }
    A(int i) : m_i(i) { cout << "construct A(int) " << m_i << " " << this << endl; }
    ~A() { cout << "destroy A " << "(" << this << ")" << endl; }
};

class B : public A {
    int m_bi;
public:
    //B() { cout << "construct B() " << endl; }
    //B(int i) :                  {          cout << "construct B " << m_i << endl; } //error: B dont call A's constructor explicitly, then A's default constructor is used, but A has no default constructor.
    //B(int i) : A::A(i), m_i(i)  {          cout << "construct B " << m_i << endl; } //error: member inherited from parent could not be initialized in initial list
    //B(int i) : A::A(i)          { m_i = i; cout << "construct B " << m_i << endl; } //pass : but they can be initialized in function body
    B(int i) : A::A(i), m_bi(i) { cout << "construct B " << m_i << " " << this << endl; }
    ~B() { cout << "destroy B " << "(" << this << ")" << endl; }
};

void test3()
{
    cout << "------------ test 3: share_ptr --------------- " << endl;
    A* p = new A(10);
    shared_ptr <A> sp1(p);
    shared_ptr <A> sp2(p); //another reference to p, will cause p to be deleted twice
    //shared_ptr <A> sp3(sp1); //same (as sp1) reference to p 
    A* p1;
    p1 = sp1.get(); cout << p1->m_i << endl;
    p1 = sp2.get(); cout << p1->m_i << endl;
    //p1 = sp3.get(); cout << p1->m_i << endl;
    cout << &sp1 << endl;
    cout << &sp2 << endl;
    //cout << &sp3 << endl;

    cout << "------------ test 3.1 ---------------- " << endl;
    shared_ptr<A> spa(new A(123));
    shared_ptr<B> spb(new B(456));
    spa = dynamic_pointer_cast<A>(spb);
    cout << "after dyn cast" << endl;
    spa.reset();
    spb.reset();
    cout << "after reset" << endl;
    
}

class C4 {
public:
    int m_i;
    C4() {cout << "construct " << this << endl; }
    C4 &operator= (const C4 & o)        
    {
        cout << "operator= &" << endl;
        return *this;
    }
#if 1
    C4 &operator= (const C4 && o)
    {
        cout << "operator= &&" << endl;
        return *this;
    }
#endif

#if 1
    C4(const C4& o)
    {
        cout << "constructor C4(const C4 &)" << endl; 
    }
    C4(const C4&& o)
    {
        cout << "constructor C4(const C4 &&)" << endl; 
    }
#endif
    ~C4() {cout << "destroy " << this << endl;}
        
};
void test4()
{
    cout << "------------ test 4: rvalue ref --------------- " << endl;
    C4 a;
    a = C4();       //operator= &&, or operator= &
    C4 b = C4();    //default  C4(const C4 &), which will not be there is move constructor/operator= is defined.
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
