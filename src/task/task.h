#ifndef _TASK_H
#define _TASK_H

#include <functional>
#include <basic/BasicInclude.h>

KURAXII_NAMESPACE_BEGIN

static const INT DEFAULT_PROORITY = 0;

class Task : public Object {
public:
    Task(std::function<void()> &&_func, INT _priority = DEFAULT_PROORITY)
        : _func(std::move(_func)), _priority(_priority)
    {
    }

    // 移动构造
    Task(Task &&other) : _func(std::move(other._func)), _priority(other._priority)
    {
    }

    // 移动赋值运算符
    Task &operator=(Task &&other)
    {
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

    // 由于小顶堆

    // 删除拷贝和复制构造
    NO_ALLOWED_COPY(Task)

private:
    std::function<void()> _func;
    INT _priority;
};

KURAXII_NAMESPACE_END

#endif