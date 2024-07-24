/*********************************************
 * @FileName: AtomicUnorderedMap.h
 * @Author: kuraxii
 * @Mail: zj.zhu.cn@gmail.com
 * @CreatedTime: 周二 7月 23 10:17:11 2024
 * @Descript: 线程安全的unordered_map
 */

#ifndef _ATOMIC_UNORDERED_MAP_H
#define _ATOMIC_UNORDERED_MAP_H
#include <unordered_map>
#include <optional>
#include <mutex>
#include "../../basic/BasicInclude.h"

KURAXII_NAMESPACE_BEGIN

template <typename Key, typename Value>
class AtomicUnorderedMap {

public:
    AtomicUnorderedMap() = default;
    ~AtomicUnorderedMap() = default;
    NO_ALLOWED_COPY(AtomicUnorderedMap)
    void insert(const Key &key, const Value &value)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        map_[key] = value;
    }
    void modify(const Key &key, const Value &value)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        map_[key] = value; // 插入或修改元素
    }

    std::optional<Value> get(const Key &key)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = map_.find(key);
        if (it != map_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    void remove(const Key &key)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        map_.erase(key);
    }

    bool contains(const Key &key)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return map_.find(key) != map_.end();
    }

private:
    std::unordered_map<Key, Value> map_;
    std::mutex mutex_;
};

KURAXII_NAMESPACE_END
#endif