/*********************************************
 * @FileName: ThreadPool.cpp
 * @Author: kuraxii
 * @Mail: zj.zhu.cn@gmail.com
 * @CreatedTime: 周四 7月 04 23:50:53 2024
 * @Descript:
 */

#include "utils/threadpool/ThreadPool.h"
#include <basic/BasicInclude.h>
#include <memory>
KURAXII_NAMESPACE_BEGIN
ThreadPool::ThreadPool() noexcept
{
    
}

void ThreadPool::init()
{
    if (_is_init) {
        return;
    }
    _threads_primary.reserve(_config.default_thread_size_);
    for (int i = 0; i < _config.default_thread_size_; i++) {
        try {
            _threads_primary.emplace_back(std::make_unique<ThreadPrimary>(*this, i));
        }
        catch (const std::exception &e) {
            std::cerr << "Failed to create ThreadPrimary: " << e.what() << std::endl;
            throw; // Re-throw the exception after logging it
        }
    }
    _is_init = true;
    _is_allow_steal = true;
}

void ThreadPool::addTask(Task &task)
{
    if (!_is_init) {
        return;
    }
    if (task.getPriority() > 0) {
        _pool_priority_task_queue.push(task);
    } else {
        INT index = dispatch(_cur_index);

        if (index < _config.default_thread_size_) {
            _threads_primary[index]->pushTask(std::move(task));

        } else {
            _pool_task_queue.push(task);
        }
    }
}

void ThreadPool::addTask(TaskGroup &tasks)
{
    if (!_is_init) {
        return;
    }
    for (auto &task : tasks.getTasks()) {
        addTask(task);
    }
}

void ThreadPool::destroy()
{
    if (_is_init) {
        for (auto &thread : _threads_primary) {
            thread->destory();
        }
        _is_init = false;
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

bool ThreadPool::isInit() const
{
    return _is_init;
}

ThreadPool::~ThreadPool()
{
    _is_allow_steal = false;
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
    : _index(index), _pool_is_allow_steal(pool.getIsAllowSteal()), _pool_config(pool.getThreadPoolConfig()),
      _pool_threads(pool.getThreads()), _pool_task_queue(pool.getPoolTaskQueue()),
      _pool_priority_task_queue(pool.getPoolPriorityTaskQueue())
{
    init();
}

void ThreadPrimary::init()
{
    // 确保线程池完全启动
    _done = false;
    _thread = std::thread(&ThreadPrimary::loopThread, this);
    setStealTargets(_index);
    _is_init = true;
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

    // 等待线程池初始化完成再steal
    if (*_pool_is_allow_steal) {
        for (auto &target : _steal_targets) {
            // std::cout << "_pool_is_init:" << _pool_is_init << " target: " << target << std::endl;
            if ((*_pool_threads)[target] && (*_pool_threads)[target]->isThreadCreated()) {
                result = (*_pool_threads)[target]->trySteal(tasks);
            }
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