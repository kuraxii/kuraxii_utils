/*********************************************
 * @FileName: ThreadConfig.h
 * @Author: kuraxii
 * @Mail: zj.zhu.cn@gmail.com
 * @CreatedTime: 周三 7月 03 16:06:28 2024
 * @Descript:
 */

#ifndef _THREADCONFIG_H
#define _THREADCONFIG_H

#include <basic/BasicInclude.h>

KURAXII_NAMESPACE_BEGIN

constexpr UINT MAX_TASK_BATCH_SIZE_ = 2;
constexpr UINT PRIMARY_THREAD_BUSY_EPOCH = 2;
constexpr UINT PRIMARY_THREAD_EMPTY_INTERVAL = 3;
constexpr bool BATCH_TASK_ENABLE = true;
constexpr UINT TASK_STEAL_RANGE = 2; // 盗取机制相邻范围
struct ThreadConfig {
public:
    // clang-format off
    UINT getMaxTaskBatchSize() const { return _max_task_batch_size_; }
    UINT getPrimaryThreadBusyEpoch() const { return _primary_thread_busy_epoch; }
    UINT getPrimaryThreadEmptyInterval() const { return _primary_thread_empty_interval; }
    UINT getTaskStealNum() const {return _max_task_steal_range; }
    bool getBatchTaskEnable() const { return _batch_task_enable; }
    // clang-format on

private:
    UINT _max_task_batch_size_ = MAX_TASK_BATCH_SIZE_;
    UINT _primary_thread_busy_epoch = PRIMARY_THREAD_BUSY_EPOCH;         // 线程无任务时休眠次数
    UINT _primary_thread_empty_interval = PRIMARY_THREAD_EMPTY_INTERVAL; // 线程无任务时 休眠时间  ms
    bool _batch_task_enable = BATCH_TASK_ENABLE;
    UINT _max_task_steal_range = TASK_STEAL_RANGE; // 盗取一次盗取的人任务数量
};

KURAXII_NAMESPACE_END

#endif