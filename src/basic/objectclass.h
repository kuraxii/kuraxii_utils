#ifndef _OBJECTCLASS_H
#define _OBJECTCLASS_H

#include "status.h"

KURAXII_NAMESPACE_BEGIN

class Object {
public:
    explicit Object() = default;
    virtual ~Object() = default;
};

KURAXII_NAMESPACE_END

#endif