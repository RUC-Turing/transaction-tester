#ifndef _MENCI_TRANSACTION_TESTER_FRAMEWORK_RECORD_TYPE
#define _MENCI_TRANSACTION_TESTER_FRAMEWORK_RECORD_TYPE

#include <string>
#include <cstdint>

using RecordKey = uint64_t;

#define TABLE_SIZE 10000
#define TEST_TXN_COUNT 10
#define YCSB_REQ_PER_QUERY 10

// Record has 10 fields, and each field's size is 100 bytes
#define FIELD_SIZE 10
#define FIELD_COUNT 10

struct RecordData {
    size_t fieldCount;
    std::string fields[FIELD_COUNT];

    RecordData() : fieldCount(FIELD_COUNT) {}
    RecordData(const std::string &string) : fieldCount(1) { fields[0] = string; }

    bool operator!=(const RecordData &other) const {
        if (this->fieldCount != other.fieldCount) return true;
        for (size_t i = 0; i < fieldCount; i++)
            if (this->fields[i] != other.fields[i]) return true;
        return false;
    }

    operator std::string() const {
        std::string result;
        for (size_t i = 0; i < fieldCount; i++) {
            result += fields[i];
            if (i != fieldCount - 1) result += ",";
        }
        return result;
    }
};

#endif // _MENCI_TRANSACTION_TESTER_FRAMEWORK_RECORD_TYPE
