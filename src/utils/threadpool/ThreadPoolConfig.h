/*********************************************
 * @FileName: ThreadPoolConfig.h
 * @Author: kuraxii
 * @Mail: zj.zhu.cn@gmail.com
 * @CreatedTime: 周四 7月 04 15:44:37 2024
 * @Descript:
 */

#ifndef _THREADPOOLCONFIG_H
#define _THREADPOOLCONFIG_H
#include <algorithm>
#include <basic/BasicInclude.h>
#include <thread>
KURAXII_NAMESPACE_BEGIN

static const UINT HARDWARE_CONCURRENCY = std::thread::hardware_concurrency();

constexpr UINT CGRAPH_THREAD_TYPE_PRIMARY = 1;
constexpr UINT CGRAPH_THREAD_TYPE_SECONDARY = 2;

static const UINT DEFAULT_THREAD_SIZE = HARDWARE_CONCURRENCY; // 默认开启主线程个数
constexpr UINT SECONDARY_THREAD_SIZE = 0;                     // 默认开启辅助线程个数
constexpr UINT MAX_THREAD_SIZE = 16;                          // 最大线程个数

constexpr UINT MAX_LOCAL_BATCH_SIZE = 2; // 批量执行本地任务最大值
constexpr UINT MAX_POOL_BATCH_SIZE = 2;  // 批量执行通用任务最大值

constexpr UINT DEFAULT_TASK = 0;        // 默认线程调度策略
constexpr UINT POOL_TASK_STRATEGY = -2; // 固定用pool中的队列的调度策略
constexpr UINT LONG_TIME_TASK = -101;   // 长时间任务调度策略

struct ThreadPoolConfig {
    UINT getDefaultThreadSize() const
    {
        return default_thread_size_;
    }

    UINT getSecondaryThreadSize() const
    {
        return secondary_thread_size_;
    }

    UINT getMaxThreadSize() const
    {
        return max_thread_size_;
    }
    UINT default_thread_size_ = DEFAULT_THREAD_SIZE;
    UINT secondary_thread_size_ = SECONDARY_THREAD_SIZE;
    UINT max_thread_size_ = MAX_THREAD_SIZE;
};

KURAXII_NAMESPACE_END

#endif