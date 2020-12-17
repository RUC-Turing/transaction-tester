#ifndef _MENCI_TRANSACTION_TESTER_FRAMEWORK_UTILITY_H
#define _MENCI_TRANSACTION_TESTER_FRAMEWORK_UTILITY_H

#include <cstdint>
#include <atomic>
#include <limits>

using timestamp_t = uint64_t;

constexpr timestamp_t TIMESTAMP_INF = std::numeric_limits<timestamp_t>::max();

inline timestamp_t getTimestamp() {
    static std::atomic_uint64_t atomic = 1;
    return atomic.fetch_add(1, std::memory_order_relaxed);
}

#endif // _MENCI_TRANSACTION_TESTER_FRAMEWORK_UTILITY_H
