#ifndef _QUEUEOBJECT_H
#define _QUEUEOBJECT_H

#include "basic/datadef.h"
#include "basic/objectclass.h"
#include <basic/BasicInclude.h>
#include <condition_variable>
#include <mutex>

KURAXII_NAMESPACE_BEGIN

class QueueObject : public Object {

protected:
    std::mutex _mutex;
    std::condition_variable _cv;
};

KURAXII_NAMESPACE_END

#endif