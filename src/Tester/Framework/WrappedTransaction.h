#ifndef _MENCI_TRANSACTION_TESTER_WRAPPED_TRANSACTION_H
#define _MENCI_TRANSACTION_TESTER_WRAPPED_TRANSACTION_H

#include <vector>
#include <unordered_map>
#include <memory>

#include "RecordType.h"

using transaction_id_t = size_t;

class WrappedTransaction {
    static void preloadData(const std::unordered_map<RecordKey, RecordData> &initialRecords);
    static std::vector<transaction_id_t> getSerializationOrder();

    std::unique_ptr<class Transaction> impl;

public:
    const transaction_id_t id;

    WrappedTransaction(const WrappedTransaction &) = delete;
    WrappedTransaction(WrappedTransaction &&);
    ~WrappedTransaction();

private:
    WrappedTransaction(transaction_id_t id);

    bool read(const RecordKey &key, RecordData &result);
    bool write(const RecordKey &key, const RecordData &newData);
    bool commit();
    void rollback();

    friend class InteractiveTransaction;
    friend class TransactionRunner;
};

#endif // _MENCI_TRANSACTION_TESTER_WRAPPED_TRANSACTION_H
