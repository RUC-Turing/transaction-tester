#include "ConflictTransactionFile.h"

#include <stdexcept>
#include <sstream>

ConflictTransactionFile::ConflictTransactionFile(const std::string &filePath) : filePath(filePath), fileStream(filePath) {
    if (!fileStream.is_open())
        throw std::system_error(errno, std::system_category(), "Failed to open transaction file " + filePath + " for read");
}

std::vector<ConflictGroup> ConflictTransactionFile::readAll() {
    fileStream.clear();
    fileStream.seekg(0);

    std::vector<ConflictGroup> result;
    size_t i = 0;
    for (std::string line; std::getline(fileStream, line); ) {
        i++;
        if (line.empty()) continue;

        std::istringstream lineStream(line);

        ConflictGroup group;
        size_t j = 0;
        for (std::string operationText; lineStream >> operationText; ) {
            j++;

            if (operationText[0] == 'R' || operationText[0] == 'W') {
                size_t transactionId = std::stoul(operationText.substr(1, operationText.length() - 2));
                char keyChar = operationText.back();
                if (!(keyChar >= 'a' && keyChar <= 'z'))
                    throw std::runtime_error("Failed to read transaction file " + filePath + ": invalid key '" + keyChar + "' on " + std::to_string(i) + "-th line's " + std::to_string(i) + "-th operation");
                size_t key = keyChar - 'a';
                
                group.maxKey = std::max(group.maxKey, key);
                group.maxTransactionId = std::max(group.maxTransactionId, transactionId);
                
                ConflictGroup::Operation operation;
                operation.transactionId = transactionId;
                operation.type = operationText[0] == 'R' ? ConflictGroup::Operation::READ : ConflictGroup::Operation::WRITE;
                operation.key = key;
                operation.newDataOrReadResult = std::to_string(j);

                group.operations.push_back(std::move(operation));
            } else if (operationText[0] == 'A' || operationText[0] == 'C') {
                size_t transactionId = std::stoul(operationText.substr(1, operationText.length() - 1));

                ConflictGroup::Operation operation;
                group.maxTransactionId = std::max(group.maxTransactionId, transactionId);
                operation.transactionId = transactionId;
                operation.type = ConflictGroup::Operation::COMMIT;

                group.operations.push_back(std::move(operation));
            } else
                throw std::runtime_error("Failed to read transaction file " + filePath + ": invalid operation '" + operationText[0] + "' of " + std::to_string(i) + "-th line's " + std::to_string(i) + "-th operation");
        }

        result.push_back(std::move(group));
    }

    return result;
}
