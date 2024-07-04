cmake_minimum_required(VERSION 3.18)

project(ThreadPool VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 11)



file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")
include_directories(./src)


add_subdirectory(src)
# add_subdirectory(test)

add_executable(ThreadPool
        tutorial.cpp)
target_link_libraries(ThreadPool pthread)
