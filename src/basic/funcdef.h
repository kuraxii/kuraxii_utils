#ifndef _FUNCDEF_H
#define _FUNCDEF_H

#include <functional>
#include "datadef.h"

KURAXII_NAMESPACE_BEGIN

using CGRAPH_DEFAULT_FUNCTION = std::function<void()>;
using CGRAPH_DEFAULT_CONST_FUNCTION_REF = const std::function<void()> &;

/** 定义为不能赋值和拷贝的对象类型 */
#define NO_ALLOWED_COPY(Type) \
    Type(const Type &) = delete;    \
    const Type &operator=(const Type &) = delete;

KURAXII_NAMESPACE_END

#endif