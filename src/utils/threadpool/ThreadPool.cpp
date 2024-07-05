/*********************************************
 * @FileName: ThreadPool.cpp
 * @Author: kuraxii
 * @Mail: zj.zhu.cn@gmail.com
 * @CreatedTime: 周四 7月 04 23:50:53 2024
 * @Descript:
 */

#include "ThreadPool.h"
#include <basic/BasicInclude.h>
#include <memory>
KURAXII_NAMESPACE_BEGIN
ThreadPool::ThreadPool() noexcept
{
    _cur_index = 0;
    _is_init = false;
    init();
}

void ThreadPool::init()
{
    if (_is_init) {
        return;
    }
    _threads_primary.reserve(_config.default_thread_size_);
    for (int i = 0; i < _config.default_thread_size_; i++) {
        _threads_primary.emplace_back(std::make_unique<ThreadPrimary>(*this));
    }
    _is_init = true;
}

void ThreadPool::addTask(Task &&task)
{
    if (task.getPriority() > 0) {
        _pool_priority_task_queue.push(std::move(task));
    } else {
        _pool_task_queue.push(std::move(task));
    }
}

void ThreadPool::addTask(TaskGroup &&tasks)
{
    _pool_task_queue.push(tasks.getTasks());
}

// 创建副线程 并执行
void addLongTask(Task &&task)
{
}

bool ThreadPool::destroy()
{
    for (auto &thread : _threads_primary) {
        thread.destory();
    }
    for (auto &thread : _threads_secondary) {
        thread.destory();
    }
}

ThreadPrimary::ThreadPrimary(ThreadPool &pool)
    : _pool_config(pool.getThreadPoolConfig()), _pool_threads(pool.getThreads()),
      _pool_task_queue(pool.getPoolTaskQueue()), _pool_priority_task_queue(pool.getPoolPriorityTaskQueue())
{
    init();
}

void ThreadPrimary::init()
{
    // 确保线程池完全启动

    _thread = std::move(std::thread{std::bind(&ThreadObject::loopThread, this)});
    _is_init = true;
    _done = false;
}

void ThreadPrimary::processTask()
{
    bool result = false;
    static thread_local std::vector<Task> tasks;
    if (_config.getBatchTaskEnable()) {
        //clang-format off
        result = (popTask(tasks, _config.getMaxTaskBatchSize()) || popPoolTask(tasks, _config.getMaxTaskBatchSize()) ||
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

bool ThreadPrimary::popPoolTask(std::vector<Task> tasks, UINT taskNum)
{
    // 优先从优先队列中取出
    bool result = _pool_priority_task_queue.tryPop(tasks, taskNum);
    INT size = taskNum - tasks.size();
    if (size > 0) {
        result |= _second_task_queue.tryPop(tasks, size);
    }
    return result;
}

bool ThreadPrimary::stealTask(std::vector<Task> tasks, UINT taskNum)
{
    bool result = false;
    for (auto &target : _steal_targets) {
        if (_pool_threads[target].isThreadCreated()) {
            result = _pool_threads[target].trySteal(tasks);
        }
    }
    return result;
}

void ThreadPrimary::setStealTargets(INT index)
{
    _steal_targets.clear();
    for (int i = 0; i < _config.getTaskStealNum(); i++) {
        auto target = (index + i + 1) % _pool_config.default_thread_size_;
        _steal_targets.push_back(target);
    }
}

ThreadSecondary::ThreadSecondary(ThreadPool &pool)
    : _pool_config(pool.getThreadPoolConfig()), _pool_task_queue(pool.getPoolTaskQueue()),
      _pool_priority_task_queue(pool.getPoolPriorityTaskQueue())
{
    init();
}

void ThreadSecondary::init()
{
    // 确保线程池完全启动

    _thread = std::move(std::thread{std::bind(&ThreadObject::loopThread, this)});
    _is_init = true;
    _done = false;
}

void ThreadSecondary::processTask()
{
    bool result = false;
    static thread_local std::vector<Task> tasks;
    if (_config.getBatchTaskEnable()) {
        //clang-format off
        result = (popTask(tasks, _config.getMaxTaskBatchSize()) || popPoolTask(tasks, _config.getMaxTaskBatchSize()));
        //clang-format on
    } else {
        result = (popTask(tasks, 1) || popPoolTask(tasks, 1));
    }
    if (result) {
        runTask(tasks);
        tasks.clear();
    } else {
        fatWait();
    }
}

bool ThreadSecondary::popPoolTask(std::vector<Task> tasks, UINT taskNum)
{
    // 优先从优先队列中取出
    bool result = _pool_priority_task_queue.tryPop(tasks, taskNum);
    INT size = taskNum - tasks.size();
    if (size > 0) {
        result |= _second_task_queue.tryPop(tasks, size);
    }
    return result;
}

KURAXII_NAMESPACE_END