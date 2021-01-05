#include "Arguments.h"

#include <ArgumentParser/ArgumentParser.h>

Arguments parseArguments(int argc, char *argv[]) {
    Arguments arguments;
    ArgumentParser(argc, argv)
        .setProgramDescription("Conflict tester for transaction concurrency control algorithm implementation.")
        .addOption(
            "conflict-transactions", "t",
            "path",
            "The file of conflict transactions to execute in the database.",
            ArgumentParser::stringParser(arguments.conflictTransactionFile),
            false
        )
        .parse();
    return arguments;
}
