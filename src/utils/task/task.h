#ifndef _TASK_H
#define _TASK_H
#include <cassert>
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
        : _func(std::move(_func)), _priority(_priority), valid(true)
    {
    }
    ~Task()
    {
    }

    // 移动构造
    Task(Task &&other) noexcept : _func(std::move(other._func)), _priority(other._priority), valid(true)
    {
        // std::cout << "Task moved from " << &other << " to " << this << std::endl;
        assert(other.valid == true);

        other.valid = false;
        // valid = true;
    }

    // 移动赋值运算符
    Task &operator=(Task &&other) noexcept
    {
        // std::cout << "Task assigned from " << &other << " to " << this << std::endl;
        assert(other.valid == true);
        if (&other != this) {

            _func = std::move(other._func);
            _priority = other._priority;
            other.valid = false;
            valid = true;
        }
        return *this;
    }

    // call
    void operator()()
    {
        assert(valid == true);
        _func();
    }
    bool operator<(const Task &b) const
    {
        assert(valid == true);
        return this->_priority < b._priority;
    }

    // clang-format off
    inline UINT getPriority() const { return _priority; }
    // clang-format on

    // 删除拷贝和复制构造
    NO_ALLOWED_COPY(Task)

private:
    std::function<void()> _func;
    INT _priority;
    bool valid;
};

KURAXII_NAMESPACE_END

#endif