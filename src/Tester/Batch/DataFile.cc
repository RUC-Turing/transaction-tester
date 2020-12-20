#include "DataFile.h"

#include <stdexcept>

DataFile::DataFile(const std::string &filePath) : filePath(filePath), fileStream(filePath) {
    if (!fileStream.is_open())
        throw std::system_error(errno, std::system_category(), "Failed to open data file " + filePath + " for read");
}

void DataFile::readAll(ReadRecordCallback callback) {
    fileStream.clear();
    fileStream.seekg(0);

    size_t n;
    if (!(fileStream >> n))
        throw std::runtime_error("Failed to read data file " + filePath + ": couldn't determine record count");
    
    for (size_t i = 1; i <= n; i++) {
        uint64_t key;
        std::string value;
        if (!(fileStream >> key >> value))
            throw std::runtime_error("Failed to read data file " + filePath + ": failed to read " + std::to_string(i) + "-th record");
        callback(std::move(key), std::move(value));
    }

    // Check extra input
    std::string s;
    fileStream >> s;
    if (!s.empty())
        throw std::runtime_error("Extra contents in the end of data file " + filePath + " :'" + s + "'");
}
