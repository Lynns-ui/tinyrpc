#include <sys/timerfd.h>
#include <string.h>
#include "timer.h"
#include "../common/log.h"
#include "../common/util.h"

namespace rocket {

Timer::Timer() {
    
    m_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    DEBUGLOG("timer_fd[%d] create success", m_fd);

    // fd的可读事件放在epoll上监听
    listen(FdEvent::IN_EVENT, std::bind(&Timer::onTimer, this));
}

Timer::~Timer() {

}

void Timer::addTimerEvent(TimerEvent::s_ptr event) {
    bool is_reset_timerfd = false;
    
    std::unique_lock<std::mutex> locker(mtx_);
    if (m_pending_events.empty()) {
        is_reset_timerfd = true;
    } else {
        auto it = m_pending_events.begin();
        // 插入的定时任务比所有定时事件都要早
        if (it->second->getArriveTime() > event->getArriveTime()) {
            is_reset_timerfd = true;
        }
    }
    m_pending_events.emplace(event->getArriveTime(), event);
    locker.unlock();

    if (is_reset_timerfd) {
        resetArriveTime();
    }
}

void Timer::resetArriveTime() {
    std::unique_lock<std::mutex> locker(mtx_);
    auto tmp = m_pending_events;
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
    int rt = timerfd_settime(m_fd, 0, &value, NULL);
    if (rt != 0) {
        ERRORLOG("timerfd settime error, errno=%d, error = %s", errno, strerror(errno));
    }
    DEBUGLOG("timer reset to %11d", now + interval);
}

void Timer::delTimerEvent(TimerEvent::s_ptr event) {
    event->setCancel(true);

    {
        std::lock_guard<std::mutex>lokcer(mtx_);
        auto begin = m_pending_events.lower_bound(event->getArriveTime());  // 第一个
        auto end = m_pending_events.upper_bound(event->getArriveTime());    // 最后一个
        
        auto it = begin;
        for (it = begin; it != end; it++) {
            if (it->second == event) {
                break;
            }
        }
        if (it != end) {
            m_pending_events.erase(it);
        }
    }
    
    DEBUGLOG("success delete TimerEvent at arrive time %11d", event->getArriveTime());
}

void Timer::onTimer() {
    DEBUGLOG("ontimer function");
    // 处理缓冲区数据，防止下一次继续触发可读事件
    char buff[8];
    while(1) {
        if (read(m_fd, buff, 8) == -1 && errno == EAGAIN) {
            break;
        }
    }

    // 执行定时任务
    int64_t now = getNowMs();
    
    std::vector<TimerEvent::s_ptr> tmps;
    std::vector<std::pair<int64_t, std::function<void()>>> tasks;

    std::unique_lock<std::mutex>locker(mtx_);
    auto it = m_pending_events.begin();
    for (;it != m_pending_events.end(); it++) {
        if (it->first <= now) {
            if (!it->second->isCancel()) {
                tmps.push_back(it->second);
                tasks.push_back(std::make_pair(it->second->getArriveTime(), it->second->getCallBack()));
            }
        } else {
            break;
        }
    }
    m_pending_events.erase(m_pending_events.begin(), it);
    locker.unlock();

    // 需要把重复的event事件再次添加
    for (auto i = tmps.begin(); i != tmps.end(); i++) {
        if ((*i)->isRepeated()) {
            // 调整arrive_time
            // DEBUGLOG("timer is repeated");
            (*i)->resetArriveTime();
            addTimerEvent(*i);
        }
    }
    resetArriveTime();

    for (auto i : tasks) {
        if (i.second) {
            DEBUGLOG("Timer task excute");
            i.second();
        }
    }
}

}


