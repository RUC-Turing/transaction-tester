#include "BatchTransactionFile.h"

#include <stdexcept>

BatchTransactionFile::BatchTransactionFile(const std::string &filePath) : filePath(filePath), fileStream(filePath) {
    if (!fileStream.is_open())
        throw std::system_error(errno, std::system_category(), "Failed to open transaction file " + filePath + " for read");
}

std::vector<std::vector<Operation>> BatchTransactionFile::readAll() {
    fileStream.clear();
    fileStream.seekg(0);

    size_t transactionCount;
    if (!(fileStream >> transactionCount))
        throw std::runtime_error("Failed to read transaction file " + filePath + ": couldn't determine transaction count");
    
    std::vector<std::vector<Operation>> result;
    for (size_t i = 1; i <= transactionCount; i++) {
        size_t operationCount;
        if (!(fileStream >> operationCount))
            throw std::runtime_error("Failed to read transaction file " + filePath + ": couldn't determine operation count of " + std::to_string(i) + "-th transaction");

        std::vector<Operation> operations(operationCount);
        for (size_t j = 1; j <= operationCount; j++) {
            Operation &operation = operations[j - 1];

            std::string type;
            if (!(fileStream >> type))
                throw std::runtime_error("Failed to read transaction file " + filePath + ": failed to read type of " + std::to_string(i) + "-th transaction's " + std::to_string(j) + "-th operation");

            if (type == "READ") operation.type = Operation::READ;
            else if (type == "WRITE") operation.type = Operation::WRITE;
            else
                throw std::runtime_error("Failed to read transaction file " + filePath + ": invalid type '" + type + "' on " + std::to_string(i) + "-th transaction's " + std::to_string(j) + "-th operation");

            if (!(fileStream >> operation.key))
                throw std::runtime_error("Failed to read transaction file " + filePath + ": failed to read key of " + std::to_string(i) + "-th transaction's " + std::to_string(j) + "-th operation");

            if (operation.type == Operation::WRITE) {
                std::string newData;
                if (!(fileStream >> newData))
                    throw std::runtime_error("Failed to read transaction file " + filePath + ": failed to read new data of " + std::to_string(i) + "-th transaction's " + std::to_string(j) + "-th operation");

                operation.newDataOrReadResult = newData;
            }
        }

        result.push_back(std::move(operations));
    }

    // Check extra input
    std::string s;
    fileStream >> s;
    if (!s.empty())
        throw std::runtime_error("Extra contents in the end of transaction file " + filePath + " :'" + s + "'");

    return result;
}
