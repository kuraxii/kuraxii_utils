set(BINARY ${CMAKE_PROJECT_NAME}_test)

find_package(GTest REQUIRED)

add_executable(AllTest TestMain.cpp)
target_link_libraries(AllTest pthread kuraxii_utils_static ${GTEST_LIBRARIES})