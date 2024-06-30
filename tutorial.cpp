#include "src/basic/BasicInclude.h"
#include <iostream>
#include <iterator>
#include <utility>


class myClass {
public:
    myClass() = default;
    myClass(myClass &other)
    {
        std::cout << "T& i" << std::endl;
    }
    myClass(myClass &&i)
    {
        std::cout << "T&& i" << std::endl;
    }

private:
    int i;
};

int main()
{
    
    
    myClass b{std::move(myClass())};
    myClass a{myClass()};
    
    return 0;
}