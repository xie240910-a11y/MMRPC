#pragma once
#include <string>
#include <thread>
#include <atomic>
#include "lockqueue.h"

class Logger
{
public:
    static Logger& getInstance();

    void setLogLevel(LogLevel level);
    void log(LogLevel level, const std::string &msg);

private:
    Logger();
    ~Logger();

    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;

    void writeThreadFunc();

private:
    std::atomic<int> m_loglevel;   // 线程安全
    LockQueue<std::string> m_lckQue;

    std::thread m_writeThread;
    std::atomic<bool> m_running;
};