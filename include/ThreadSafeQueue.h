#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

using namespace std;

template<typename T>
class ThreadSafeQueue {
public:
    ThreadSafeQueue() = default;
    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

    void push(T value) {
        {
            lock_guard<mutex> lock(m_mutex);
            m_queue.push(move(value));
        }
        m_cond.notify_one();
    }

    T pop() {
        unique_lock<mutex> lock(m_mutex);
        m_cond.wait(lock, [this]() { return !m_queue.empty(); });

        T value = move(m_queue.front());
        m_queue.pop();
        return value;
    }

    bool try_pop(T& value) {
        lock_guard<mutex> lock(m_mutex);
        if (m_queue.empty()) {
            return false;
        }

        value = move(m_queue.front());
        m_queue.pop();
        return true;
    }

    bool empty() const {
        lock_guard<mutex> lock(m_mutex);
        return m_queue.empty();
    }

    size_t size() const {
        lock_guard<mutex> lock(m_mutex);
        return m_queue.size();
    }

    void notify_all() {
        m_cond.notify_all();
    }

private:
    mutable mutex m_mutex;
    queue<T> m_queue;
    condition_variable m_cond;
};

#endif