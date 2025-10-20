#include <thread>
#include "../lynns/common/log.h"
#include "../lynns/common/config.h"

void test1() {
    DEBUGLOG("this is debug log[1]!");
    INFOLOG("this is info log[1]!");
    ERRORLOG("this is error log[1]!");
    
}

void test2() {
    DEBUGLOG("this is debug log[2]!");
    INFOLOG("this is info log[2]!");
    ERRORLOG("this is error log[2]!");
}

int main() {
    lynns::Configer::setGlobalConfiger("../config/lynns.xml");
    lynns::Logger::initGlobalLogger();


    std::thread t1(test1);
    std::thread t2(test2);

    DEBUGLOG("this is debug log!");
    INFOLOG("this is info log!");
    ERRORLOG("this is error log!");
    
    t1.join();
    t2.join();
}