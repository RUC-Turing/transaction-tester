#ifndef _MENCI_TRANSACTION_TESTER_TRANSACTION_FILE_H
#define _MENCI_TRANSACTION_TESTER_TRANSACTION_FILE_H

#include <fstream>
#include <vector>
#include <functional>

#include "Operation.h"

class TransactionFile {
    std::string filePath;
    std::ifstream fileStream;

public:
    TransactionFile(const std::string &filePath);

    std::vector<std::vector<Operation>> readAll();
};

#endif // _MENCI_TRANSACTION_TESTER_TRANSACTION_FILE_H
