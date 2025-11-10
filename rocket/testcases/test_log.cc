#include "../rocket/common/log.h"
#include "../rocket/common/config.h"
#include <iostream>
#include <thread>

void test() {
    DEBUGLOG("debug");
    INFOLOG("info");
    ERRORLOG("error");
}
void test2() {
    for (int i = 0; i < 3000; i++) {
        DEBUGLOG("debug2");
        INFOLOG("info2");
        ERRORLOG("error2");
    }
}

int main() {
    // std::string msg = rocket::formatString("DEBUG log test!");
    // std::cout << "formatString :[ " << msg << "]" << std::endl;
    
    rocket::Config::SetGlobalConfiger("../config/rocket.xml");
    rocket::Logger::InitLogger(rocket::Config::GetGlobalCongfiger()->m_log_path, rocket::Config::GetGlobalCongfiger()->m_log_name,
        rocket::Config::GetGlobalCongfiger()->m_file_size, rocket::Config::GetGlobalCongfiger()->m_async_log);
    std::thread t(test);
    std::thread t2(test2);

    DEBUGLOG("debug");
    INFOLOG("info");
    ERRORLOG("error");
    t.join();
    t2.join();
}