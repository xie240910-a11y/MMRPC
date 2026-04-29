#include "logger.h"
#include <iostream>
#include <ctime>
#include <cstdio>

Logger& Logger::getInstance()
{
    static Logger instance;
    return instance;
}

Logger::Logger()
{
    m_running = true;

    m_writeThread = std::thread(&Logger::writeThreadFunc, this);
}

Logger::~Logger()
{
    m_running = false;
    m_lckQue.stop();   // ⭐ 唤醒阻塞线程
    if (m_writeThread.joinable())
        m_writeThread.join();
}

void Logger::setLogLevel(LogLevel level)
{
    m_loglevel = level;
}

void Logger::log(LogLevel level, const std::string &msg)
{
    if (level < m_loglevel)
        return;

    m_lckQue.push(msg);
}

void Logger::writeThreadFunc()
{
    FILE *pf = nullptr;
    std::string current_file;

    while (m_running)
    {
        std::string msg;
        if (m_lckQue.pop(msg))
        {
            // 获取当前时间（必须每条日志更新）
            time_t now = time(nullptr);
            tm *nowtm = localtime(&now);

            // 生成文件名
            char file_name[128] = {0};
            sprintf(file_name, "%04d-%02d-%02d-log.txt",
                    nowtm->tm_year + 1900,
                    nowtm->tm_mon + 1,
                    nowtm->tm_mday);

            // 如果日期变化，重新打开文件
            if (current_file != file_name)
            {
                if (pf)
                    fclose(pf);

                pf = fopen(file_name, "a+");
                if (!pf)
                {
                    std::cout << "open log file error: " << file_name << std::endl;
                    continue;
                }

                current_file = file_name;
            }

            // 格式化时间
            char time_buf[128] = {0};
            sprintf(time_buf, "%02d:%02d:%02d [%s]: ",
                    nowtm->tm_hour,
                    nowtm->tm_min,
                    nowtm->tm_sec,
                    (m_loglevel == INFO ? "INFO" : "ERROR"));

            msg.insert(0, time_buf);

            fputs(msg.c_str(), pf);
            fputs("\n", pf);
            fflush(pf); // 防止丢日志
        }
        else
        {
            break;
        }
    }

    if (pf)
        fclose(pf);
}