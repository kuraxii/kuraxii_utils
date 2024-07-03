#ifndef _TASK_H
#define _TASK_H

#include <iostream>
#include <functional>
#include <basic/BasicInclude.h>

/*
    所有的task只可移动 不可复制
*/

KURAXII_NAMESPACE_BEGIN

static const INT DEFAULT_PROORITY = 0;

class Task : public Object {
public:
    Task(KURAXII_MOVE_FUNCTION_REF _func, INT _priority = DEFAULT_PROORITY)
        : _func(std::move(_func)), _priority(_priority)
    {
        std::cout << "Task move func structor" << std::endl;
    }

    // 移动构造
    Task(Task &&other) : _func(std::move(other._func)), _priority(other._priority)
    {
        std::cout << "Task move structor " << &_func << std::endl;
    }

    // 移动赋值运算符
    Task &operator=(Task &&other)
    {
        std::cout << "Task move=  structor" << std::endl;

        if (&other != this)
        {
            _func = std::move(other._func);
            _priority = other._priority;
            other._priority = 0;
        }
        return *this;
    }

    // call
    void operator()()
    {
        _func();
    }

    // 用于小顶堆

    // 删除拷贝和复制构造
    NO_ALLOWED_COPY(Task)

private:
    std::function<void()> _func;
    INT _priority;
};

KURAXII_NAMESPACE_END

#endif