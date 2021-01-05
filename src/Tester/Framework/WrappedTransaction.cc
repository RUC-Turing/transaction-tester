#include "WrappedTransaction.h"

#include "Algorithm/Transaction.h"

void WrappedTransaction::preloadData(const std::unordered_map<RecordKey, RecordData> &initialRecords) {
    ::preloadData(initialRecords);
}

std::vector<transaction_id_t> WrappedTransaction::getSerializationOrder() {
    return ::getSerializationOrder();
}

WrappedTransaction::WrappedTransaction(transaction_id_t id)
: impl(new Transaction(id)), id(id) {}

WrappedTransaction::WrappedTransaction(WrappedTransaction &&) = default;

WrappedTransaction::~WrappedTransaction() = default;

bool WrappedTransaction::read(const RecordKey &key, RecordData &result) {
    return this->impl->read(key, result);
}

bool WrappedTransaction::write(const RecordKey &key, const RecordData &newData) {
    return this->impl->write(key, newData);
}

bool WrappedTransaction::commit() {
    return this->impl->commit();
}

void WrappedTransaction::rollback() {
    this->impl->rollback();
}
