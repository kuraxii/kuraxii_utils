#ifndef _TASKGROUP_H
#define _TASKGROUP_H

#include <vector>
#include <basic/BasicInclude.h>
#include "task.h"

KURAXII_NAMESPACE_BEGIN

// 只有默认优先级
class TaskGroup {
public:
    TaskGroup(Task &&task)
    {
        taskGroup.emplace_back(std::move(task));
    }
    TaskGroup(TaskGroup &&other) : taskGroup(std::move(other.taskGroup))
    {
    }

    TaskGroup &operator=(TaskGroup &&other)
    {
        auto a = &other;
        if (this != &other)
        {
            taskGroup = std::move(other.taskGroup);
        }
        return *this;
    }

    void addTask(Task &&task)
    {
        taskGroup.emplace_back(std::move(task));
    }

    void addTaskGroup(TaskGroup &&TaskGroup)
    {
        for (const auto &it : TaskGroup.taskGroup)
        {
            taskGroup.emplace_back(std::move(it));
        }
    }

    NO_ALLOWED_COPY(TaskGroup)

private:
    std::vector<Task> taskGroup;
};

KURAXII_NAMESPACE_END

#endif