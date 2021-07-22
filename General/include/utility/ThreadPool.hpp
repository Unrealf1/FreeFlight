#pragma once

#include <thread>
#include <functional>
#include <queue>
#include <mutex>
#include <atomic>
#include <chrono>
#include <condition_variable>


class ThreadPool {
    using thread_container_t = std::vector<std::thread>;
public:
    using task_t = std::function<void(void)>;
    // TODO: implement proper thread-safe queue
    // with proper waiting 
    using task_container_t = std::queue<task_t>;

    ThreadPool(uint32_t num_threads = std::thread::hardware_concurrency(), bool run_immediately = false) : num_threads(num_threads) {
        if (run_immediately) {
            run();
        }
    }

    void submit(task_t task) {
        std::unique_lock<std::mutex> guard(_lock);
        _tasks.push(task);
        guard.unlock();
        _cv.notify_one();
    }

    void run() {
        //todo do something like "double check" to mostly check non-atomic bool
        if (_alive.load()) {
            return;
        }

        for (auto& t : _threads) {
            t.join();
        }

        _alive.store(true);

        _threads.clear();
        _threads.resize(num_threads);

        for (auto& t : _threads) {
            t = std::thread([this](){
                while(_alive.load()) {
                    auto task = get_task();
                    task();
                }
            });
        }
    }

    void stop() {
        _alive.store(false);
    }

    const uint32_t num_threads;
private:
    task_t get_task() {
        std::unique_lock<std::mutex> guard(_lock);
        _cv.wait(guard, [this]{return !_tasks.empty();});

        auto res = _tasks.front();
        _tasks.pop();
        return res;
    }

    task_container_t _tasks;
    std::mutex _lock;
    std::condition_variable _cv;
    std::atomic<bool> _alive;

    thread_container_t _threads;
    
};

ThreadPool& get_default_thread_pool() {
    static ThreadPool pool(std::thread::hardware_concurrency(), true);
    return pool;
}
