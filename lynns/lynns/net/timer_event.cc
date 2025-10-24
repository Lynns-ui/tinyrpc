#include "timer_event.h"
#include "../common/util.h"
#include "../common/log.h"

namespace lynns {

TimerEvent::TimerEvent(int interval, bool is_repeated, std::function<void()> cb) : is_repeated_(is_repeated),
    is_canceled_(false), interval_(interval), task_(cb) {
    resetArriveTime();
}

void TimerEvent::resetArriveTime() {
    arrive_time_ = getNowMs() + interval_;
    DEBUGLOG("reset Arrive time at [%11d]", arrive_time_);
}

}