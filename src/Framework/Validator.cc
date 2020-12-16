#include "Validator.h"

#include <stdexcept>

Validator::Validator(const std::unordered_map<RecordKey, RecordData> &records) : records(records) {}

void Validator::validateTransaction(size_t id, const std::vector<Operation> &operations, const std::vector<RecordData> &readResults) {
    size_t i = 0;

    for (auto &operation : operations) {
        if (operation.type == Operation::WRITE)
            records[operation.key] = operation.newData;
            // operation.updateData(records[operation.key]);
        else {
            auto &correctReadResult = records[operation.key];
            if (correctReadResult != readResults[i]) {
                throw std::runtime_error(
                    "The " + std::to_string(i + 1) + "-th read result in transaction " + std::to_string(id) + " is wrong: " + 
                    "expected '" + correctReadResult + "' but got '" + readResults[i] + "'"
                );
            }
        }
    }
}
