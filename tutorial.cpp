#include <iostream>
#include <iterator>
#include <utility>
#include <thread>
#include <future>
#include <functional>
#include <basic/BasicInclude.h>
#include <utils/task/TaskInclude.h>
#include <utils/queue/QueueInclude.h>
#include <utils/thread/ThreadObject.h>
#include <unistd.h>
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
    std::cout << "func be called: " << a + b << std::endl;
}

int main()
{
#if 0
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
#else
    KURAXII::ThreadObject thread;
    KURAXII::Task t1 = {std::move(std::bind(func, 1, 2))};
    KURAXII::Task t2 = {std::move(std::bind(func, 5, 6))};
    thread.pushTask(std::move(t1));
    thread.pushTask(std::move(t2));
    thread.init();
    sleep(1);

#endif
    return 0;
}