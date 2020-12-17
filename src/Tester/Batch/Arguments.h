#ifndef _MENCI_TRANSACTION_TESTER_BATCH_ARGUMENTS_H
#define _MENCI_TRANSACTION_TESTER_BATCH_ARGUMENTS_H

#include <string>

struct Arguments {
    std::string preloadDataFile;
    std::string batchTransactionFile;
    size_t threads;
};

Arguments parseArguments(int argc, char *argv[]);

#endif // _MENCI_TRANSACTION_TESTER_BATCH_ARGUMENTS_H
