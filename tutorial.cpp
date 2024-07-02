#include <iostream>
#include <iterator>
#include <utility>
#include <thread>
#include <future>
#include <functional>
#include <basic/BasicInclude.h>
#include <utils/task/TaskInclude.h>
#include <utils/queue/QueueInclude.h>
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

    myClass &operator=(myClass &&other) = default;

    int i;
    std::string str;
};

int asyncTask()
{
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "Task completed" << std::endl;
    return -2;
}

void func(int a, int b)
{
    int i = a + b;
    std::cout << "func be called" << std::endl;
}

int main()
{

    myClass b{std::move(myClass())};
    b.str = "abcdef";
    myClass a = std::move(b);

    std::cout << b.str << std::endl;

    std::function<void()> f = std::bind(func, 1, 2);
    // KURAXII::TaskGroup group(f);
    KURAXII::Task t(std::bind(func, 1, 2));
    t();
    KURAXII::TaskGroup tg(std::bind(func, 1, 2));
    // std::future<int> res = std::async(asyncTask);
    // res.get();
    unsigned int in = std::thread::hardware_concurrency();

    std::cout << in << std::endl;
    return 0;
}