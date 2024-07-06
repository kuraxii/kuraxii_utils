#ifndef _THREADOBJECT_H
#define _THREADOBJECT_H

#include <basic/BasicInclude.h>
#include <utils/queue/WorkStealingQueue.h>
#include <utils/task/task.h>
#include <utils/task/taskgroup.h>
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
        bool result = false;
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

    virtual bool popTask(std::vector<Task> &tasks, UINT taskNum)
    {
        bool result = _primary_task_queue.tryPop(tasks, taskNum);
        INT size = taskNum - tasks.size();
        if (size > 0) {
            result |= _normal_task_queue.tryPop(tasks, size);
        }
        return result;
    }

    virtual void pushTask(Task &&task)
    {
        while (!(_normal_task_queue.tryPush(std::move(task)) || !_primary_task_queue.tryPush(std::move(task)))) {
            std::this_thread::yield();
        }
        _cv.notify_one();
    }

    virtual bool trySteal(std::vector<Task> &tasks)
    {
        bool result = _primary_task_queue.trySteal(tasks, _config.getTaskStealNum());
        INT size = _config.getTaskStealNum() - tasks.size();
        if (size > 0) {
            result |= _normal_task_queue.trySteal(tasks, size);
        }
        return result;
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

    bool isThreadCreated()
    {
        return _is_init;
    }
    NO_ALLOWED_COPY(ThreadObject)
protected:
    bool _done = true;                           // 是否线程是否借结束
    bool _is_init = false;                       // 初始化状态
    bool _is_running = false;                    // 是否正在执行
    ThreadConfig _config;                        // 线程配置
    INT _cur_empty_epoch = 0;                    // 当前轮转次数
    WorkStealingQueue<Task> _primary_task_queue; // 主任务队列
    WorkStealingQueue<Task> _normal_task_queue;  // 副任务队列
    std::condition_variable _cv;                 // 条件变量 唤醒线程
    std::mutex _mutex;
    std::thread _thread;
};

KURAXII_NAMESPACE_END

#endif