#ifndef LYNNS_NET_EVENTLOOP_H
#define LYNNS_NET_EVENTLOOP_H

#include <sys/epoll.h>
#include <functional>
#include <queue>
#include <mutex>
#include <set>
#include "fdevent.h"
#include "wakeupevent.h"
#include "../common/util.h"

namespace lynns {

class EventLoop {
public:

    EventLoop();

    ~EventLoop();

    void loop();

    void addEpollEvent(FdEvent* fdevent);

    void delEpollEvent(FdEvent* fdevent);

    bool isLocalLoop();

    void addTask(std::function<void()> callback, bool is_wake_up = false);

    void wakeup();

    void initWakupFdEvent();

private:

    int epoll_fd_ {0};

    bool is_close_ {true};

    int listen_fd_ {0};

    int wakeup_fd_ {0};

    WakeUpEvent* wakeup_event_;

    pid_t thread_id_ {0};

    std::set<int> listen_fds_;

    std::queue<std::function<void()>> tasks_;
    std::mutex mtx_;
};

}


#endif