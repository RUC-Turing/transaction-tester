#include "TransactionRunner.h"

#include "Algorithm/Transaction.h"

std::future<bool> TransactionRunner::runTransaction(
    size_t id,
    const std::vector<Operation> &operations,
    ThreadPool &threadPool,
    std::vector<RecordData> &readResults
) {
    return threadPool.run([&, id] {
        Transaction transaction(id);
        
        bool failed = false;

        for (const auto &operation : operations) {
            if (operation.type == Operation::READ) {
                RecordData readResult;
                if (!transaction.read(operation.key, readResult)) {
                    failed = true;
                    break;
                }
                readResults.push_back(readResult);
            } else {
                if (!transaction.write(operation.key, operation.newData)) {
                    failed = true;
                    break;
                }
            }
        }

        if (!failed)
            failed = !transaction.commit();

        if (failed)
            transaction.rollback();

        return !failed;
    });
}
