#include <string>
enum LogLevel
{
    INFO,
    ERROR
};

typedef struct {
    string msg;
    LogLevel level;
} InfoMsg;
