#include "../rocket/common/log.h"
#include <iostream>
#include <thread>

void test() {
    DEBUGLOG("DEBUGLOG thread test!");
}


int main() {
    // std::string msg = rocket::formatString("DEBUG log test!");
    // std::cout << "formatString :[ " << msg << "]" << std::endl;

    // std::thread t(test);

    DEBUGLOG("DEBUG log test!");
    INFO("INFO log test");
    ERROR("ERROR log test");
}