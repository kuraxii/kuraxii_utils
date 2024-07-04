#ifndef _ATOMICQUEUE_H
#define _ATOMICQUEUE_H

#include <queue>
#include <thread>
#include <basic/BasicInclude.h>

#include "QueueObject.h"
KURAXII_NAMESPACE_BEGIN
template <typename T>
class AtomicQueue : public QueueObject {
public:
    AtomicQueue() = default;
    ~AtomicQueue() = default;

    void waitPop(T &value)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _cv.wait(lock, [this] { return !_queue.empty(); });
        value = std::move(_queue.front());
        _queue.pop();
    }

    bool tryPop(T &value)
    {
        std::unique_lock<std::mutex> lock(_mutex, std::try_to_lock);
        if (!lock.owns_lock() || _queue.empty())
            return false;

        value = std::move(_queue.front());
        _queue.pop();
        return true;
    }
    // 弹出多个

    bool tryPop(std::vector<T> &values, int maxPoolBatchSize)
    {
        std::unique_lock<std::mutex> lock(_mutex, std::try_to_lock);
        if (!lock.owns_lock())
            return false;

        while (!_queue.empty() && maxPoolBatchSize-- > 0) {
            values.emplace_back(std::move(_queue.front()));
            _queue.pop();
        }
        return !values.empty();
    }

    void push(T &value)
    {
        while (true) {
            if (_mutex.try_lock()) {
                _queue.emplace(std::forward<T>(value));
                _mutex.unlock();
                _cv.notify_one();
                break;
            } else {
                std::this_thread::yield();
            }
        }
    }

    void push(T &&value)
    {
        while (true) {
            if (_mutex.try_lock()) {
                _queue.emplace(std::forward<T>(value));
                _mutex.unlock();
                _cv.notify_one();
                break;
            } else {
                std::this_thread::yield();
            }
        }
    }

    bool empty()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _queue.empty();
    }

    NO_ALLOWED_COPY(AtomicQueue)

private:
    std::queue<T> _queue;
};

KURAXII_NAMESPACE_END
#endif
// push 使用自旋锁：因为插入操作通常很快，可以通过自旋锁减少线程挂起和唤醒的开销，提高性能。
// pop 使用条件变量：因为需要等待队列中有元素才能进行弹出操作，使用条件变量可以在等待期间节省CPU资源，避免忙等待。