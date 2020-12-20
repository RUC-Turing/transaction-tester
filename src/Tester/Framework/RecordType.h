#ifndef _MENCI_TRANSACTION_TESTER_FRAMEWORK_RECORD_TYPE
#define _MENCI_TRANSACTION_TESTER_FRAMEWORK_RECORD_TYPE

#include <cstdint>
#include <string>
#include <vector>

using RecordKey = uint64_t;

struct RecordData {
    std::vector<std::string> fields;

    RecordData(size_t fieldCount = 0) : fields(fieldCount) {}
    RecordData(const std::string &string) : fields(1) { fields[0] = string; }

    bool operator!=(const RecordData &other) const {
        if (this->fields.size() != other.fields.size()) return true;
        for (size_t i = 0; i < fields.size(); i++)
            if (this->fields[i] != other.fields[i]) return true;
        return false;
    }

    operator std::string() const {
        std::string result;
        for (size_t i = 0; i < fields.size(); i++) {
            result += fields[i];
            if (i != fields.size() - 1) result += ",";
        }
        return result;
    }
};

#endif // _MENCI_TRANSACTION_TESTER_FRAMEWORK_RECORD_TYPE
