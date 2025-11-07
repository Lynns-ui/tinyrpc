#include "log.h"
#include "util.h"
#include "config.h"

namespace rocket {

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

bool isOpenLog(const std::string& open_log) {
    if (open_log == "true") {
        return true;
    } else {
        return false;
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


Logger* Logger::g_logger = nullptr;
Logger* Logger::GetGlobalLogger() {
    return g_logger;
}

Logger::Logger(LogLevel level, std::string file_name, std::string file_path, int max_file_size, bool open_file /* = false */) :
        m_set_level(level), m_file_name(file_name), m_file_path(file_path), m_max_file_size(max_file_size) , m_is_openfile(open_file) { 
    if (m_is_openfile) {
        m_asynclogger = std::make_shared<AsyncLogger>(file_name, file_path, max_file_size);
        m_asynclogger->start();
    }
}

void Logger::InitLogger(std::string log_path, std::string log_name, std::string max_line_count, std::string is_open_file /* "false" */) {
    if (g_logger) {
        return;
    }
    std::string global_log_level = Config::GetGlobalCongfiger()->m_log_level;
    printf("Init Log level [%s]\n", global_log_level.c_str());
    printf("Whether to enable asynchronous logging [%s]\n", is_open_file.c_str());
    if (isOpenLog(is_open_file)) {
        printf("log file path is [%s/%s], max file count[%s]\n", log_path.c_str(), log_name.c_str(), max_line_count.c_str());
    }
    g_logger = new Logger(StringToLogLevel(global_log_level), log_name, log_path, std::stoi(max_line_count), isOpenLog(is_open_file));
}

void Logger::pushLog(const std::string& msg) {
    std::lock_guard<std::mutex>locker(mtx_);
    m_buff.push(msg);
}

void Logger::log() {
    std::lock_guard<std::mutex>locker(mtx_);
    if (!m_is_openfile) {
        while (!m_buff.empty()) {
            std::string msg = m_buff.front();
            m_buff.pop();
            
            printf(msg.c_str());
        }
    } else {
        m_asynclogger->pushLog(m_buff);
    }
}

AsyncLogger::AsyncLogger(std::string file_name, std::string file_path, int max_file_size) : 
    m_start(false), m_file_name(file_name), m_file_path(file_path), m_max_file_size(max_file_size) {
}

AsyncLogger::~AsyncLogger() {
    m_start = false;
    m_cond.notify_all();
    if (m_write_thread.joinable()) {
        m_write_thread.join();
    }
}

void AsyncLogger::start() {
    m_start = true;
    // 创建一个写线程
    m_write_thread = std::thread(&AsyncLogger::loop, this);
}

void AsyncLogger::loop() {
    // 将 buffer 里面的全部数据打印到文件中
    // 然后线程阻塞直到有新的数据出现在buffer中
    if (!createFile()) {
        return;
    }

    std::unique_lock<std::mutex> locker(m_mtx);
    while (m_start) {
        // 当lambda表达式，返回false线程释放锁，阻塞
        m_cond.wait(locker, [this](){
            return !m_buff.empty() || !m_start;
        });
        
        if (!m_start) {
            break;
        }

        while (!m_buff.empty()) {
            std::string log = m_buff.front();
            m_buff.pop();
            m_log_file << log;
            m_log_file.flush();
            m_line_count++;

            // std::streampos file_size = m_log_file.tellp();
            if (m_line_count >= m_max_file_size) {
                m_file_index++;
                m_log_file.close();
                if(!createFile()) {
                    std::cout << "ctreate file failed" << std::endl;
                    return;
                }
                m_line_count = 0;
            }
        }
       
    }
}

void AsyncLogger::pushLog(std::queue<std::string>& buff) {
    std::lock_guard<std::mutex> locker(m_mtx);
    m_buff.swap(buff);
    m_cond.notify_one();
}

bool AsyncLogger::createFile() {
    struct timeval now_time;

    gettimeofday(&now_time, nullptr);

    struct tm now_time_t;
    localtime_r(&(now_time.tv_sec), &now_time_t);

    char buff[128];
    strftime(&buff[0], 128, "%Y%m%d", &now_time_t);

    std::string time_str(buff);
    
    if (!m_file_path.empty() && m_file_path.back() == '/') {
        m_file_path.pop_back(); // 移除尾部 '/'
    }

    std::stringstream ss;
    ss << m_file_path << "/" << m_file_name << "_" << time_str << "_" 
        << std::to_string(m_file_index) << ".log";
    
    if (m_log_file.is_open()) {
        m_log_file.close();
    }

    m_log_file.open(ss.str(), std::ios::out | std::ios::app | std::ios::binary);
    if (!m_log_file.is_open()) {
        std::cerr << "log file open failed: " << ss.str() 
              << " error: " << std::strerror(errno) << std::endl;
        return false;
    }

    return true;
}

}