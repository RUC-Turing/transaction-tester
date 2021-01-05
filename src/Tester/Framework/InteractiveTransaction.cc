#include "InteractiveTransaction.h"

#include <string>
#include <stdexcept>
#include <mutex>

#include "TransactionRunner.h"

InteractiveTransaction::InteractiveTransaction(size_t id, std::shared_ptr<std::vector<Operation>> operations)
: transaction(id), status(RUNNING), operations(operations) {}

void InteractiveTransaction::ensureRunning(const char *methodName) {
    if (status == RUNNING) return;

    std::string statusString;
    if (status == ROLLED_BACK) statusString = "rolled-back";
    else statusString = "commited";
    
    throw std::logic_error(methodName + ("() called on a " + statusString + " transaction " + std::to_string(transaction.id)));
}

bool InteractiveTransaction::read(const RecordKey &key, RecordData &result) {
    ensureRunning("read");

    if (!transaction.read(key, result)) {
        rollback();
        return false;
    } else {
        operations->push_back({Operation::READ, key, result});
        return true;
    }
}

bool InteractiveTransaction::write(const RecordKey &key, const RecordData &newData) {
    ensureRunning("write");

    operations->push_back({Operation::WRITE, key, newData});
    if (!transaction.write(key, newData)) {
        rollback();
        return false;
    } else
        return true;
}

bool InteractiveTransaction::commit() {
    ensureRunning("commit");
    
    if (transaction.commit()) {
        status = COMMITED;

        // Append to commited transaction list
        std::lock_guard lock(TransactionRunner::lockForCommittedTransactions);
        TransactionRunner::committedTransactions.push_back(transaction.id);

        return true;
    } else {
        rollback();
        return false;
    }
}

void InteractiveTransaction::rollback() {
    transaction.rollback();
    status = ROLLED_BACK;
}
