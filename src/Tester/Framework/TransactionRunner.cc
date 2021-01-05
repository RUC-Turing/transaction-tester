#include "TransactionRunner.h"

#include <unordered_set>
#include <iostream>

#include <TerminalColor/TerminalColor.h>

#include "Validator.h"

std::unordered_map<RecordKey, RecordData> TransactionRunner::initialRecords;

std::vector<std::shared_ptr<std::vector<Operation>>> TransactionRunner::operationsByTransaction;
std::mutex TransactionRunner::lockForOperationsByTransaction;

std::vector<transaction_id_t> TransactionRunner::committedTransactions;
std::mutex TransactionRunner::lockForCommittedTransactions;

std::chrono::high_resolution_clock::time_point TransactionRunner::startTime;

void TransactionRunner::preloadData(const std::unordered_map<RecordKey, RecordData> &initialRecords) {
    TransactionRunner::initialRecords = initialRecords;
    WrappedTransaction::preloadData(initialRecords);
}

transaction_id_t TransactionRunner::allocateTransactionId() {
    std::lock_guard lock(lockForOperationsByTransaction);
    if (operationsByTransaction.empty()) {
        // Record the start time
        startTime = std::chrono::high_resolution_clock::now();
    }
    operationsByTransaction.push_back(std::make_shared<std::vector<Operation>>());
    return operationsByTransaction.size();
}

InteractiveTransaction TransactionRunner::createTransaction() {
    transaction_id_t id = allocateTransactionId();
    return InteractiveTransaction(id, operationsByTransaction[id - 1]);
}

std::future<bool> TransactionRunner::runTransaction(std::function<void (InteractiveTransaction &transaction)> transactionUser) {
    transaction_id_t id = allocateTransactionId();
    std::shared_ptr<std::vector<Operation>> operations = operationsByTransaction[id - 1];

    return threadPool->run([operations, id, transactionUser{std::move(transactionUser)}] {
        InteractiveTransaction transaction(id, operations);

        transactionUser(transaction);

        if (transaction.status == InteractiveTransaction::RUNNING)
            throw std::logic_error("Transaction " + std::to_string(id) + "'s user function returned without committing");

        return transaction.status == InteractiveTransaction::COMMITED;
    });
}

void TransactionRunner::runBatchTransactions(const std::vector<std::vector<Operation>> &operationsByTransaction) {
    std::vector<std::future<bool>> futures(operationsByTransaction.size());
    for (size_t i = 0; i < operationsByTransaction.size(); i++)
        futures[i] = TransactionRunner::runTransaction([&, i] (InteractiveTransaction &transaction) {
            for (const auto &operation : operationsByTransaction[i])
                if (operation.type == Operation::READ) {
                    RecordData readResult;
                    if (!transaction.read(operation.key, readResult))
                        return;
                } else {
                    if (!transaction.write(operation.key, operation.newDataOrReadResult))
                        return;
                }
            transaction.commit();
        });

    for (auto &future : futures)
        future.wait();
}

void TransactionRunner::validateAndPrintStatistics() {
    auto endTime = std::chrono::high_resolution_clock::now();
    double timeElapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count(),
           timeElapsedInSeconds = timeElapsed / 1000000000;

    const auto &serializationOrder = WrappedTransaction::getSerializationOrder();

    std::unordered_set<transaction_id_t> committedTransactionSet(committedTransactions.begin(), committedTransactions.end());

    // Validate serialization order
    if (serializationOrder.size() != committedTransactionSet.size())
        throw std::runtime_error(
            "Wrong serialization order: list size (" + std::to_string(serializationOrder.size()) + ") different than commited transactions (" + std::to_string(committedTransactionSet.size()) + ")"
        );

    for (size_t id : serializationOrder)
        if (committedTransactionSet.count(id) == 0)
            throw std::runtime_error("Wrong serialization order: rolled back transaction " + std::to_string(serializationOrder.size()) + " appears in the list");

    // Validate read results
    Validator validator(initialRecords);
    for (size_t id : serializationOrder)
        validator.validateTransaction(id, *operationsByTransaction[id - 1]);

    std::cout << TerminalColor::ForegroundGreen << TerminalColor::Bold << "Success: " << TerminalColor::Reset
              << TerminalColor::Bold << committedTransactionSet.size() << TerminalColor::Reset
              << "/"
              << TerminalColor::Bold << operationsByTransaction.size() << TerminalColor::Reset
              << " commited in "
              << TerminalColor::Bold << std::fixed << timeElapsedInSeconds << TerminalColor::Reset
              << " seconds, "
              << TerminalColor::ForegroundBlue << TerminalColor::Bold << committedTransactionSet.size() / timeElapsedInSeconds << TerminalColor::Reset
              << " TPS."
              << std::endl;
} 
