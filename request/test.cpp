#include <iostream>
#include <string>
#include <fstream>

class test
{
private:
    /* data */
public:
    test(/* args */);
    void func();
    ~test();
};

test::test(/* args */)
{
}
void test::func()
{
    static int n;
    std::cout << n++;
}

test::~test()
{
}


int main()
{
    test obj1;
    obj1.func();
    obj1.func();
    test obj2;
    obj2.func();
}
