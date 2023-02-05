#ifndef GBHS_THREAD_POOL_H
#define GBHS_THREAD_POOL_H

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

// TODO try advanced techniques; e.g. stealing tasks, future when_all, multiple queues
// TODO improve waitUntilDone function

namespace gbhs {

class ThreadPool {
   public:
    using Task = std::function<void(void)>;

    ThreadPool(const size_t& thread_count = std::thread::hardware_concurrency());
    ~ThreadPool();

    void addTask(const Task& task);
    void waitUntilDone();

    ThreadPool(const ThreadPool& t) = delete;
    ThreadPool(ThreadPool&& t) = delete;
    ThreadPool& operator=(const ThreadPool& t) = delete;
    ThreadPool& operator=(ThreadPool&& t) = delete;

   private:
    std::vector<std::thread> threads;
    std::queue<std::unique_ptr<Task>> tasks;
    std::mutex queue_lock;
    std::mutex done_lock;
    std::condition_variable cond_queue;
    std::condition_variable cond_done;
    std::atomic<int> cnt_tasks_pending = 0;
};

}  // namespace gbhs

#endif
