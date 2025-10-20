#ifndef LYNNS_COMMON_CONFIG_H
#define LYNNS_COMMON_CONFIG_H

#include <string>

namespace lynns {

class Configer {
public:
    ~Configer() = default;

    static void setGlobalConfiger(const char* xmlfile);

    static Configer* getGlobalConfiger();

    static Configer* g_configer;

    std::string g_config_level;

private:
    Configer(const char* xmlfile);

};


}

#endif