#ifndef _STATUS_H
#define _STATUS_H

#include "datadef.h"

KURAXII_NAMESPACE_BEGIN

enum STATUSCODE {
    STATUS_OK = 0,
    STATUS_ERR = -1,
};
class STATUS {
public:
    explicit STATUS(STATUSCODE code = STATUSCODE::STATUS_OK) : status(code)
    {
    }

    INT getCode() const
    {
        return this->status;
    }

    bool isOk() const
    {
        return this->status == STATUSCODE::STATUS_OK;
    }

    bool isErr() const
    {
        return this->status == STATUSCODE::STATUS_ERR;
    }

private:
    STATUSCODE status;
};

KURAXII_NAMESPACE_END

#endif