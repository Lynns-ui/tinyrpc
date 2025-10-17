#include "../rocket/common/log.h"
#include "../rocket/common/config.h"
#include <iostream>
#include <thread>

int main() {
    // std::string msg = rocket::formatString("DEBUG log test!");
    // std::cout << "formatString :[ " << msg << "]" << std::endl;

    // std::thread t(test);
    rocket::Config::SetGlobalConfiger("../config/rocket.xml");

    DEBUGLOG("debug");
    INFOLOG("info");
    ERRORLOG("error");
}