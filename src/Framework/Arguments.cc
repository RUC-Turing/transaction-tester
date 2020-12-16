#include "Arguments.h"

#include <ArgumentParser/ArgumentParser.h>

Arguments parseArguments(int argc, char *argv[]) {
    Arguments arguments;
    ArgumentParser(argc, argv)
        .setProgramDescription("Tester for transaction concurrency control algorithm implementation.")
        .addOption(
            "preload-data", "d",
            "path",
            "The file of dataset to be preloaded to the database.",
            ArgumentParser::stringParser(arguments.preloadDataFile),
            false
        )
        .addOption(
            "transactions", "t",
            "path",
            "The file of transactions to execute in the database.",
            ArgumentParser::stringParser(arguments.transactionFile),
            false
        )
        .addOption(
            "threads", "n",
            "number",
            "The number of threads executing in parallel.",
            ArgumentParser::integerParser(arguments.threads),
            true, "16"
        )
        .parse();
    return arguments;
}
