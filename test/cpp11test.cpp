#include <iostream>
#include <memory>

using namespace std;

class A {
    int m_i;
public:
    A(int i) : m_i(i) {}
    ~A() { cout << "destroy A " << m_i << endl; }
};

void test1()
{
    typedef int (*iif_t)(int, int);
    iif_t iif;
    auto f = [=](int x, int y) -> int {return x+y;};
    //iif = f; //error!
    cout << f(1,2) << endl;
}

auto add (int x, int y) -> int { return x+y; }
void test2()
{
    cout << add(3,4) << endl;
}

void test3()
{
    A* p = new A(10);
    shared_ptr <A> sp1(p);
    shared_ptr <A> sp2(p);
    shared_ptr <A> sp3(sp1);
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    test3();
    return 0;
}
