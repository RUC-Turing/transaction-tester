#ifndef _MENCI_TRANSACTION_TESTER_TRANSACTION_H
#define _MENCI_TRANSACTION_TESTER_TRANSACTION_H

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

#include "Framework/RecordType.h"

using transaction_id_t = size_t;

// This function is called only once, before ALL transactions
void preloadData(const std::unordered_map<RecordKey, RecordData> &records);

// This function is called only once, after ALL transactions
std::vector<size_t> getSerializationOrder();

class Transaction {
    Transaction(transaction_id_t id) : id(id) { start(); }
    friend class InteractiveTransaction;

public:
    // ID is used internally by the framework to identify the transaction, for validation
    // You should return a sequence of commited transactions' IDs, in their serialization order
    const transaction_id_t id;

    Transaction(const Transaction &) = delete;
    Transaction(Transaction &&) = default;

private:
    // NOTE: look at here!

    // Fill your transaction-specfied data here
    // e.g. transaction timestamp
    uint64_t timestamp;

    // These 5 functions below are the framework's interface
    // Return false on ANY function means the transaction should ROLLBACK
    void start();
    bool read(const RecordKey &key, RecordData &result);
    bool write(const RecordKey &key, const RecordData &newData);
    bool commit();
    void rollback();

    // You can declare more utility functions to reuse your code
    void cleanUp();
};

#endif // _MENCI_TRANSACTION_TESTER_TRANSACTION_H
