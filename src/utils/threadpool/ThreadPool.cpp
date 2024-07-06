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
    init();
}

void ThreadPool::init()
{
    if (_is_init) {
        return;
    }
    _threads_primary.reserve(_config.default_thread_size_);
    for (int i = 0; i < _config.default_thread_size_; i++) {
        try {
            //
            // new ThreadPrimary(*this, i);
            _threads_primary.emplace_back(std::make_unique<ThreadPrimary>(*this, i));
        }
        catch (const std::exception &e) {
            std::cerr << "Failed to create ThreadPrimary: " << e.what() << std::endl;
            throw; // Re-throw the exception after logging it
        }
    }
    _is_init = true;
    std::cout << "poll init" << std::endl;
}

void ThreadPool::addTask(Task &&task)
{
    if (task.getPriority() > 0) {
        _pool_priority_task_queue.push(std::move(task));
    } else {
        INT index = dispatch(_cur_index);
        if (index < _config.default_thread_size_) {
            _threads_primary[index]->pushTask(std::move(task));
        } else {
            _pool_task_queue.push(std::move(task));
        }
    }
    std::cout << "addtask succ\n" << std::endl;
}

void ThreadPool::addTask(TaskGroup &&tasks)
{
    for (auto &task : tasks.getTasks()) {
        addTask(std::move(task));
    }
}

void ThreadPool::destroy()
{
    for (auto &thread : _threads_primary) {
        thread->destory();
    }
}

INT ThreadPool::dispatch(INT &index)
{
    INT realIndex;
    realIndex = index++;
    if (index > _config.default_thread_size_ || index < 0) {
        index = 0;
    }
    return realIndex;
}

ThreadPool::~ThreadPool()
{
    destroy();
}

ThreadPrimary::ThreadPrimary(ThreadPrimary &&other) noexcept
    : _index(other._index), _steal_targets(std::move(other._steal_targets)), _pool_config(other._pool_config),
      _pool_threads(other._pool_threads), _pool_task_queue(other._pool_task_queue),
      _pool_priority_task_queue(other._pool_priority_task_queue)
{
    other._pool_config = nullptr;
    other._pool_threads = nullptr;
    other._pool_task_queue = nullptr;
    other._pool_priority_task_queue = nullptr;
}
ThreadPrimary &ThreadPrimary::operator=(ThreadPrimary &&other) noexcept
{
    if (this != &other) {
        _index = other._index;
        _steal_targets = std::move(other._steal_targets);
        _pool_config = other._pool_config;
        _pool_threads = other._pool_threads;
        _pool_task_queue = other._pool_task_queue;
        _pool_priority_task_queue = other._pool_priority_task_queue;

        other._pool_config = nullptr;
        other._pool_threads = nullptr;
        other._pool_task_queue = nullptr;
        other._pool_priority_task_queue = nullptr;
    }
    return *this;
}

ThreadPrimary::ThreadPrimary(ThreadPool &pool, INT index)
    : _index(index), _pool_config(pool.getThreadPoolConfig()), _pool_threads(pool.getThreads()),
      _pool_task_queue(pool.getPoolTaskQueue()), _pool_priority_task_queue(pool.getPoolPriorityTaskQueue())
{
    init();
}

void ThreadPrimary::init()
{
    // 确保线程池完全启动
    std::cout << "Initializing thread " << _index << std::endl;
    try {
        _thread = std::thread(&ThreadPrimary::loopThread, this);
    }
    catch (const std::exception &e) {
        std::cerr << "Failed to create thread: " << e.what() << std::endl;
        throw;
    }
    _is_init = true;
    _done = false;
    setStealTargets(_index);
    std::cout << "Thread " << _index << " initialized" << std::endl;
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

bool ThreadPrimary::popPoolTask(std::vector<Task> &tasks, UINT taskNum)
{
    // 优先从优先队列中取出
    bool result = (*_pool_priority_task_queue).tryPop(tasks, taskNum);
    INT size = taskNum - tasks.size();
    if (size > 0) {
        result |= _normal_task_queue.tryPop(tasks, size);
    }
    return result;
}

bool ThreadPrimary::stealTask(std::vector<Task> &tasks, UINT taskNum)
{
    bool result = false;

    for (auto &target : _steal_targets) {
        if ((*_pool_threads)[target] && (*_pool_threads)[target]->isThreadCreated()) {
            result = (*_pool_threads)[target]->trySteal(tasks);
        }
    }
    return result;
}

void ThreadPrimary::setStealTargets(INT index)
{
    _steal_targets.clear();
    for (int i = 0; i < _config.getTaskStealNum(); i++) {
        auto target = (index + i + 1) % (*_pool_config).default_thread_size_;
        _steal_targets.push_back(target);
    }
}

KURAXII_NAMESPACE_END