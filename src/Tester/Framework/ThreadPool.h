#ifndef _MENCI_TRANSACTION_TESTER_FRAMEWORK_THREAD_POOL_H
#define _MENCI_TRANSACTION_TESTER_FRAMEWORK_THREAD_POOL_H

#include <thread>
#include <mutex>
#include <vector>
#include <future>
#include <queue>
#include <condition_variable>
#include <functional>
#include <type_traits>
#include <memory>

class ThreadPool {
    std::vector<std::thread> threadList;
    std::queue<std::function<void ()>> queue;
    std::mutex mutex;
    std::condition_variable conditionVariable;
    bool stopped;

    void workerThread();

public:
    ThreadPool(size_t threads);
    ~ThreadPool();

    template <typename T>
    std::future<std::invoke_result_t<T>> run(T &&task) {
        if (stopped)
            throw std::logic_error("ThreadPool is being stopped");

        auto packagedTask = std::make_shared<std::packaged_task<std::invoke_result_t<T> ()>>(task);

        {
            std::lock_guard lock(mutex);
            queue.emplace([packagedTask] { (*packagedTask)(); });
        }

        conditionVariable.notify_one();
        return packagedTask->get_future();
    }
};

extern std::unique_ptr<ThreadPool> threadPool;

#endif // _MENCI_TRANSACTION_TESTER_FRAMEWORK_THREAD_POOL_H
