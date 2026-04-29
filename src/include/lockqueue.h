#pragma once
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

// 异步写日志的日志队列
class LockQueue
{
private:
    std::mutex m_mutex;
    std::queue<InfoMsg> m_queue;
    std::condition_variable m_condvariable;
    bool m_stop = false;
public:
    void push(const InfoMsg&data)
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_stop)
                return;
            m_queue.push(data);
        }
        m_condvariable.notify_one();
    }
    bool pop(InfoMsg& data)
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        m_condvariable.wait(lock, [this] {
            return m_stop || !m_queue.empty();
        });

        if (m_stop && m_queue.empty())
            return false;

        data = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }
    bool pop(InfoMsg&& data)
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        m_condvariable.wait(lock, [this] {
            return m_stop || !m_queue.empty();
        });

        if (m_stop && m_queue.empty())
            return false;

        data = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }
    void stop()
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_stop = true;
        }
        m_condvariable.notify_all();
    }
    bool try_pop(InfoMsg& data)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_queue.empty())
            return false;

        data = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }
    size_t size()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }
    bool empty()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }
};