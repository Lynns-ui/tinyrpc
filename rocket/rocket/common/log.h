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
#include <thread>
#include <functional>
#include <condition_variable>
#include <atomic>
#include <fstream>
#include <iostream>

namespace rocket {

#define DEBUGLOG(str, ...)\
    if (rocket::Logger::GetGlobalLogger()->getLogLevel() <= rocket::Debug)\
    {\
        std::string msg_debug = (rocket::LogEvent(rocket::LogLevel::Debug)).toString() + \
            "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + rocket::formatString(str, ##__VA_ARGS__);\
        msg_debug += "\n";\
        rocket::Logger::GetGlobalLogger()->pushLog(msg_debug);\
        rocket::Logger::GetGlobalLogger()->log();\
    }\

#define INFOLOG(str, ...)\
    if (rocket::Logger::GetGlobalLogger()->getLogLevel() <= rocket::Info)\
    {\
        std::string msg_info = (rocket::LogEvent(rocket::LogLevel::Info)).toString()  + \
            "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + rocket::formatString(str, ##__VA_ARGS__); \
        msg_info += "\n";\
        rocket::Logger::GetGlobalLogger()->pushLog(msg_info);\
        rocket::Logger::GetGlobalLogger()->log();\
    }\

#define ERRORLOG(str, ...)\
    if (rocket::Logger::GetGlobalLogger()->getLogLevel() <= rocket::Error)\
    {\
        std::string msg_error = (rocket::LogEvent(rocket::LogLevel::Error)).toString() + \
            "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + rocket::formatString(str, ##__VA_ARGS__);\
        msg_error += "\n";\
        rocket::Logger::GetGlobalLogger()->pushLog(msg_error);\
        rocket::Logger::GetGlobalLogger()->log();\
    }\

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


// 异步日志类
class AsyncLogger {
public:
    AsyncLogger(std::string file_name, std::string file_path, int max_file_size);

    ~AsyncLogger();

    void loop();

    void pushLog(const std::string& log);

    void start();

private:
    bool createFile();

    std::queue<std::string> m_buff;

    std::atomic<bool> m_start;

    // m_file_path/m_file_name_yyyymmdd.1
    std::string m_file_name;    // 文件名字
    std::string m_file_path;    // 文件路径

    int m_max_file_size;         // 文件最大行数 字节

    std::mutex m_mtx;
    std::thread m_write_thread;
    std::condition_variable m_cond;

    std::ofstream m_log_file;

    int m_file_index {0};

    std::atomic<int> m_line_count {0};
};

class Logger {
public:
    void pushLog(const std::string& msg);

    void log();

    static Logger* GetGlobalLogger();
    static Logger* g_logger;

    static void InitLogger(std::string log_path, std::string log_name, std::string max_line_count, std::string is_open_file = "false");

    LogLevel getLogLevel() {
        return m_set_level;
    }

private:
    Logger(LogLevel level, std::string file_name, std::string file_path, int max_file_size, bool is_open_file = false);

    LogLevel m_set_level;
    std::queue<std::string> m_buff;
    bool m_is_openfile {false};

    std::mutex mtx_;

    // m_file_path/m_file_name_yyyymmdd.1
    // 
    std::string m_file_name;    // 文件名字
    std::string m_file_path;    // 文件路径

    int m_max_file_size;         // 文件最大行数

    std::shared_ptr<AsyncLogger> m_asynclogger {nullptr};
};


std::string LogLevelToString(LogLevel level);

LogLevel StringToLogLevel(const std::string& log_level);

bool isOpenLog(const std::string& open_log);

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


}

#endif