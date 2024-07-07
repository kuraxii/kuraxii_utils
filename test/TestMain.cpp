#include <gtest/gtest.h>
#include <atomic>
#include <basic/BasicInclude.h>
#include <utils/threadpool/ThreadPool.h>
#include <utils/task/task.h>
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

    KURAXII::Task task([&counter]() {
        counter++;
        std::cout << "counter***********" << std::endl;
    });

    threadPool.addTask(std::move(task));

    // Give some time for the task to execute
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_EQ(counter.load(), 1);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}