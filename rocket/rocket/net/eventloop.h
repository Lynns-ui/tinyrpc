#ifndef ROCKET_NET_EVENTLOOP_H
#define ROCKET_NET_EVENTLOOP_H

#include <pthread.h>
#include <set>
#include <functional>
#include <queue>
#include <mutex>
#include "fdevent.h"
#include "wakeupfd.h"

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

private:
    void dealWakeup();

    void initWakeUpFdEvent();
    
    pid_t thread_id {0};
    int m_epoll_fd {0};
    int m_wakeup_fd {0};

    WakeUpFdEvenet* m_wakeup_fd_event {NULL};

    bool m_stop_flag { false }; // c++11初始化方法

    std::set<int> m_listen_fds;
    
    std::queue<std::function<void()>> m_pending_tasks;
    std::mutex task_mtx_;
};




}

#endif