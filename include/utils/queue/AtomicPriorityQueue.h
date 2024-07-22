#ifndef _ATMOICPRIORITYQUEUE_H
#define _ATMOICPRIORITYQUEUE_H
#include <queue>
#include <thread>
#include "../../basic/BasicInclude.h"
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
        value = std::move(const_cast<T &>(_priority_queue.top()));
        _priority_queue.pop();
    }

    bool tryPop(T &value)
    {
        std::unique_lock<std::mutex> lock(_mutex, std::try_to_lock);
        if (!lock.owns_lock() || _priority_queue.empty()) {
            return false;
        }
        value = std::move(const_cast<T &>(_priority_queue.top()));
        _priority_queue.pop();
        return true;
    }
    // 弹出多个

    bool tryPop(std::vector<T> &values, int maxPoolBatchSize)
    {

        std::unique_lock<std::mutex> lock(_mutex, std::try_to_lock);
        if (!lock.owns_lock())
            return false;

        while (!_priority_queue.empty() && maxPoolBatchSize-- > 0) {
            /*
               const_reference top() const
               top的返回值为 const ref 使用强转
               https://stackoverflow.com/questions/20149471/move-out-element-of-std-priority-queue-in-c11
               */
            values.emplace_back(std::move(const_cast<T &>(_priority_queue.top())));
            _priority_queue.pop();
        }

        return !values.empty();
    }
    template <typename U>
    void push(U &&value)
    {
        static_assert(std::is_convertible<U, T>::value, "Task type must be convertible to queue element type");
        std::unique_lock<std::mutex> lock(_mutex, std::defer_lock);
        while (!lock.try_lock()) {
            std::this_thread::yield();
        }
        _priority_queue.emplace(std::forward<U>(value));
        _cv.notify_one();
    }

    bool empty()
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