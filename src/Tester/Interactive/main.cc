#include <iostream>
#include <future>

#include <TerminalColor/TerminalColor.h>

#include "Framework/TransactionRunner.h"
#include "Arguments.h"

int main(int argc, char *argv[]) {
    // Parse arguments
    Arguments arguments = parseArguments(argc, argv);

    // Initialization
    threadPool = std::make_unique<ThreadPool>(arguments.threads);

    try {
        // Read initial data
        std::unordered_map<RecordKey, RecordData> initialRecords;

        initialRecords["test"] = "test1";

        // Preload initial data
        TransactionRunner::preloadData(initialRecords);

        std::vector<std::future<bool>> futures; // A std::future is used for awaiting transaction execuation
        for (size_t i = 0; i < 10; i++) {
            futures.push_back(TransactionRunner::runTransaction([] (InteractiveTransaction &transaction) {
                RecordData readResult;
                if (!transaction.read("test", readResult)) return;
                if (!transaction.write("test", readResult + "1")) return;
                transaction.commit();
            }));
        }

        // Wait all transactions to finish
        for (auto &future : futures)
            future.wait();

        // Validate
        TransactionRunner::validateAndPrintStatistics();
    } catch (const std::runtime_error &e) {
        std::cout << TerminalColor::ForegroundRed << TerminalColor::Bold << "Error: " << TerminalColor::Reset
                  << e.what()
                  << std::endl;
        return 1;
    }
}
