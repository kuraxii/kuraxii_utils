cmake_minimum_required(VERSION 3.18)

project(ThreadPool VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 11)



file(GLOB_RECURSE CTP_SRC_LIST "./src/*.cpp")
include_directories(./src)
# 如果开启此宏定义，则CGraph执行过程中，不会在控制台打印任何信息
# add_definitions(-D_CGRAPH_SILENCE_)

# 编译libCThreadPool动态库
# add_library(CThreadPool SHARED ${CTP_SRC_LIST})

# 编译libCThreadPool静态库
# add_library(CThreadPool STATIC ${CTP_SRC_LIST})

add_executable(CThreadPool
        ${CTP_SRC_LIST}
        tutorial.cpp)
target_link_libraries(CThreadPool pthread)
