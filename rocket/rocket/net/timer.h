#ifndef ROCKET_NET_TIMER_H
#define ROCKET_NET_TIMER_H

#include <map>
#include <mutex>
#include "fdevent.h"
#include "timerevent.h"

namespace rocket {

class Timer : public FdEvent {
public:
    Timer();

    ~Timer();

    void addTimerEvent(TimerEvent::s_ptr event);

    void delTimerEvent(TimerEvent::s_ptr event);

    void onTimer();  // 发生IO事件，event会执行这个回调函数

private:
    void resetArriveTime();

    std::mutex mtx_;
    std::multimap<int64_t, TimerEvent::s_ptr> m_pending_events; // 键（key）会自动按照升序进行排序

};


}

#endif