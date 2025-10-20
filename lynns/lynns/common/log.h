#ifndef LYNNS_COMMON_LOG_H
#define LYNNS_COMMON_LOG_H

#include <stdarg.h>
#include <string>
#include <stdio.h>
#include <mutex>
#include <queue>
#include "util.h"

namespace lynns {

// 可变参数模板
template <typename... Args>
std::string formatString(const char* str, Args&&... args) {
    int size = snprintf(nullptr, 0, str, std::forward<Args>(args)...);
    std::string result;
    if (size > 0) {
        result.resize(size);
        snprintf(&result[0], size + 1, str, std::forward<Args>(args)...);
    }
    return result;
}

// 三种日志级别
enum LogLevel {
    UNKNOW = 0,
    DEBUG = 1,
    INFO = 2,
    ERROR = 3,
};

// 日志器：发送日志
class Logger {
public:
    
    ~Logger() {
        if (g_logger) {
            delete g_logger;
            g_logger = nullptr;
        }
    }

    static Logger* g_logger;
    static Logger* getGlobalLogger();

    static void initGlobalLogger();

    void pushLog(const std::string& msg);

    void log();

    LogLevel getLogLevel() const {
        return level_;
    }

private:
    Logger(LogLevel level);

    LogLevel level_;

    std::mutex mtx_;
    std::queue<std::string> buff_;

};


std::string LogLevelToString(LogLevel level);
LogLevel StringToLogLevel(const std::string& g_config_level);

// 日志事件
class LogEvent {
public:
    LogEvent(LogLevel level) : level_(level){

    }
    ~LogEvent() = default;

    std::string toString();
private:
    std::string filename_;
    std::string fileline_;
    
    int pid_;
    int threadid_;
    LogLevel level_;
};

#define DEBUGLOG(str, ...)\
    if (lynns::Logger::getGlobalLogger()->getLogLevel() <= lynns::DEBUG) \
    {\
        std::string msg_debug = (new lynns::LogEvent(lynns::DEBUG))->toString() \
            + "[" + __FILE__ ":" + std::to_string(__LINE__) + "]\t"\
            + lynns::formatString(str, ##__VA_ARGS__) + "\n";\
        lynns::Logger::getGlobalLogger()->pushLog(msg_debug); \
        lynns::Logger::getGlobalLogger()->log(); \
    } \

#define INFOLOG(str, ...)\
    if (lynns::Logger::getGlobalLogger()->getLogLevel() <= lynns::INFO) \
    {\
        std::string msg_info = (new lynns::LogEvent(lynns::INFO))->toString() \
            + "[" + __FILE__ ":" + std::to_string(__LINE__) + "]\t"\
            + lynns::formatString(str, ##__VA_ARGS__) + "\n";\
        lynns::Logger::getGlobalLogger()->pushLog(msg_info); \
        lynns::Logger::getGlobalLogger()->log(); \
    } \

#define ERRORLOG(str, ...)\
    if (lynns::Logger::getGlobalLogger()->getLogLevel() <= lynns::ERROR) \
    {\
        std::string msg_error = (new lynns::LogEvent(lynns::ERROR))->toString() \
            + "[" + __FILE__ ":" + std::to_string(__LINE__) + "]\t"\
            + lynns::formatString(str, ##__VA_ARGS__) + "\n";\
        lynns::Logger::getGlobalLogger()->pushLog(msg_error); \
        lynns::Logger::getGlobalLogger()->log(); \
    } \

}


#endif