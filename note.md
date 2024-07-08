# note

## 完美转发 std::forward 

传入参数的类型和值类别（左值或右值）在传递过程中可能会发生变化，特别是在模板函数中。这是由于C++的参数传递和引用折叠规则所导致的。为了保持传入参数的类型和值类别不变，我们使用了完美转发（perfect forwarding）。

问题：
T &&task 什么时候是万能引用，什么时候是右值引用 
在模板函数中 T &&task 就是万能引用，普通函数就是右值引用
对于万能引用使用std::forward 才有效
```cpp
template <typename T>
class A{
    // 右值引用
    void func(T &&value);
    // 万能引用
    template <typename U>
    void func(U &&value);
};

// 右值引用
void func(int &&value);
    // 万能引用
template <typename T>
void func(T &&value);

```



问题：线程的退出时间很长
最开始使用perf工具来排错但是并找不到关键的应用层函数的问题
后来开始使用valgrind工具
最终使用打印log日志的方式来进行问题排查，最后发现线程池的析构函数调用时间很长。最后想到了可能是哪个线程的调度的问题。每个线程在执行时会尝试从自己的任务队列，线程池队列和相邻线程队列盗取任务
如果失败的话就会使线程进入休眠状态。这个休眠在最坏的情况下就可能线性休眠顺序，16个线程就会导析构后函数等待16*休眠的时间。主要减少休眠时间就可以大幅减少线程池的析构时间


问题：优先队列的top返回 const ref如何使用移动语义
使用const_cast强转
```cpp
 bool tryPop(T &value)
{
    std::unique_lock<std::mutex> lock(_mutex, std::try_to_lock);
    if (!lock.owns_lock() || _priority_queue.empty()) {
        return false;
    }
    value = std::move(const_cast<T &>(_priority_queue.top()));
    _priority_queue.pop();
    return true;
}
```











