#include <iostream>

#include <TerminalColor/TerminalColor.h>

#include "Framework/TransactionRunner.h"
#include "Arguments.h"
#include "DataFile.h"
#include "BatchTransactionFile.h"

int main(int argc, char *argv[]) {
    // Parse arguments
    Arguments arguments = parseArguments(argc, argv);

    // Initialization
    threadPool = std::make_unique<ThreadPool>(arguments.threads);

    try {
        // Read initial data
        std::unordered_map<RecordKey, RecordData> initialRecords;

        DataFile dataFile(arguments.preloadDataFile);
        dataFile.readAll([&] (RecordKey &&key, RecordData &&data) {
            initialRecords[std::move(key)] = std::move(data);
        });

        // Preload initial data
        TransactionRunner::preloadData(initialRecords);

        // Read transactions
        BatchTransactionFile batchTransactionFile(arguments.batchTransactionFile);
        auto operationsByTransaction = batchTransactionFile.readAll();

        // Run transactions
        TransactionRunner::runBatchTransactions(operationsByTransaction);

        // Validate
        TransactionRunner::validateAndPrintStatistics();
    } catch (const std::runtime_error &e) {
        std::cout << TerminalColor::ForegroundRed << TerminalColor::Bold << "Error: " << TerminalColor::Reset
                  << e.what()
                  << std::endl;
        return 1;
    }
}
