#include <sys/timerfd.h>
#include <string.h>
#include "timer.h"
#include "../common/log.h"

namespace lynns {

Timer::Timer() {
    fd_ = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    DEBUGLOG("timer_fd [%d] create successed", fd_);

    // 定时器的时间到达触发的是可读事件，执行OnTimer函数
    listen(FdEvent::IN_EVENT, std::bind(&Timer::onTimer, this));
}

Timer::~Timer() {

}

void Timer::addTimerEvent(TimerEvent::s_ptr event) {
    bool is_reset_timer = false;

    std::unique_lock<std::mutex> locker(mtx_);
    if (pending_events_.empty()) {
        is_reset_timer = true;
    } else {
        auto it = pending_events_.begin();
        if (it->second->getArriveTime() > event->getArriveTime()) {
            is_reset_timer = true;
        }
    }
    pending_events_.emplace(event->getArriveTime(), event);
    locker.unlock();

    if (is_reset_timer) {
        resetArriveTime();
    }
}

void Timer::delTimerEvent(TimerEvent::s_ptr event) {
    event->setCancel(true);

    {
        std::lock_guard<std::mutex>lokcer(mtx_);
        auto begin = pending_events_.lower_bound(event->getArriveTime());  // 第一个
        auto end = pending_events_.upper_bound(event->getArriveTime());    // 最后一个
        
        auto it = begin;
        for (; it != end; it++) {
            if (it->second == event) {
                break;
            }
        }
        if (it != end) {
            pending_events_.erase(it);
        }
    }
    DEBUGLOG("success delete TimerEvent at arrive time [%11d]", event->getArriveTime());
}

void Timer::onTimer() {
    DEBUGLOG("ontimer function");

    char buff[8];
    while (1) {
        if (read(fd_, buff, 8) == -1 && errno == EAGAIN) {
            break;
        }
    }

    // 执行定时任务
    int64_t now = getNowMs();
    
    // 任务集
    std::vector<TimerEvent::s_ptr> tmps;
    std::queue<std::function<void()>> tasks;    // 任务队列

    std::unique_lock<std::mutex> locker(mtx_);
    auto it = pending_events_.begin();
    for (;it != pending_events_.end(); it++) {
        if (it->first <= now) {
            if (!it->second->isCancel()) {
                tmps.push_back(it->second);
                tasks.push(it->second->getTask());
            }
        } else {
            break;
        }
    }
    pending_events_.erase(pending_events_.begin(), it);
    locker.unlock();

    for (int i = 0; i < tmps.size(); i++) {
        if (tmps[i]->isRepeated()) {
            tmps[i]->resetArriveTime();
            addTimerEvent(tmps[i]);
        }
    }
    resetArriveTime();

    while (!tasks.empty()) {
        auto task = tasks.front();
        tasks.pop();
        if (task) {
            INFOLOG("execute task");
            task();
        }
    }
}

void Timer::resetArriveTime() {
    std::unique_lock<std::mutex> locker(mtx_);
    auto tmp = pending_events_;
    locker.unlock();
    if (tmp.empty()) {
        return;
    }

    int64_t now = getNowMs();

    auto it = tmp.begin();
    int64_t interval = 0;
    if (it->second->getArriveTime() > now) {
        interval = it->second->getArriveTime() - now;
    } else {
        interval = 100;
    }

    timespec ts;
    memset(&ts, 0, sizeof(ts));
    ts.tv_sec = interval / 1000;
    ts.tv_nsec = (interval % 1000) * 1000000;

    itimerspec value;
    memset(&value, 0, sizeof(value));
    value.it_value = ts;

    int rt = timerfd_settime(fd_, 0, &value, NULL);
    if (rt < 0) {
        ERRORLOG("timerfd settime error, errno=%d, error = %s", errno, strerror(errno));

    }
    DEBUGLOG("timer reset to [%11d]", now + interval);
}

}