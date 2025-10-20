#include "logger.h"
#include <iostream>
#include <time.h>

Logger&  Logger::getInstance()
{
    static Logger log;
    return log;
}

void Logger::setLogLevel(LogLevel level)
{
    m_loglevel = level; // 设置日志级别
}
// 写日志，把日志信息写到lockqueue缓冲区当中
void Logger::log(const std::string &msg)
{
    m_lckQue.push(msg);
}
Logger::Logger()
{
    // 启动专门写日志的线程
    std::thread writeLogTask([&](){
        for(;;)
        {
            // 获取当前的日期，然后写日志信息，写入相应的日志文件当中
            time_t now = time(nullptr);
            tm * nowtm = localtime(&now);

            char file_name[128] = {0};
            sprintf(file_name, "%04d-%02d-%02d-log.txt", nowtm->tm_year + 1900, nowtm->tm_mon + 1, nowtm->tm_mday);

            FILE *pf = fopen(file_name, "a+");
            if(nullptr == pf)
            {
                std::cout << "logger file: " << file_name << " open error!\n";
                exit(EXIT_FAILURE);
            }
            std::string msg = m_lckQue.pop();
            char time_buf[128] = {0};
            sprintf(time_buf, "%02d:%02d:%02d ==> [%s]: ",
                    nowtm->tm_hour,
                    nowtm->tm_min,
                    nowtm->tm_sec,
                    (m_loglevel == INFO ? "info" : "error")
                );
            msg.insert(0, time_buf);
            fputs(msg.c_str(), pf);
            fclose(pf);
        }
    });
    writeLogTask.detach();
}
