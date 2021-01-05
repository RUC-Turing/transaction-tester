#ifndef _MENCI_TRANSACTION_TESTER_CONFLICT_CONFLICT_TRANSACTION_FILE_H
#define _MENCI_TRANSACTION_TESTER_CONFLICT_CONFLICT_TRANSACTION_FILE_H

#include <fstream>
#include <vector>

#include "Framework/Operation.h"

struct ConflictGroup {
    struct Operation {
        // This id is defined in the input file, not the same as id of running transaction
        size_t transactionId;

        enum Type {
            READ, WRITE, COMMIT
        } type;

        RecordKey key;
        RecordData newDataOrReadResult;
    };

    std::vector<Operation> operations;
    size_t maxTransactionId;
    size_t maxKey;

    ConflictGroup() : maxTransactionId(0), maxKey(0) {}
};

class ConflictTransactionFile {
    std::string filePath;
    std::ifstream fileStream;

public:
    ConflictTransactionFile(const std::string &filePath);

    std::vector<ConflictGroup> readAll();
};

#endif // _MENCI_TRANSACTION_TESTER_CONFLICT_CONFLICT_TRANSACTION_FILE_H
