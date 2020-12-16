#ifndef _MENCI_TRANSACTION_TESTER_TRANSACTION_RUNNER_H
#define _MENCI_TRANSACTION_TESTER_TRANSACTION_RUNNER_H

#include <fstream>
#include <vector>
#include <functional>

#include "Operation.h"
#include "ThreadPool.h"

class TransactionRunner {
public:
    static std::future<bool> runTransaction(
        size_t id,
        const std::vector<Operation> &operations,
        ThreadPool &threadPool,
        std::vector<RecordData> &readResults
    );
};

#endif // _MENCI_TRANSACTION_TESTER_TRANSACTION_RUNNER_H
