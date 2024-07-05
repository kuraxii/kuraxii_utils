#ifndef _THREADPOOL_H
#define _THREADPOOL_H
#include <basic/BasicInclude.h>
#include <utils/task/task.h>
#include <future>
#include "ThreadPoolConfig.h"
#include <utils/queue/AtomicQueue.h>
#include <utils/queue/AtomicPriorityQueue.h>
#include <utils/thread/ThreadObject.h>
KURAXII_NAMESPACE_BEGIN

class ThreadPool;

class ThreadPrimary : public ThreadObject {
public:
    ThreadPrimary(ThreadPool &pool);
    virtual void init() override;
    void processTask() override;
    bool popPoolTask(std::vector<Task> tasks, UINT taskNum);
    bool stealTask(std::vector<Task> tasks, UINT taskNum);
    void setStealTargets(INT index);

private:
    UINT _index;                     // 当前线程的索引
    std::vector<INT> _steal_targets; // 盗取目标

    // 线程池属性
    ThreadPoolConfig &_pool_config;
    std::vector<ThreadPrimary> &_pool_threads;
    AtomicQueue<Task> &_pool_task_queue;
    AtomicPriorityQueue<Task> &_pool_priority_task_queue;
};

// 副线程不能steal
class ThreadSecondary : public ThreadObject {
public:
    ThreadSecondary(ThreadPool &pool);
    virtual void init() override;
    void processTask() override;
    bool popPoolTask(std::vector<Task> tasks, UINT taskNum);

private:
    // 线程池属性
    ThreadPoolConfig &_pool_config;
    AtomicQueue<Task> &_pool_task_queue;
    AtomicPriorityQueue<Task> &_pool_priority_task_queue;
};

class ThreadPool : Object {
public:
    explicit ThreadPool() noexcept;

    /**
     * 析构函数
     */
    ~ThreadPool() = default;

    void init();

    void addTask(Task &&task);
    void addTask(TaskGroup &&tasks);
    void addLongTask(Task &&task);



    /**
     * 获取根据线程id信息，获取线程index信息
     * @param tid
     * @return
     * @notice 辅助线程返回-1
     */
    INT getThreadIndex(UINT tid);

    /**
     * 释放所有的线程信息
     * @return
     */
    bool destroy();

    bool isInit() const;

    // clang-format off
    inline std::vector<ThreadPrimary>& getThreads() { return _threads_primary; }
    inline AtomicQueue<Task>& getPoolTaskQueue() { return _pool_task_queue; }
    inline AtomicPriorityQueue<Task>& getPoolPriorityTaskQueue() { return _pool_priority_task_queue; }
    inline ThreadPoolConfig& getThreadPoolConfig() { return _config; }
    //clang-format on

    NO_ALLOWED_COPY_AND_MOVE(ThreadPool)

private:
    INT _cur_index;
    bool _is_init;

    ThreadPoolConfig _config;
    // 主线程执行短任务
    std::vector<ThreadPrimary> _threads_primary;
    // 副线程执行长任务，比如需要while(1) 的线程
    std::vector<ThreadSecondary> _threads_secondary;
    AtomicQueue<Task> _pool_task_queue;  // 普通队列
    AtomicPriorityQueue<Task> _pool_priority_task_queue; // 优先级队列
};

KURAXII_NAMESPACE_END

#endif