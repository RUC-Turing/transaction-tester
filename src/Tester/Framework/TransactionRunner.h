#ifndef _MENCI_TRANSACTION_TESTER_FRAMEWORK_TRANSACTION_RUNNER_H
#define _MENCI_TRANSACTION_TESTER_FRAMEWORK_TRANSACTION_RUNNER_H

#include <unordered_map>
#include <functional>
#include <future>
#include <memory>
#include <chrono>

#include "Operation.h"
#include "ThreadPool.h"
#include "InteractiveTransaction.h"

class TransactionRunner {
    static std::unordered_map<RecordKey, RecordData> initialRecords;

    static std::vector<std::shared_ptr<std::vector<Operation>>> operationsByTransaction;
    static std::mutex lockForOperationsByTransaction;

    // InteractiveTransaction will add its id to this list when committing
    static std::vector<transaction_id_t> committedTransactions;
    static std::mutex lockForCommittedTransactions;

    // Time on first transaction created
    static std::chrono::high_resolution_clock::time_point startTime;

    static transaction_id_t allocateTransactionId();

    friend class InteractiveTransaction;

public:
    static void preloadData(const std::unordered_map<RecordKey, RecordData> &initialRecords);

    static InteractiveTransaction createTransaction();

    // Only ONE of two functions below could be called
    static std::future<bool> runTransaction(std::function<void (InteractiveTransaction &transaction)> transactionUser);
    // XXX: `operationsByTransaction` will be duplicated
    static void runBatchTransactions(const std::vector<std::vector<Operation>> &operationsByTransaction);

    static void validateAndPrintStatistics();
};

#endif // _MENCI_TRANSACTION_TESTER_FRAMEWORK_TRANSACTION_RUNNER_H
