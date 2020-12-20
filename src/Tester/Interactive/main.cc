#include <iostream>
#include <future>

#include <TerminalColor/TerminalColor.h>

#include "Framework/TransactionRunner.h"
#include "Arguments.h"
#include "benchmark/qry_ycsb.h"
int main(int argc, char *argv[]) {
    // Parse arguments
    Arguments arguments = parseArguments(argc, argv);

    // Initialization
    threadPool = std::make_unique<ThreadPool>(arguments.threads);

    try {
        // Read initial data
        std::unordered_map<RecordKey, RecordData> initialRecords;
        // Ycsb generates data
        for (size_t i = 0; i < TABLE_SIZE; i++) {
            RecordKey key = "ycsb" + String.valueof(i);
            // ycsb table has 10 colums
            RecordData value;
            value.fields[0] = key;
            for (int fid = 1; fid < FIELD_COUNT; fid ++) {
                value.fields[i];
				int field_size = FIELD_SIZE;
				for (int i = 0; i < field_size; i++) 
					value.fields[i] += (char)rand() % (1<<8) ;
			}
            initialRecords[key] = value;
        }
        
        // Preload initial data
        TransactionRunner::preloadData(initialRecords);

        QueryGenYCSB * gen = new QueryGenYCSB;
        gen->init();
        QryYCSB query[TEST_TXN_COUNT];
        for (size_t i = 0; i < TEST_TXN_COUNT; i++)
            query[i] = gen->create_query();

        std::vector<std::future<bool>> futures; // A std::future is used for awaiting transaction execuation
        for (size_t i = 0; i < TEST_TXN_COUNT; i++) {
            futures.push_back(TransactionRunner::runTransaction([&] (InteractiveTransaction &transaction) {
                RecordData readResult;
                for (size_t j = 0; j < YCSB_REQ_PER_QUERY; j++) {
                    if (!transaction.read(query[i].requests[j].key, readResult)) return;
                    if (query[i].requests[j].acctype == WR) {
                        readResult.fields[2] += query[i].requests[j].value;
                        if (!transaction.write("test", readResult + "1")) return;
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
