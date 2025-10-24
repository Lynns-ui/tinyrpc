#ifndef LYNNS_NET_TIMER_EVENT_H
#define LYNNS_NET_TIMER_EVENT_H

#include <functional>
#include <memory>

namespace lynns {

class TimerEvent { 
public:
    typedef std::shared_ptr<TimerEvent> s_ptr;

    TimerEvent(int interval, bool is_repeated, std::function<void()> cb);

    ~TimerEvent() = default;

    int64_t getArriveTime() const {
        return arrive_time_;
    }

    void setCancel(bool iscancel) {
        is_canceled_ = iscancel;
    }

    bool isCancel() {
        return is_canceled_;
    }

    std::function<void()> getTask() const {
        return task_;
    }

    bool isRepeated() const {
        return is_repeated_;
    }

    void resetArriveTime();
private:
    int64_t arrive_time_;
    bool is_repeated_;
    bool is_canceled_;
    int64_t interval_;
    std::function<void()> task_;
};




}


#endif