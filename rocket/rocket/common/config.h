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
private:
    Config(const char* xmlfile);
    // std::map<std::string, std::string> m_config_values; 
    

};



}


#endif