#ifndef _MENCI_TRANSACTION_TESTER_OPERATION_H
#define _MENCI_TRANSACTION_TESTER_OPERATION_H

#include <string>
#include <functional>

#include "RecordType.h"

struct Operation {
    enum Type {
        READ, WRITE
    } type;

    RecordKey key;

    // For write operations only
    RecordData newData;
    // std::function<void (RecordData &data)> updateData; // TODO
};

#endif // _MENCI_TRANSACTION_TESTER_OPERATION_H
