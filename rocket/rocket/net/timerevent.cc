#include "timerevent.h"
#include "../common/log.h"
#include "../common/util.h"

namespace rocket {

TimerEvent::TimerEvent(int interval, bool is_repeated, std::function<void()> cb) : 
    m_interval(interval), m_is_repeated(is_repeated), m_task(cb) {
    resetArriveTime();
}

void TimerEvent::resetArriveTime() {
    // 当前时间 + 时间间隔
    m_arrive_time = getNowMs() + m_interval;
    DEBUGLOG("success create timer event, it will excute at [%11d]", m_arrive_time);
}

}