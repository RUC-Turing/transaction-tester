#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>

#include <TerminalColor/TerminalColor.h>

#include "Framework/TransactionRunner.h"
#include "Arguments.h"
#include "ConflictTransactionFile.h"

void runConflictGroup(const ConflictGroup &group) {
    try {
        // Read initial data
        std::unordered_map<RecordKey, RecordData> initialRecords;

        size_t currentValue = 0;
        for (size_t i = 0; i <= group.maxKey; i++)
            initialRecords[i] = std::to_string(++currentValue);

        // Preload initial data
        TransactionRunner::preloadData(initialRecords);

        // Run transactions
        std::vector<std::unique_ptr<InteractiveTransaction>> transactions(group.maxTransactionId + 1);
        for (const auto &operation : group.operations) {
            auto &transaction = transactions[operation.transactionId];

            if (!transaction) transaction = std::make_unique<InteractiveTransaction>(TransactionRunner::createTransaction());

            if (transaction->getStatus() == InteractiveTransaction::ROLLED_BACK) continue;

            if (operation.type == ConflictGroup::Operation::READ) {
                RecordData data;
                transaction->read(operation.key, data);
            } else if (operation.type == ConflictGroup::Operation::WRITE) {
                transaction->write(operation.key, std::to_string(++currentValue));
            } else {
                transaction->commit();
            }
        }

        // Validate
        TransactionRunner::validateAndPrintStatistics();
    } catch (const std::runtime_error &e) {
        std::cout << TerminalColor::ForegroundRed << TerminalColor::Bold << "Error: " << TerminalColor::Reset
                  << e.what()
                  << std::endl;
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    // Parse arguments
    Arguments arguments = parseArguments(argc, argv);

    // Read transactions
    ConflictTransactionFile conflictTransactionFile(arguments.conflictTransactionFile);
    auto conflictGroups = conflictTransactionFile.readAll();

    size_t i = 0;
    for (const auto &group : conflictGroups) {
        ++i;

        pid_t pid = fork();

        if (pid != 0) {
            int status;
            do waitpid(pid, &status, 0); while (!WIFEXITED(status));
        } else {
            std::cout << TerminalColor::ForegroundBlue
                      << "[" << i << "/" << conflictGroups.size() << "]"
                      << TerminalColor::Reset
                      << " ";
            runConflictGroup(group);
            exit(0);
        }
    }
}
