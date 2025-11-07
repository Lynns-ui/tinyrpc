#ifndef ROCKET_COMMON_CONFIG_H
#define ROCKET_COMMON_CONFIG_H

#include <tinyxml2.h>
#include <map>
#include <string>

namespace rocket {

class Config {
public:
    static void SetGlobalConfiger(const char* xmlfile);
    static Config* GetGlobalCongfiger();

    static Config* g_configer;
    std::string m_log_level;
    std::string m_async_log;
    std::string m_log_path;
    std::string m_log_name;
    std::string m_file_size;

    std::string m_ip;
    std::string m_port;

    std::string m_client_async_log;
    std::string m_client_log_path;
    std::string m_client_log_name;
    std::string m_client_file_size;
private:
    Config(const char* xmlfile);
    // std::map<std::string, std::string> m_config_values; 
    

};



}


#endif