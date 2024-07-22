#ifndef _FUNCDEF_H
#define _FUNCDEF_H

#include <functional>
#include "datadef.h"
#include <string>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <iostream>
KURAXII_NAMESPACE_BEGIN

using KURAXII_DEFAULT_FUNCTION = std::function<void()>;
using KURAXII_MOVE_FUNCTION_REF = std::function<void()> &&;

/** 定义为不能赋值和拷贝的对象类型 */
#define NO_ALLOWED_COPY(Type)    \
    Type(const Type &) = delete; \
    const Type &operator=(const Type &) = delete;

#define NO_ALLOWED_COPY_AND_MOVE(Type) \
    NO_ALLOWED_COPY(Type)              \
    Type(Type &&) = delete;            \
    Type &operator=(Type &&) = delete;

// 获取当前时间的字符串表示
inline std::string getCurrentTime()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
    return ss.str();
}

// 封装的日志函数
inline void logWithInfo(const std::string &customMessage, const char *className, const char *functionName,
                        int lineNumber)
{
    std::cout << "[" << getCurrentTime() << "] " << className << "::" << functionName << " "
              << "Line: " << lineNumber << " " << customMessage << std::endl;
}

#define LOG_MESSAGE(msg) logWithInfo(msg, typeid(*this).name(), __func__, __LINE__)

KURAXII_NAMESPACE_END

#endif