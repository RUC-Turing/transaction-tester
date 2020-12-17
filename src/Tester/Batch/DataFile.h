#ifndef _MENCI_TRANSACTION_TESTER_BATCH_DATAFILE_H
#define _MENCI_TRANSACTION_TESTER_BATCH_DATAFILE_H

#include <fstream>
#include <functional>

#include "Framework/RecordType.h"

using ReadRecordCallback = std::function<void (RecordKey &&key, RecordData &&value)>;

class DataFile {
    std::string filePath;
    std::ifstream fileStream;

public:
    DataFile(const std::string &filePath);

    void readAll(ReadRecordCallback callback);
};

#endif // _MENCI_TRANSACTION_TESTER_BATCH_DATAFILE_H
