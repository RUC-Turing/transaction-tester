#ifndef _MENCI_TRANSACTION_TESTER_ARGUMENTS_H
#define _MENCI_TRANSACTION_TESTER_ARGUMENTS_H

#include <string>

struct Arguments {
    std::string preloadDataFile;
    std::string transactionFile;
    size_t threads;
};

Arguments parseArguments(int argc, char *argv[]);

#endif // _MENCI_TRANSACTION_TESTER_ARGUMENTS_H
