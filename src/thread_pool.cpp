#include "thread_pool.hpp"

#include <iostream>
#include <stdexcept>

namespace gbhs {

ThreadPool::ThreadPool(const size_t& thread_count) {
    if (thread_count == 0) {
        throw std::invalid_argument("Thread count cannot be 0.");
    }

    // create threads
    std::cout << "creating " << thread_count << " threads ..." << std::endl;
    for (int i = 0; i < thread_count; ++i) {
        threads.push_back(std::thread([this]() {
            std::unique_ptr<Task> assigned_task{nullptr};
            while (true) {
                // wait for task
                {
                    std::unique_lock<std::mutex> lock(queue_lock);
                    cond_queue.wait(lock, [this]() { return !tasks.empty(); });
                    assigned_task = std::move(tasks.front());
                    tasks.pop();
                }

                if (assigned_task == nullptr) {
                    break;
                }

                // perform task
                (*assigned_task)();

                // notify if all tasks are done
                // TODO correctness and performance
                if (--cnt_tasks_pending == 0) {
                    cond_done.notify_one();
                }
            }
        }));
    }
}

ThreadPool::~ThreadPool() {
    // TODO: what about stuck threads?
    {
        std::unique_lock guard(queue_lock);
        for (auto& t : threads) {
            tasks.push(nullptr);
        }
    }
    cond_queue.notify_all();
    for (auto& t : threads) {
        t.join();
    }
}

void ThreadPool::addTask(Task task) {
    auto task_ptr = std::make_unique<Task>(std::move(task));
    {
        std::unique_lock<std::mutex> lock(queue_lock);
        tasks.push(std::move(task_ptr));
    }
    ++cnt_tasks_pending;
    cond_queue.notify_one();
}

void ThreadPool::waitUntilDone() {
    std::unique_lock<std::mutex> lock(done_lock);
    cond_done.wait(lock, [this]() { return tasks.empty(); });
}

}  // namespace gbhs
