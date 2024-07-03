#ifndef _THREADOBJECT_H
#define _THREADOBJECT_H

#include <basic/BasicInclude.h>
#include <utils/queue/QueueInclude.h>
#include <utils/task/TaskInclude.h>
#include "ThreadConfig.h"
#include <mutex>
KURAXII_NAMESPACE_BEGIN

class ThreadObject : Object {

public:
    ThreadObject() = default;
    ~ThreadObject()
    {
        reset();
    }
    virtual void init()
    {
        _thread = std::move(std::thread{std::bind(&ThreadObject::loopThread, this)});
        _is_init = true;
        _done = false;
    }

    virtual void processTask()
    {
        BOOL result = false;
        static thread_local std::vector<Task> tasks;
        if (_config.getBatchTaskEnable()) {
            result = popTask(tasks, _config.getMaxTaskBatchSize());
        } else {
            result = popTask(tasks, 1);
        }
        if (result) {
            runTask(tasks);
            tasks.clear();
        } else {
            fatWait();
        }
    }

    virtual void loopThread()
    {
        while (!_done) {
            processTask();
        }
    }

    void runTask(std::vector<Task> &tasks)
    {
        for (auto &it : tasks) {
            it();
        }
    }

    virtual void fatWait()
    {
        _cur_empty_epoch++;
        std::this_thread::yield();
        if (_cur_empty_epoch > _config.getPrimaryThreadBusyEpoch()) {
            std::unique_lock<std::mutex> lock(_mutex);
            _cv.wait_for(lock, std::chrono::milliseconds(_config.getPrimaryThreadEmptyInterval()));
            _cur_empty_epoch = 0;
        }
    }

    virtual bool popTask(Task &task)
    {
        return _primary_task_queue.tryPop(task) || _second_task_queue.tryPop(task);
    }
    virtual bool popTask(std::vector<Task> &tasks, UINT taskNum)
    {
        BOOL result = _primary_task_queue.tryPop(tasks, taskNum);
        INT size = taskNum - tasks.size();
        if (size > 0) {
            result |= _second_task_queue.tryPop(tasks, size);
        }
        return result;
    }

    virtual void pushTask(Task &&task)
    {
        while (!(_second_task_queue.tryPush(std::move(task)) || !_primary_task_queue.tryPush(std::move(task)))) {
            std::this_thread::yield();
        }
        _cv.notify_one();
    }
    virtual void pushTask(Task &task)
    {

        while (!(_second_task_queue.tryPush(task) || !_primary_task_queue.tryPush(task))) {
            std::this_thread::yield();
        }
        _cv.notify_one();
    }

    // 显示销毁线程
    /* 结束线程循环 */
    virtual void reset()
    {
        _done = true;
        if (_thread.joinable()) {
            _thread.join();
        }
    };
    virtual void destory()
    {
        reset();
    }

protected:
    BOOL _done = true;          // 是否线程是否借结束
    BOOL _is_init = 0;          // 初始化状态
    BOOL _is_running = 0;       // 是否正在执行
    INT _type = 0;              // 线程类型 主 or 副
    UINT64 _total_task_num = 0; // 统计处理的线程数
    ThreadConfig _config;
    INT _cur_empty_epoch = 0; // 当前轮转次数
    WorkStealingQueue<Task> _primary_task_queue;
    WorkStealingQueue<Task> _second_task_queue;
    std::condition_variable _cv; // 条件变量 唤醒线程
    std::mutex _mutex;
    std::thread _thread;
    // AtomicQueue<Task>& _pool_task_queue;
    // AtomicPriorityQueue<Task>& _pool_priority_task_queue;
};

KURAXII_NAMESPACE_END

#endif