#include "Validator.h"

#include <stdexcept>

Validator::Validator(const std::unordered_map<RecordKey, RecordData> &records) : records(records) {}

void Validator::validateTransaction(size_t id, const std::vector<Operation> &operations) {
    size_t i = 0;

    for (auto &operation : operations) {
        if (operation.type == Operation::WRITE)
            records[operation.key] = operation.newDataOrReadResult;
        else {
            auto &correctReadResult = records[operation.key];
            if (correctReadResult != operation.newDataOrReadResult) {
                throw std::runtime_error(
                    "The " + std::to_string(i + 1) + "-th read result in transaction " + std::to_string(id) + " is wrong:\n" + 
                    "                expected '" + (std::string)correctReadResult + "'\n" +
                    "                 but got '" + (std::string)operation.newDataOrReadResult + "'"
                );
            }
        }
    }
}
