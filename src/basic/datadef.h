#ifndef _DATADEF_H
#define _DATADEF_H

#include <cstddef>
#include <cstdint>

// clang-format off
#define KURAXII_NAMESPACE_BEGIN                        \
namespace KURAXII {                                    

#define KURAXII_NAMESPACE_END                          \
} /* end of namespace THREAD_POOL */

// clang-format on

KURAXII_NAMESPACE_BEGIN

using VOIDPTR = void *;
using CHAR = int8_t;
using UCHAR = uint8_t;
using INT = int32_t;
using UINT = uint32_t;
using INT64 = int64_t;
using UINT64 = uint64_t;
using BOOL = bool;
using FLOAT = float;
using DOUBLE = double;
using CONSTSTR = const char *;
using SIZE = size_t;

KURAXII_NAMESPACE_END

#endif