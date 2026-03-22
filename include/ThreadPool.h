#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <thread>
#include <functional>
#include <atomic>
#include <iostream>
#include <memory>
#include <condition_variable>
#include <future>
#include <queue>
#include <stdexcept>
#include <utility>

using namespace std;

class ThreadPool {
public:
    explicit ThreadPool(size_t num_threads = thread::hardware_concurrency()) : m_stop(false) {
        for (size_t i = 0; i < num_threads; ++i) {
            m_workers.emplace_back([this] {
                while (true) {
                    packaged_task<void()> task;

                    {
                        unique_lock<mutex> lock(m_mutex);
                        m_cv.wait(lock, [this] {
                            return m_stop || !m_tasks.empty();
                            });

                        if (m_stop && m_tasks.empty()) {
                            return;
                        }

                        task = move(m_tasks.front());
                        m_tasks.pop();
                    }

                    try {
                        task();
                    }
                    catch (const exception& e) {
                        cerr << "Exception in worker thread: " << e.what() << endl;
                    }
                }
                });
        }
    }

    ~ThreadPool() {
        {
            unique_lock<mutex> lock(m_mutex);
            m_stop = true;
        }
        m_cv.notify_all();

        for (thread& worker : m_workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    template<typename Func>
    void submit(Func&& func) {
        packaged_task<void()> task(forward<Func>(func));
        {
            unique_lock<mutex> lock(m_mutex);
            if (m_stop) {
                throw runtime_error("submit on stopped ThreadPool");
            }
            m_tasks.push(move(task));
        }
        m_cv.notify_one();
    }

    void wait_all() {
        unique_lock<mutex> lock(m_mutex);
        m_cv.wait(lock, [this]() { return m_tasks.empty(); });
    }

    size_t get_thread_count() const {
        return m_workers.size();
    }

    size_t get_task_count() const {
        unique_lock<mutex> lock(m_mutex);
        return m_tasks.size();
    }

private:
    vector<thread> m_workers;
    queue<packaged_task<void()>> m_tasks;
    mutable mutex m_mutex;
    condition_variable m_cv;
    bool m_stop;
};

#endif