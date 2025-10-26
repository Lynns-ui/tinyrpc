#ifndef ROCKET_NET_EVENTLOOP_H
#define ROCKET_NET_EVENTLOOP_H

#include <pthread.h>
#include <set>
#include <functional>
#include <queue>
#include <mutex>
#include "fdevent.h"
#include "wakeupfd.h"
#include "timer.h"

namespace rocket {

class EventLoop {
public:
    EventLoop();
    ~EventLoop();

    void loop();

    void wakeup();

    void stop();

    void addEpollEvent(FdEvent* event);

    void delEpollEvent(FdEvent* event);

    bool isInLoopThread();

    void addTask(std::function<void()> cb, bool is_wake_up = false);

    void addTimerEvent(TimerEvent::s_ptr event);

    // 获取当前线程的
    static EventLoop* GetCurrentEventloop(); 

private:
    void dealWakeup();

    void initWakeUpFdEvent();

    void initTimer();
    
    pid_t thread_id {0};
    int m_epoll_fd {0};
    int m_wakeup_fd {0};

    WakeUpFdEvenet* m_wakeup_fd_event {NULL};

    Timer* m_timer { nullptr};

    bool m_stop_flag { false }; // c++11初始化方法

    std::set<int> m_listen_fds;
    
    std::queue<std::function<void()>> m_pending_tasks;
    std::mutex task_mtx_;
};




}

#endif