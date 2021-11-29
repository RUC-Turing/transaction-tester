#include "Transaction.h"

#include <cstddef>
#include <unordered_map>
#include <shared_mutex>
#include <atomic>

struct RecordItem {
    RecordData data;
};

static std::unordered_map<RecordKey, RecordItem> storage;

static std::vector<transaction_id_t> serializationOrder;
static std::mutex serializationOrderLock;

void preloadData(const std::unordered_map<RecordKey, RecordData> &initialRecords) {
    for (auto &[key, data] : initialRecords) {
        storage[key].data = data;
    }
}

std::vector<transaction_id_t> getSerializationOrder() {
    return serializationOrder;
}

void Transaction::start() {
    timestamp = getTimestamp();
}

bool Transaction::read(const RecordKey &key, RecordData &result) {
    result = storage[key].data;

    return true;
}

bool Transaction::write(const RecordKey &key, const RecordData &newData) {
    storage[key].data = newData;

    return true;
}

bool Transaction::commit() {
    // Append the commited transaction to serialization order list
    {
        std::lock_guard lock(serializationOrderLock);
        serializationOrder.push_back(this->id);
    }

    // "return true" means successfully commited
    return true;
}

void Transaction::rollback() {
    // It won't rollback
}
