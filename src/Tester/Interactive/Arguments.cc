#include "Arguments.h"

#include <ArgumentParser/ArgumentParser.h>

Arguments parseArguments(int argc, char *argv[]) {
    Arguments arguments;
    ArgumentParser(argc, argv)
        .setProgramDescription("Interactive tester for transaction concurrency control algorithm implementation.")
        .addOption(
            "table-size", "s",
            "number",
            "The record count of generated being tested dataset.",
            ArgumentParser::integerParser(arguments.tableSize),
            true, "10000"
        )
        .addOption(
            "field-count", "f",
            "number",
            "The number of fields in each record",
            ArgumentParser::integerParser(arguments.fieldCount),
            true, "10"
        )
        .addOption(
            "field-length", "l",
            "number",
            "The initial length of each field's value",
            ArgumentParser::integerParser(arguments.fieldLength),
            true, "10"
        )
        .addOption(
            "transactions", "t",
            "number",
            "The number of transactions being tested.",
            ArgumentParser::integerParser(arguments.transactions),
            true, "10000"
        )
        .addOption(
            "requests-per-transaction", "r",
            "number",
            "The number of request in each transaction being tested.",
            ArgumentParser::integerParser(arguments.requestsPerTransaction),
            true, "10"
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
