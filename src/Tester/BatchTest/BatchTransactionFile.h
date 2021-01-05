#ifndef _MENCI_TRANSACTION_TESTER_BATCH_BATCH_TRANSACTION_FILE_H
#define _MENCI_TRANSACTION_TESTER_BATCH_BATCH_TRANSACTION_FILE_H

#include <fstream>
#include <vector>

#include "Framework/Operation.h"

class BatchTransactionFile {
    std::string filePath;
    std::ifstream fileStream;

public:
    BatchTransactionFile(const std::string &filePath);

    std::vector<std::vector<Operation>> readAll();
};

#endif // _MENCI_TRANSACTION_TESTER_BATCH_BATCH_TRANSACTION_FILE_H
