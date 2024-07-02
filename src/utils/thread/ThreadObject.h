#ifndef _THREADOBJECT_H
#define _THREADOBJECT_H

#include <basic/BasicInclude.h>
#include <mutex>
KURAXII_NAMESPACE_BEGIN

class ThreadObject : Object {

public:
    ThreadObject() = default;
    ~ThreadObject() = default;
    virtual void processTask() = 0;
    virtual void processTasks() = 0;
    virtual void loopThread() = 0;

    // 显示销毁线程
    virtual void reset() = 0;
    virtual void destory() = 0;
    
protected:
    std::mutex _mutex;
};

KURAXII_NAMESPACE_END

#endif