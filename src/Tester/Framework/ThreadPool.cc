#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t threads) : stopped(false) {
    // Create worker threads
    for (size_t i = 0; i < threads; i++)
        threadList.emplace_back([this] {
            this->workerThread();
        });
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard lock(mutex);
        stopped = true;
    }

    conditionVariable.notify_all();

    for (auto &thread : threadList)
        thread.join();
}

void ThreadPool::workerThread() {
    while (1) {
        std::unique_lock lock(mutex);

        // Wait until thread pool being stopped or new task arrives
        conditionVariable.wait(lock, [this] {
            return stopped || !queue.empty();
        });

        // If all tasks have finished, and thread pool is being stopped
        if (stopped && queue.empty())
            return;

        auto task = std::move(queue.front());
        queue.pop();

        // Unlock and run task
        lock.unlock();
        task();
    }
}

std::unique_ptr<ThreadPool> threadPool;
