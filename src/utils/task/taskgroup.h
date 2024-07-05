#ifndef _TASKGROUP_H
#define _TASKGROUP_H

#include <vector>
#include <basic/BasicInclude.h>
#include "task.h"

KURAXII_NAMESPACE_BEGIN

// 只有默认优先级
class TaskGroup {
public:
    TaskGroup() = default;

    TaskGroup(KURAXII_MOVE_FUNCTION_REF func)
    {
        taskGroup.emplace_back(std::move(func));
    }

    TaskGroup(Task &&task)
    {
        taskGroup.emplace_back(std::move(task));
    }
    TaskGroup(TaskGroup &&other) noexcept : taskGroup(std::move(other.taskGroup))
    {
    }

    TaskGroup &operator=(TaskGroup &&other) noexcept
    {
        if (this != &other) {
            taskGroup = std::move(other.taskGroup);
        }
        return *this;
    }

    void addTask(Task &&task)
    {
        taskGroup.emplace_back(std::move(task));
    }

    void addTaskGroup(TaskGroup &&other)
    {
        for (auto &it : other.taskGroup) {
            taskGroup.emplace_back(std::move(it));
        }
        other.taskGroup.clear();
    }

    std::vector<Task> &getTasks()
    {
        return taskGroup;
    }

    NO_ALLOWED_COPY(TaskGroup)

private:
    std::vector<Task> taskGroup;
};

KURAXII_NAMESPACE_END

#endif