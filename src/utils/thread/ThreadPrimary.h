#ifndef _THREADPRIMARY_H
#define _THREADPRIMARY_H

#include <basic/BasicInclude.h>
#include "ThreadObject.h"
#include <utils/queue/WorkStealingQueue.h>
#include <utils/task/TaskInclude.h>
KURAXII_NAMESPACE_BEGIN

class ThreadPrimary : public ThreadObject {
public:





private:
    WorkStealingQueue<Task> primary_process_queue;
    WorkStealingQueue<Task> second_process_queue;
};

KURAXII_NAMESPACE_END

#endif