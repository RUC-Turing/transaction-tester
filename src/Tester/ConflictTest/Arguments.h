#ifndef _MENCI_TRANSACTION_TESTER_CONFLICT_ARGUMENTS_H
#define _MENCI_TRANSACTION_TESTER_CONFLICT_ARGUMENTS_H

#include <string>

struct Arguments {
    std::string conflictTransactionFile;
};

Arguments parseArguments(int argc, char *argv[]);

#endif // _MENCI_TRANSACTION_TESTER_CONFLICT_ARGUMENTS_H
