#ifndef _THREADPRIMARY_H
#define _THREADPRIMARY_H

#include <basic/BasicInclude.h>
#include <utils/thread/ThreadObject.h>
#include <utils/queue/WorkStealingQueue.h>
#include <utils/threadpool/ThreadPool.h>
#include <utils/threadpool/ThreadPoolConfig.h>
KURAXII_NAMESPACE_BEGIN

class ThreadPrimary : public ThreadObject {
public:
    ThreadPrimary(ThreadPool pool)
        : _pool_config(pool.getThreadPoolConfig()), _pool_threads(pool.getThreads()),
          _pool_task_queue(pool.getPoolTaskQueue()), _pool_priority_task_queue(pool.getPoolPriorityTaskQueue())
    {
        init();
    }

    virtual void init() override
    {
        // 确保线程池完全启动

        _thread = std::move(std::thread{std::bind(&ThreadObject::loopThread, this)});
        _is_init = true;
        _done = false;
    }

    void processTask() override
    {
        bool result = false;
        static thread_local std::vector<Task> tasks;
        if (_config.getBatchTaskEnable()) {
            //clang-format off
            result =
                (popTask(tasks, _config.getMaxTaskBatchSize()) || popPoolTask(tasks, _config.getMaxTaskBatchSize()) ||
                 stealTask(tasks, _config.getMaxTaskBatchSize()));
            //clang-format on
        } else {
            result = (popTask(tasks, 1) || popPoolTask(tasks, 1) || stealTask(tasks, 1));
        }
        if (result) {
            runTask(tasks);
            tasks.clear();
        } else {
            fatWait();
        }
    }
    bool popPoolTask(std::vector<Task> tasks, UINT taskNum)
    {
        // 优先从优先队列中取出
        bool result = _pool_priority_task_queue.tryPop(tasks, taskNum);
        INT size = taskNum - tasks.size();
        if (size > 0) {
            result |= _second_task_queue.tryPop(tasks, size);
        }
        return result;
    }

    bool stealTask(std::vector<Task> tasks, UINT taskNum)
    {
        bool result = false;
        for (auto &target : _steal_targets) {
            if (_pool_threads[target].isThreadCreated()) {
                result = _pool_threads[target].trySteal(tasks);
            }
        }
        return result;
    }

    void setStealTargets(INT index)
    {
        _steal_targets.clear();
        for (int i = 0; i < _config.getTaskStealNum(); i++) {
            auto target = (index + i + 1) % _pool_config.default_thread_size_;
            _steal_targets.push_back(target);
        }
    }

private:
    std::vector<INT> _steal_targets; // 盗取目标

    // 线程池属性
    ThreadPoolConfig &_pool_config;
    std::vector<ThreadObject> &_pool_threads;
    AtomicQueue<Task> &_pool_task_queue;
    AtomicPriorityQueue<Task> &_pool_priority_task_queue;
};

KURAXII_NAMESPACE_END

#endif