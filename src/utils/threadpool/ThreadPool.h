#ifndef _THREADPOOL_H
#define _THREADPOOL_H
#include <basic/BasicInclude.h>
#include <utils/task/task.h>
#include <utils/thread/ThreadObject.h>
#include <future>
#include "ThreadPoolConfig.h"
#include <utils/queue/AtomicQueue.h>
#include <utils/queue/AtomicPriorityQueue.h>
KURAXII_NAMESPACE_BEGIN

class ThreadPool : Object {
public:
    std::future<void> addTask(Task &&task);
    std::future<void> addTask(TaskGroup &&tasks);
    // 同步添加
    std::future<void> addTaskSync(TaskGroup &&tasks);

    // clang-format off
    inline std::vector<ThreadObject>& getThreads() { return _threads; }
    inline AtomicQueue<Task>& getPoolTaskQueue() { return _pool_task_queue; }
    inline AtomicPriorityQueue<Task>& getPoolPriorityTaskQueue() { return _pool_priority_task_queue; }
    inline ThreadPoolConfig& getThreadPoolConfig() { return _config; }
    //clang-format on
private:
    ThreadPoolConfig _config;
    std::vector<ThreadObject> _threads;
    AtomicQueue<Task> _pool_task_queue;
    AtomicPriorityQueue<Task> _pool_priority_task_queue;
};

KURAXII_NAMESPACE_END

#endif