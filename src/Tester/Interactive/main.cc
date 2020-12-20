#include <iostream>
#include <future>
#include <cstdlib>

#include <TerminalColor/TerminalColor.h>

#include "Framework/TransactionRunner.h"
#include "YCSB/qry_ycsb.h"
#include "Arguments.h"

int main(int argc, char *argv[]) {
    // Parse arguments
    Arguments arguments = parseArguments(argc, argv);

    // Initialization
    threadPool = std::make_unique<ThreadPool>(arguments.threads);

    try {
        std::unordered_map<RecordKey, RecordData> initialRecords;

        // YCSB generates data
        for (size_t i = 0; i < arguments.tableSize; i++) {
            RecordKey key = i;
            // YCSB table has 10 colums
            RecordData value(arguments.fieldCount);
            value.fields[0] = std::to_string(key);
            for (size_t fid = 1; fid < arguments.fieldCount; fid++) {
				for (size_t i = 0; i < arguments.fieldLength; i++)
					value.fields[fid] += 'A' + rand() % ('Z' - 'A' + 1);
			}
            initialRecords[key] = value;
        }
        
        // Preload initial data
        TransactionRunner::preloadData(initialRecords);

        QueryGenYCSB *gen = new QueryGenYCSB;
        gen->init();
        QryYCSB *query[arguments.transactions];
        for (size_t i = 0; i < arguments.transactions; i++)
            query[i] = gen->create_query(arguments.tableSize, arguments.requestsPerTransaction);

        std::vector<std::future<bool>> futures; // A std::future is used for awaiting transaction execuation
        for (size_t i = 0; i < arguments.transactions; i++) {
            futures.push_back(TransactionRunner::runTransaction([&, i] (InteractiveTransaction &transaction) {
                RecordData readResult;
                for (size_t j = 0; j < arguments.requestsPerTransaction; j++) {
                    if (!transaction.read(query[i]->requests[j]->key, readResult)) return;
                    if (query[i]->requests[j]->acctype == WR) {
                        readResult.fields[2] += query[i]->requests[j]->value;
                        if (!transaction.write(query[i]->requests[j]->key, readResult)) return;
                    }
                }
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
