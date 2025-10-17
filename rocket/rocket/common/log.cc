#include "log.h"
#include "util.h"

namespace rocket {

Logger* Logger::g_logger = nullptr;

Logger* Logger::GetGlobalLogger() {
    if (g_logger) {
        return g_logger;
    }
    g_logger = new Logger();
    return g_logger;
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
       << "[" << std::to_string(m_pid) << ":" << std::to_string(m_thread_id) << "]\t"
       << "[" << std::string(__FILE__) << ":" << __LINE__ << "]\t";
    return ss.str();
}

void Logger::pushLog(const std::string& msg) {
    m_buff.push(msg);
}

void Logger::log() {
    while (!m_buff.empty()) {
        std::string msg = m_buff.front();
        m_buff.pop();
        
        printf(msg.c_str());
    }
}

}