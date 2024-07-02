#ifndef _THREADPOOL_H
#define _THREADPOOL_H
#include <basic/BasicInclude.h>
#include <utils/task/TaskInclude.h>
#include <future>
KURAXII_NAMESPACE_BEGIN

class ThreadPool : Object {
    std::future<void> addTask(Task &&task);
    std::future<void> addTask(TaskGroup &&tasks);
    // 同步添加
    std::future<void> addTaskSync(TaskGroup &&tasks);
};

KURAXII_NAMESPACE_END

#endif