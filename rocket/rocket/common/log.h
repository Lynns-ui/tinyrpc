#ifndef ROCKET_COMMON_LOG_H
#define ROCKET_COMMON_LOG_H

#include <string>
#include <sys/time.h>
#include <ctime>
#include <sstream>
#include <queue>
#include <memory>
#include <stdio.h>
#include <stdarg.h>
#include <mutex>

namespace rocket {

template<typename... Args>
std::string formatString(const char* str, Args&&... args) {
    int size = snprintf(nullptr, 0, str, std::forward<Args>(args)...);
    std::string result;
    if (size > 0) {
        result.resize(size);
        snprintf(&result[0], size + 1, str, std::forward<Args>(args)...);
    }
    return result;
}

enum LogLevel{
    Unknown = 0,
    Debug = 1,
    Info = 2,
    Error = 3,
};

class Logger {
public:

    void pushLog(const std::string& msg);
    void log();

    static Logger* GetGlobalLogger();
    static Logger* g_logger;
    static void InitLogger();

    LogLevel getLogLevel() {
        return m_set_level;
    }

private:
    Logger(LogLevel level) : m_set_level(level) { }
    LogLevel m_set_level;
    std::queue<std::string> m_buff;
    std::mutex mtx_;
};


std::string LogLevelToString(LogLevel level);
LogLevel StringToLogLevel(const std::string& log_level);

class LogEvent {
public:
    LogEvent(LogLevel level):m_evel(level) { }

    std::string getFileName() const {
        return m_file_name;
    }

    LogLevel getLogLevel() const {
        return m_evel; 
    }

    // 日志格式：[level][%y-%m-%d %H:%M:%S.%ms]\t[pid:thread_id]\t[file_name:line][%msg]
    std::string toString();
private:
    std::string m_file_name;    // 文件名
    std::string m_file_line;    // 行号
    int m_pid;                  // 进程Id
    int m_thread_id;            // 线程id

    LogLevel m_evel;
};


#define DEBUGLOG(str, ...)\
    if (rocket::Logger::GetGlobalLogger()->getLogLevel() <= rocket::Debug)\
    {\
        std::string msg_debug = (new rocket::LogEvent(rocket::LogLevel::Debug))->toString() + \
            "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + rocket::formatString(str, ##__VA_ARGS__);     \
        msg_debug += "\n";                                                                                                                  \
        rocket::Logger::GetGlobalLogger()->pushLog(msg_debug);                                                                              \
        rocket::Logger::GetGlobalLogger()->log();                                                                                           \
    }\
    

#define INFOLOG(str, ...)                                                                                                                  \
    if (rocket::Logger::GetGlobalLogger()->getLogLevel() <= rocket::Info)\
    {\
        std::string msg_info = (new rocket::LogEvent(rocket::LogLevel::Info))->toString()  + \
            "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + rocket::formatString(str, ##__VA_ARGS__);       \
        msg_info += "\n";                                                                                                                   \
        rocket::Logger::GetGlobalLogger()->pushLog(msg_info);                                                                               \
        rocket::Logger::GetGlobalLogger()->log();                                                                                           \
    }\
    

#define ERRORLOG(str, ...)                                                                                                                 \
    if (rocket::Logger::GetGlobalLogger()->getLogLevel() <= rocket::Error)\
    {\
        std::string msg_error = (new rocket::LogEvent(rocket::LogLevel::Error))->toString() + \
            "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + rocket::formatString(str, ##__VA_ARGS__);     \
        msg_error += "\n";                                                                                                                  \
        rocket::Logger::GetGlobalLogger()->pushLog(msg_error);                                                                              \
        rocket::Logger::GetGlobalLogger()->log();\
    }\

}

#endif