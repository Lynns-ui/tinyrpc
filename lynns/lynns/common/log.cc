#include <sys/time.h>
#include <time.h>
#include <sstream>
#include <cstring>
#include "log.h"
#include "config.h"

namespace lynns{

std::string LogLevelToString(LogLevel level) {
    switch (level)
    {
    case DEBUG:
        return "DEBUG";
        break;
    case INFO:
        return "INFO";
        break;
    case ERROR:
        return "ERROR";
        break;
    default:
        return "UNKNOW";
        break;
    }
}

LogLevel StringToLogLevel(const std::string& g_config_level) {
    if (g_config_level == "DEBUG") {
        return DEBUG;
    } else if (g_config_level == "INFO") {
        return INFO;
    } else if (g_config_level == "ERROR") {
        return ERROR;
    } else {
        return UNKNOW;
    }
}

// [level][%y-%m-%d %H:%M:%s.%ms]\t[pid:thread_id]\t[file_name:line][msg]
std::string LogEvent::toString(){
    struct timeval now_time;
    gettimeofday(&now_time, NULL);

    struct tm now_time_t;
    localtime_r(&(now_time.tv_sec), &now_time_t);

    char buff[128];
    strftime(&buff[0], 128, "%y-%m-%d %H:%M:%S", &now_time_t);

    std::string time_str(buff);
    int ms = now_time.tv_usec / 1000;
    time_str = time_str + "." + std::to_string(ms);

    pid_ = getPid();
    threadid_ = getThreadId();

    std::stringstream ss;
    ss << "[" << LogLevelToString(level_) << "]\t"
       << "[" << time_str << "]\t"
       << "[" << std::to_string(pid_) << ":" << std::to_string(threadid_) << "]\t";
    return ss.str();
}

Logger* Logger::g_logger = nullptr;

Logger::Logger(LogLevel level) : level_(level) { }

void Logger::initGlobalLogger() {
    if (g_logger == nullptr) {
        LogLevel g_level = StringToLogLevel(Configer::getGlobalConfiger()->g_config_level);
        g_logger = new Logger(g_level);
        printf("Init Logger Success, LogLevel:[%s]\n", Configer::getGlobalConfiger()->g_config_level.c_str());
    }
}

Logger* Logger::getGlobalLogger() {
    return g_logger;
}

void Logger::pushLog(const std::string& msg) {
    std::lock_guard<std::mutex> locker(mtx_);
    buff_.push(msg);
}

void Logger::log() {
    std::lock_guard<std::mutex> locker(mtx_);
    while (!buff_.empty()) {
        std::string msg = buff_.front();
        buff_.pop();

        printf(msg.c_str());
    }
}

}