#include "Arguments.h"

#include <ArgumentParser/ArgumentParser.h>

Arguments parseArguments(int argc, char *argv[]) {
    Arguments arguments;
    ArgumentParser(argc, argv)
        .setProgramDescription("Interactive tester for transaction concurrency control algorithm implementation.")
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
