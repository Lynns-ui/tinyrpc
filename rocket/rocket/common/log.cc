#include "log.h"
#include "util.h"
#include "config.h"

namespace rocket {


Logger* Logger::g_logger = nullptr;

Logger* Logger::GetGlobalLogger() {
    return g_logger;
}

void Logger::InitLogger() {
    if (g_logger) {
        return;
    }
    std::string global_log_level = Config::GetGlobalCongfiger()->m_log_level;
    printf("Init Log level [%s]\n", global_log_level.c_str());
    g_logger = new Logger(StringToLogLevel(global_log_level));
}

std::string LogLevelToString(LogLevel level) {
    switch (level)
    {
    case Debug:
        return "DEBUG";
        break;
    case Info:
        return "INFO";
        break;
    case Error:
        return "ERROR";
        break;
    default:
        return "UNKOWN";
        break;
    }
}

LogLevel StringToLogLevel(const std::string& log_level) {
    if (log_level == "DEBUG") {
        return Debug;
    } else if (log_level == "INFO") {
        return Info;
    } else if (log_level == "ERROR") {
        return Error;
    } else {
        return Unknown;
    }
}

std::string LogEvent::toString() {
    struct timeval now_time;

    gettimeofday(&now_time, nullptr);

    struct tm now_time_t;
    localtime_r(&(now_time.tv_sec), &now_time_t);

    char buff[128];
    strftime(&buff[0], 128, "%y-%m-%d %H:%M:%S", &now_time_t);

    std::string time_str(buff);
    int ms = now_time.tv_usec / 1000;
    time_str = time_str + "." + std::to_string(ms);

    m_pid = getPid();
    m_thread_id = getThreadId();
    
    std::stringstream ss;
    ss << "[" << LogLevelToString(m_evel) << "]\t"
       << "[" << time_str << "]\t" 
       << "[" << std::to_string(m_pid) << ":" << std::to_string(m_thread_id) << "]\t";
    return ss.str();
}

void Logger::pushLog(const std::string& msg) {
    std::lock_guard<std::mutex>locker(mtx_);
    m_buff.push(msg);
}

void Logger::log() {
    std::lock_guard<std::mutex>locker(mtx_);
    while (!m_buff.empty()) {
        std::string msg = m_buff.front();
        m_buff.pop();
        
        printf(msg.c_str());
    }
}

}