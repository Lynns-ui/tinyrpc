#ifndef LYNNS_NET_TIMER_H
#define LYNNS_NET_TIMER_H

#include <map>
#include <mutex>
#include "fdevent.h"
#include "timer_event.h"

namespace lynns {

class Timer : public FdEvent {
public:
    Timer();

    ~Timer();

    void addTimerEvent(TimerEvent::s_ptr event);

    void delTimerEvent(TimerEvent::s_ptr event);

    void onTimer();

private:
    void resetArriveTime();

    std::multimap<int64_t, TimerEvent::s_ptr> pending_events_;
    std::mutex mtx_;
};


}


#endif