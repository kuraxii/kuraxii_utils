/*********************************************
 * @FileName: atomic_list.h
 * @Author: kuraxii
 * @Mail: zj.zhu.cn@gmail.com
 * @CreatedTime: 周二 7月 23 10:16:33 2024
 * @Descript: 线程安全的list
 */

#ifndef _ATMOIC_LIST_H
#define _ATMOIC_LIST_H
#include <list>
#include <mutex>
#include <optional>
#include "../../basic/BasicInclude.h"

KURAXII_NAMESPACE_BEGIN
template <typename T>
class AtomicList {

public:
    AtomicList() = default;
    ~AtomicList() = default;
    // 禁用拷贝和赋值
    NO_ALLOWED_COPY(AtomicList)
    // 支持移动构造
    AtomicList(AtomicList &&other) noexcept
    {
        std::lock_guard<std::mutex> lock(other.mutex_);
        list_ = std::move(other);
    }

    AtomicList &operator=(AtomicList &&other) noexcept
    {
        if (this != &other) {
            std::unique_lock<std::mutex> lockThis(mutex_, std::defer_lock);
            std::unique_lock<std::mutex> lockOther(mutex_, std::defer_lock);
            std::lock(lockThis, lockOther);
            list_ = std::move(other.list_);
        }
        return *this;
    }

    template <typename... Args>
    void emplace_back(Args &&...args)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        list_.emplace_back(std::forward<Args>(args)...);
    }

    std::optional<T> pop_front()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (list_.empty()) {
            return std::nullopt;
        }
        return list_.front();
    }
    std::optional<T> pop_back()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (list_.empty()) {
            return std::nullopt;
        }
        return list_.back();
    }

    std::optional<T> find_if(std::function<bool(const T &)> predicate) const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = std::find_if(list_.begin(), list_.end(), predicate);
        if (it != list_.end()) {
            return *it;
        }
        return std::nullopt;
    }

    bool find_and_modify(std::function<bool(const T &)> predicate, std::function<void(T &)> modify)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = std::find_if(list_.begin(), list_.end(), predicate);
        if (it != list_.end()) {
            modify(*it);
            return true;
        }
        return false;
    }

    bool empty()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return list_.empty();
    }

    size_t size()
    {
        return list_.size();
    }

private:
    std::list<T> list_;
    mutable std::mutex mutex_; // 为了能够在const函数中上锁 使用mutable
};

KURAXII_NAMESPACE_END
#endif