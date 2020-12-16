#include "Transaction.h"

#include <cstddef>
#include <unordered_map>
#include <mutex>
#include <iostream>
#include <list>

#include "Framework/Utility.h"

struct DataVersion {
    // A verion's valid timestamp interval
    // [begin-ts, end-ts)
    timestamp_t beginTimestamp, endTimestamp;

    // The data of the version
    RecordData data;
};

struct MultiVersionData {
    // When accessing a linked-list from multiple threads in parallel, the list must be locked
    // NOTE: You may want to implement a lock-free linked-list to reduce overhead of the lock
    std::mutex listLock;

    // Here is the list of ALL existing versions of the record's data
    // NOTE: You may replace linked-list with other data structures, e.g. ordered set
    std::list<DataVersion> versionsList;
};

// A key to a list of versions
static std::unordered_map<RecordKey, MultiVersionData> storage;

// A list of commited transaction IDs, in their serialization order
static std::vector<transaction_id_t> serializationOrder;
static std::mutex serializationOrderLock;

// This function is called only once, before ALL transactions
void preloadData(const std::unordered_map<RecordKey, RecordData> &records) {
    for (auto &[key, data] : records) {
        DataVersion version;
        version.beginTimestamp = 1;
        version.endTimestamp = TIMESTAMP_INF;
        version.data = data;

        // You could NOT write "storage[key] = multiVersion" since std::mutex is NOT copy constructible
        storage[key].versionsList.push_back(version);
    }
}

// This function is called only once, after ALL transactions
std::vector<size_t> getSerializationOrder() {
    return serializationOrder;
}

void Transaction::start() {
    // Acquire the start timestamp of the transaction
    // getTimestamp() will return the number you have called it as a timestamp, starting from 1
    timestamp = getTimestamp();
}

bool Transaction::read(const RecordKey &key, RecordData &result) {
    // Get the list of versions of the record from the storage
    MultiVersionData &multiVersion = storage[key];

    // Lock the list so you can access it safely
    std::lock_guard guard(multiVersion.listLock);

    for (DataVersion &version : multiVersion.versionsList) {
        if (this->timestamp >= version.beginTimestamp && this->timestamp < version.endTimestamp) {
            // Get the valid version of data in the current transaction
            result = version.data;
            return true;
        }
    }

    // The requested record always exists so this should happen
    // You can do something to help you debugging
    throw std::logic_error("read: couldn't find any valid version");
}

bool Transaction::write(const RecordKey &key, const RecordData &newData) {
    MultiVersionData &multiVersion = storage[key];
    std::lock_guard guard(multiVersion.listLock);

    // 1.
    // Consider if the record has been written by current transaction
    // Try to find the version created by current transaction
    for (DataVersion &version : multiVersion.versionsList)
        if (version.beginTimestamp == this->timestamp) {
            version.data = newData;
            return true;
        }

    // 2.

    // Update the current latest version's end-ts
    for (DataVersion &version : multiVersion.versionsList)
        if (version.endTimestamp == TIMESTAMP_INF) {
            version.endTimestamp = this->timestamp;
            break;
        }

    // Create a new version
    DataVersion newVersion;
    newVersion.beginTimestamp = this->timestamp;
    newVersion.endTimestamp = TIMESTAMP_INF;
    newVersion.data = newData;
    multiVersion.versionsList.push_back(newVersion);

    return true;
}

bool Transaction::commit() {
    // Validate the transaction
    // If you think this transaction couldn't be commited, "return false" to let the framework rollback it
    // Do NOT call rollback() by yourself

    // Do commit

    // Append the commited transaction to serialization order list
    {
        std::lock_guard lock(serializationOrderLock);
        serializationOrder.push_back(this->id);
    }

    // "return true" means successfully commited
    return true;
}

void Transaction::rollback() {
    // If any of your "read", "write" and "commit" functions returned false in current transaction
    // The rollback() function is called immediately
}

void Transaction::cleanUp() {
    // This is NOT a part of interface, but you can write transaction clean-up related code in a function to reuse your code
    // You should call it by yourself
}
