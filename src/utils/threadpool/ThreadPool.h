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
    ThreadPrimary() = default;
    ThreadPrimary(ThreadPool &pool, INT index);
    ThreadPrimary(ThreadPrimary &&other) noexcept;            // 移动构造函数
    ThreadPrimary &operator=(ThreadPrimary &&other) noexcept; // 移动赋值操作符
    NO_ALLOWED_COPY(ThreadPrimary)
    virtual void init() override;
    void processTask() override;

private:
    bool popPoolTask(std::vector<Task> &tasks, UINT taskNum);
    bool stealTask(std::vector<Task> &tasks, UINT taskNum);
    void setStealTargets(INT index);

private:
    UINT _index = -1;                // 当前线程的索引
    std::vector<INT> _steal_targets; // 盗取目标

    // 线程池属性
    ThreadPoolConfig *_pool_config = nullptr;
    std::vector<std::unique_ptr<ThreadPrimary>> *_pool_threads = nullptr;
    AtomicQueue<Task> *_pool_task_queue = nullptr;
    AtomicPriorityQueue<Task> *_pool_priority_task_queue = nullptr;
};

class ThreadPool : Object {
public:
    explicit ThreadPool() noexcept;

    /**
     * 析构函数
     */
    ~ThreadPool();
    /**
     * 初始化函数
     */

    void addTask(Task &&task);
    void addTask(TaskGroup &&tasks);

    void destroy();

    bool isInit() const;

    // clang-format off
    inline std::vector<std::unique_ptr<ThreadPrimary>>* getThreads() { return &_threads_primary; }
    inline AtomicQueue<Task>* getPoolTaskQueue() { return &_pool_task_queue; }
    inline AtomicPriorityQueue<Task>* getPoolPriorityTaskQueue() { return &_pool_priority_task_queue; }
    inline ThreadPoolConfig* getThreadPoolConfig() { return &_config; }
    //clang-format on

    NO_ALLOWED_COPY_AND_MOVE(ThreadPool)

private:
    INT dispatch(INT &index);
    void init();

private:
    INT _cur_index = 0;
 
    bool _is_init = false;

    ThreadPoolConfig _config;
    // 主线程执行短任务
    std::vector<std::unique_ptr<ThreadPrimary>> _threads_primary;
    AtomicQueue<Task> _pool_task_queue;  // 普通队列
    AtomicPriorityQueue<Task> _pool_priority_task_queue; // 优先级队列 
};

KURAXII_NAMESPACE_END

#endif