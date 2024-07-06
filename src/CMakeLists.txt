set(BINARY kuraxii_utils)


# 编译libCThreadPool静态库
file(GLOB_RECURSE UTILS_SRC "utils/threadpool/ThreadPool.cpp")
add_library(${BINARY}_static STATIC ${UTILS_SRC})


# 编译libCThreadPool动态库
add_library(${BINARY}_shared SHARED ${UTILS_SRC})
# target_link_libraries(${BINARY} pthread)