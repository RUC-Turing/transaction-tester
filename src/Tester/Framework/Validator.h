#ifndef _MENCI_TRANSACTION_TESTER_FRAMEWORK_VALIDATOR_H
#define _MENCI_TRANSACTION_TESTER_FRAMEWORK_VALIDATOR_H

#include <unordered_map>
#include <vector>

#include "Operation.h"

class Validator {
    std::unordered_map<RecordKey, RecordData> records;

public:
    Validator(const std::unordered_map<RecordKey, RecordData> &records);

    void validateTransaction(size_t id, const std::vector<Operation> &operations);
};

#endif // _MENCI_TRANSACTION_TESTER_FRAMEWORK_VALIDATOR_H
