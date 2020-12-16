#include <unordered_set>
#include <iostream>

#include "Arguments.h"
#include "DataFile.h"
#include "TransactionFile.h"
#include "TransactionRunner.h"
#include "Validator.h"

#include "Algorithm/Transaction.h"

int main(int argc, char *argv[]) {
    // Parse arguments
    Arguments arguments = parseArguments(argc, argv);

    try {
        // Read initial data
        std::unordered_map<RecordKey, RecordData> initialData;

        DataFile dataFile(arguments.preloadDataFile);
        dataFile.readAll([&] (RecordKey &&key, RecordData &&data) {
            initialData[std::move(key)] = std::move(data);
        });

        // Preload initial data
        preloadData(initialData);

        // Read transactions
        TransactionFile transactionFile(arguments.transactionFile);
        auto allTransactionOperations = transactionFile.readAll();

        // Run transactions
        ThreadPool threadPool(arguments.threads);

        std::vector<std::future<bool>> futures(allTransactionOperations.size());
        std::vector<std::vector<RecordData>> allTransactionReadResults(allTransactionOperations.size());
        for (size_t i = 0; i < allTransactionOperations.size(); i++) {
            const auto &transactionOperations = allTransactionOperations[i];
            futures[i] = TransactionRunner::runTransaction(
                i,
                transactionOperations,
                threadPool,
                allTransactionReadResults[i]
            );
        }

        // Validation
        std::unordered_set<size_t> commitedTransactionIDs;
        for (size_t i = 0; i < allTransactionOperations.size(); i++) {
            auto &future = futures[i];

            future.wait();
            if (future.get() == true)
                commitedTransactionIDs.insert(i);
        }

        const auto &serializationOrder = getSerializationOrder();

        if (serializationOrder.size() != commitedTransactionIDs.size())
            throw std::runtime_error("Wrong serialization order: list size (" + std::to_string(serializationOrder.size()) + ") different than commited transactions (" + std::to_string(commitedTransactionIDs.size()) + ")");

        for (size_t id : serializationOrder)
            if (commitedTransactionIDs.count(id) == 0)
                throw std::runtime_error("Wrong serialization order: rolled back transaction " + std::to_string(serializationOrder.size()) + " appears in the list");

        Validator validator(initialData);
        for (size_t id : serializationOrder)
            validator.validateTransaction(id, allTransactionOperations[id], allTransactionReadResults[id]);

        std::cout << "Validated successfully, " << commitedTransactionIDs.size() << "/" << allTransactionOperations.size() << " commited." << std::endl;
    } catch (const std::runtime_error &e) {
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    }
}
