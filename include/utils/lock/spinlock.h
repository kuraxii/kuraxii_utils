/*********************************************
 * @FileName: spinlock.h
 * @Author: kuraxii
 * @Mail: zj.zhu.cn@gmail.com
 * @CreatedTime: 周二 7月 23 10:37:30 2024
 * @Descript: 自旋锁
 */
#ifndef _SPINLOCK_H
#define _SPINLOCK_H

#include <atomic>
#include <thread>
#include "../../basic/BasicInclude.h"

KURAXII_NAMESPACE_BEGIN

class SpinLock {
public:
    void lock()
    {
        // lock后面的是临界资源，不能
        // memory_order_acquire 后面访存指令勿重排至此条指令之前
        while (_flag.test_and_set(std::memory_order_acquire)) {
            std::this_thread::yield(); // 让出CPU时间片，防止忙等待占用CPU
        }
    }

    // 解锁
    void unlock()
    {
        // memory_order_release 前面访存指令勿重排到此条指令之后
        _flag.clear(std::memory_order_release);
    }

    // 尝试加锁。若未加锁，会上锁
    bool tryLock()
    {
        return !_flag.test_and_set();
    }
    SpinLock(const SpinLock &) = delete;
    SpinLock &operator=(const SpinLock &) = delete;

private:
    std::atomic_flag _flag = ATOMIC_FLAG_INIT;
};

KURAXII_NAMESPACE_END

#endif