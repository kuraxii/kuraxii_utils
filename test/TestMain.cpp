#include <gtest/gtest.h>
#include <atomic>
#include <basic/BasicInclude.h>
#include <utils/threadpool/ThreadPool.h>
#include <utils/task/task.h>
#include <utils/task/taskgroup.h>
class ThreadPoolTest : public ::testing::Test {
protected:
    KURAXII::ThreadPool threadPool;

    void SetUp() override
    {
        // Initialize thread pool before each test
        threadPool.init();
    }

    void TearDown() override
    {
        // Destroy thread pool after each test
        threadPool.destroy();
    }
};

TEST_F(ThreadPoolTest, InitializeThreadPool)
{
    EXPECT_TRUE(threadPool.isInit());
}

TEST_F(ThreadPoolTest, AddSingleTask)
{
    std::atomic<int> counter = 0;

    KURAXII::Task task([&counter]() { counter++; });

    threadPool.addTask(task);

    // Give some time for the task to execute
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_EQ(counter.load(), 1);
}

TEST_F(ThreadPoolTest, AddMultipleTasks)
{
    std::atomic<int> counter = 0;
    const int taskCount = 10;

    KURAXII::TaskGroup tasks;
    for (int i = 0; i < taskCount; i++) {
        tasks.addTask(KURAXII::Task([&counter]() { counter++; }));
    }

    threadPool.addTask(tasks);

    // Give some time for all tasks to execute
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    EXPECT_EQ(counter.load(), taskCount);
}

TEST_F(ThreadPoolTest, TaskWithPriority)
{
    std::atomic<int> counter = 0;

    KURAXII::Task highPriorityTask([&counter]() { counter += 10; }, 1); // Priority 1

    KURAXII::Task lowPriorityTask([&counter]() { counter++; });

    threadPool.addTask(lowPriorityTask);
    threadPool.addTask(highPriorityTask);

    // Give some time for tasks to execute
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_EQ(counter.load(), 11); // highPriorityTask should execute first
}

TEST_F(ThreadPoolTest, AddTaskToUninitializedThreadPool)
{
    KURAXII::ThreadPool newThreadPool; // Do not initialize

    std::atomic<int> counter = 0;
    KURAXII::Task task([&counter]() { counter++; });

    // This should not add the task since the thread pool is not initialized
    newThreadPool.addTask(task);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_EQ(counter.load(), 0);
}

TEST_F(ThreadPoolTest, DestroyThreadPool)
{
    threadPool.destroy();

    EXPECT_FALSE(threadPool.isInit());
}

TEST_F(ThreadPoolTest, AddTaskToDestroyedThreadPool)
{
    threadPool.destroy();

    std::atomic<int> counter = 0;
    KURAXII::Task task([&counter]() { counter++; });

    // This should not add the task since the thread pool is destroyed
    threadPool.addTask(task);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_EQ(counter.load(), 0);
}

TEST_F(ThreadPoolTest, ReinitializeThreadPool)
{
    threadPool.destroy();

    EXPECT_FALSE(threadPool.isInit());

    threadPool.init();

    EXPECT_TRUE(threadPool.isInit());
}

TEST_F(ThreadPoolTest, StealTask)
{
    std::atomic<int> counter = 0;
    const int taskCount = 5;

    KURAXII::TaskGroup tasks;
    for (int i = 0; i < taskCount; i++) {
        tasks.addTask(KURAXII::Task([&counter]() { counter++; }));
    }

    threadPool.addTask(tasks);

    // Artificially simulate task stealing
    for (int i = 0; i < taskCount; i++) {
        KURAXII::Task stealTask([&counter]() { counter++; });
        threadPool.addTask(stealTask);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    EXPECT_EQ(counter.load(), taskCount * 2); // All tasks should be executed
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}