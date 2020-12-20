#ifndef _MENCI_TRANSACTION_TESTER_FRAMEWORK_RECORD_TYPE
#define _MENCI_TRANSACTION_TESTER_FRAMEWORK_RECORD_TYPE

#include <string>


#define TABLE_SIZE 10000
#define TEST_TXN_COUNT 10
#define YCSB_REQ_PER_QUERY 10
using RecordKey = std::string;

// record has 10 fields, and every fields's size are 100 bytes.
#define FIELD_SIZE 100
#define FIELD_COUNT 10
typedef struct RecordData {
    std::string fields[FIELD_COUNT];
} RecordData;

#endif // _MENCI_TRANSACTION_TESTER_FRAMEWORK_RECORD_TYPE
