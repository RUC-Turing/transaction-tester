#ifndef _MENCI_TRANSACTION_TESTER_FRAMEWORK_OPERATION_H
#define _MENCI_TRANSACTION_TESTER_FRAMEWORK_OPERATION_H

#include <string>
#include <functional>

#include "RecordType.h"

struct Operation {
    enum Type {
        READ, WRITE
    } type;

    RecordKey key;
    RecordData newDataOrReadResult;
};

#endif // _MENCI_TRANSACTION_TESTER_FRAMEWORK_OPERATION_H
