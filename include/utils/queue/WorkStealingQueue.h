/***************************
@Author: kuraxii
@Contact: zj.zhu.cn@gmail.com
@File: UWorkStealingQueue.h
@Time: 2024/6/3 10:49 下午
@Desc: 实现了一个包含盗取功能的安全队列
***************************/

#ifndef _WORKSTEALINGQUEUE_H
#define _WORKSTEALINGQUEUE_H

#include <iostream>
#include <deque>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>
#include "QueueObject.h"
#include "../../basic/BasicInclude.h"
#include "../../basic/funcdef.h"

/*
保证任务尽量按插入顺序执行

弹出操作：
正常弹出 pop_front
窃取弹出 pop_back

插入操作： 均使用尾插
*/

KURAXII_NAMESPACE_BEGIN

template <typename T>
class WorkStealingQueue : public QueueObject {
public:
    WorkStealingQueue() = default;
    ~WorkStealingQueue() = default;
    NO_ALLOWED_COPY(WorkStealingQueue)

    // 插入队列
    template <typename U>
    void push(U &&task)
    {
        static_assert(std::is_convertible<U, T>::value, "Task type must be convertible to queue element type");
        while (true) {
            if (_lock.try_lock()) {
                _deque.emplace_back(std::forward<U>(task));
                _lock.unlock();
                break;
            } else {
                std::this_thread::yield();
            }
        }
    }

    // 插入一组信息  对于vector只支持移动
    void push(std::vector<T> &tasks)
    {
        while (true) {
            if (_lock.try_lock()) {
                for (auto &task : tasks) {
                    _deque.emplace_back(std::move<T>(task));
                }
                _lock.unlock();
                break;
            } else {
                std::this_thread::yield();
            }
        }
    }

    // // 尝试插入队列
    template <typename U>
    bool tryPush(U &&task)
    {
        static_assert(std::is_convertible<U, T>::value, "Task type must be convertible to queue element type");
        bool result = false;
        if (_lock.try_lock()) {
            _deque.emplace_back(std::forward<U>(task)); // Perfect forwarding
            _lock.unlock();
            result = true;
        }
        return result;
    }
    // 尝试插入一组信息
    bool tryPush(std::vector<T> &tasks)
    {
        bool result = false;

        if (_lock.try_lock()) {
            for (const auto &task : tasks) {
                _deque.emplace_back(std::move<T>(task));
            }
            tasks.clear();
            _lock.unlock();
            result = false;
        }
        return result;
    }

    // 弹出节点 从头部进行
    bool tryPop(T &task)
    {
        bool result = false;
        if (!_deque.empty() && _lock.try_lock()) {
            if (!_deque.empty()) {
                task = std::move(_deque.front());
                _deque.pop_front();
                result = true;
            }
            _lock.unlock();
        }
        return result;
    }

    // 从头部开始批量获取可执行任务信息
    bool tryPop(std::vector<T> &tasks, INT maxLocalBatchSize)
    {
        bool result = false;
        if (!_deque.empty() && _lock.try_lock()) {
            while (!_deque.empty() && maxLocalBatchSize--) {
                tasks.emplace_back(std::move(_deque.front()));
                _deque.pop_front();
                result = true;
            }
            _lock.unlock();
        }
        return result;
    }

    // 窃取节点
    bool trySteal(T &task)
    {
        bool result = false;
        if (!_deque.empty() && _lock.try_lock()) {
            if (!_deque.empty()) {
                task = std::move(_deque.back());
                _deque.pop_back();
                result = true;
            }
            _lock.unlock();
        }
        return result;
    }
    // 批量窃取节点
    bool trySteal(std::vector<T> &taskArr, INT maxStealBatchSize)
    {
        bool result = false;
        if (!_deque.empty() && _lock.try_lock()) {
            while (!_deque.empty() && maxStealBatchSize--) {
                taskArr.emplace_back(std::move(_deque.front()));
                _deque.pop_front();
                result = true;
            }
            _lock.unlock();
        }
        return result;
    }

    SIZE size()
    {
        std::unique_lock<std::mutex> lock(_lock);
        return _deque.size();
    }

private:
    std::deque<T> _deque;
    std::mutex _lock;
};

KURAXII_NAMESPACE_END

#endif