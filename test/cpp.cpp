#include <iostream>
#include <memory>

using namespace std;

class A
{
public:
    A()
    {
        cout << "A()" << endl;
    }
    A(const A&)
    {
        cout << "A(const A&)" << endl;
    }


};

A Get()
{
    cout << "Get begin" << endl;
    A tmp;
    cout << "Get end" << endl;
    return tmp;
}

int main()
{
    A v = Get();

    return 0;
}

