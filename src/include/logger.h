#pragma once
#include "lockqueue.h"

// 定义宏
#define LOG_INFO(logmsgformat, ...) \
    do { \
        Logger &logger = Logger::getInstance(); \
        logger.setLogLevel(INFO); \
        char msg[1024] = {0}; \
        sprintf(msg, logmsgformat, ##__VA_ARGS__); \
        std::string str(msg); \
        logger.log(str); \
    } while(0)


#define LOG_ERR(logmsgformat, ...) \
    do { \
        Logger &logger = Logger::getInstance(); \
        logger.setLogLevel(ERROR); \
        char msg[1024] = {0}; \
        sprintf(msg, logmsgformat, ##__VA_ARGS__); \
        std::string str(msg); \
        logger.log(str); \
    } while(0)

enum LogLevel
{
    INFO,  // 普通信息
    ERROR, // 错误信息
};

class Logger
{
public:
    static Logger&  getInstance();
    void setLogLevel(LogLevel level);
    void log(const std::string &msg);
    Logger();
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;  
private:
    int m_loglevel;
    LockQueue<std::string> m_lckQue; // 日志缓冲队列
};