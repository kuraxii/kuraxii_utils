#include "src/basic/BasicInclude.h"
#include <iostream>
#include <iterator>
#include <utility>
#include <thread>
#include <future>
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

    myClass& operator=(myClass&& other) = default;

    int i;
    std::string str;


};

int asyncTask()
{
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "Task completed" << std::endl;
    return -2;
}

int main()
{

    myClass b{std::move(myClass())};
    b.str = "abcdef";
    myClass a = std::move(b);

    std::cout << b.str << std::endl; 

    std::future<int> res = std::async(asyncTask);

    return 0;
}