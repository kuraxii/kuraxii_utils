cmake_minimum_required(VERSION 3.18)

project(test VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)



file(GLOB_RECURSE SOURCES "src/*.cpp" "include/*.h" "test/*cpp")
include_directories(./include)


add_subdirectory(src)
add_subdirectory(test)

add_executable(tutorial tutorial.cpp)
target_link_libraries(tutorial kuraxii_utils_static pthread)
