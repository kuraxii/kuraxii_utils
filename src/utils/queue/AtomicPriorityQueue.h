#ifndef _ATMOICPRIORITYQUEUE_H
#define _ATMOICPRIORITYQUEUE_H
#include <queue>
#include <thread>
#include <basic/BasicInclude.h>
#include "QueueObject.h"
KURAXII_NAMESPACE_BEGIN

template <typename T>
class AtomicPriorityQueue : public QueueObject {

public:
    AtomicPriorityQueue() = default;
    ~AtomicPriorityQueue() = default;
    void waitPop(T &value)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _cv.wait(lock, [this] { return !_priority_queue.empty(); });
        value = std::move(_priority_queue.top());
        _priority_queue.pop();
    }

    BOOL tryPop(T &value)
    {
        std::unique_lock<std::mutex> lock(_mutex, std::try_to_lock);
        if (!lock.owns_lock() || _priority_queue.empty())
        {
            return false;
        }
        value = std::move(_priority_queue.top());
        _priority_queue.pop();
        return true;
    }
    // 弹出多个

    BOOL tryPop(std::vector<T> &values, int maxPoolBatchSize)
    {
        std::unique_lock<std::mutex> lock(_mutex, std::try_to_lock);
        if (!lock.owns_lock())
            return false;

        while (!_priority_queue.empty() && maxPoolBatchSize-- > 0)
        {
            values.emplace_back(std::move(_priority_queue.top()));
            _priority_queue.pop();
        }
        return !values.empty();
    }

    void push(T &&value)
    {
        while (true)
        {
            if (_mutex.try_lock())
            {
                _priority_queue.emplace(std::forward<T>(value));
                _mutex.unlock();
                _cv.notify_one();
                break;
            }
            else
            {
                std::this_thread::yield();
            }
        }
    }

    BOOL empty()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _priority_queue.empty();
    }

private:
    // 默认大顶堆
    std::priority_queue<T> _priority_queue;
};

KURAXII_NAMESPACE_END

#endif