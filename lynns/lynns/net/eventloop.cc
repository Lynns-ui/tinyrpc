#include <errno.h>
#include <string.h>
#include <sys/eventfd.h>
#include "eventloop.h"
#include "../common/log.h"

namespace lynns {

#define ADD_TO_EPOLL() \
    int fd = fdevent->getFd();\
    int op = EPOLL_CTL_ADD;\
    if (listen_fds_.find(fd) != listen_fds_.end()) {\
        op = EPOLL_CTL_MOD;\
    }\
    auto event = fdevent->getEpollEvent();\
    int rt = epoll_ctl(epoll_fd_, op, fd, &event);\
    if (rt < 0) {\
        ERRORLOG("failed to add fd:%d to epoll, error info: %s", fd, strerror(errno));\
    }\
    DEBUGLOG("add epoll_event success, fd[%d]", fd);\

#define DEL_FROM_EPOLL()\
    int fd = fdevent->getFd();\
    int op = EPOLL_CTL_DEL;\
    if (listen_fds_.find(fd) == listen_fds_.end()) {\
        return;\
    }\
    auto event = fdevent->getEpollEvent();\
    int rt = epoll_ctl(epoll_fd_, op, fd, &event);\
    if (rt < 0) {\
        ERRORLOG("failed delete fd:%d from epoll, error info: %s", fd, strerror(errno));\
    }\
    DEBUGLOG("delete epoll_event success, fd[%d]", fd);\

static thread_local EventLoop* t_current_thread = nullptr;
static int g_timeout = 10000;
static int g_maxfds = 1024;

EventLoop::EventLoop() : is_close_(false) {
    if (t_current_thread) {
        ERRORLOG("failed to create eventloop, this thread has created eventloop!");
        exit(0);
    }
    thread_id_ = getThreadId();

    epoll_fd_ = epoll_create(10);
    if (epoll_fd_ < 0) {
        ERRORLOG("failed to create eventloop, epoll_fd create failed");
        exit(0);
    }

    initWakupFdEvent();
    initTimer();

    INFOLOG("success create eventloop in thread %d", thread_id_);
    t_current_thread = this;
}

EventLoop::~EventLoop() {
    close(epoll_fd_);
    if (wakeup_event_) {
        delete wakeup_event_;
        wakeup_event_ = nullptr;
    }
    is_close_ = true;
}

void EventLoop::initWakupFdEvent() {
    wakeup_fd_ = eventfd(0, EFD_NONBLOCK);
    if (wakeup_fd_ < 0) {
        ERRORLOG("failed to create event loop, eventfd create error, error info[%d]", errno);
        exit(0);
    }
    wakeup_event_ = new WakeUpEvent(wakeup_fd_);
    wakeup_event_->listen(FdEvent::IN_EVENT, [this](){
        char buff[8];
        while (read(wakeup_fd_, buff, 8) != -1 || errno != EAGAIN) {

        }
        DEBUGLOG("read full bytes from wakeup fd[%d]", wakeup_fd_);
    });

    addEpollEvent(wakeup_event_);
}

void EventLoop::initTimer() {
    timer_ = new Timer();
    addEpollEvent(timer_);
}

void EventLoop::addTimerEvent(TimerEvent::s_ptr event) {
    timer_->addTimerEvent(event);
}

void EventLoop::loop() {
    while (!is_close_) {
        std::unique_lock<std::mutex> locker(mtx_);
        std::queue<std::function<void()>> tmp;
        tasks_.swap(tmp);
        locker.unlock();

        while (!tmp.empty()) {
            auto task = tmp.front();
            tmp.pop();
            if (task) {
                DEBUGLOG("start task");
                task();
            }
        }

        // 获取监听描述符
        epoll_event events[g_maxfds];
        DEBUGLOG("start epoll_wait!");
        int n = epoll_wait(epoll_fd_, &events[0], g_maxfds, g_timeout);
        DEBUGLOG("end epoll_wait!, n = %d", n);
        
        if (n < 0) {
            ERRORLOG("epoll_wait error, error info is [%s]", strerror(errno));
        } else {
            for (int i = 0; i < n; i++) {
                epoll_event trigger_event = events[i];
                FdEvent* fdevent = static_cast<FdEvent*>(trigger_event.data.ptr);
                if (fdevent == nullptr) {
                    continue;
                }
                if (trigger_event.events & EPOLLIN) {
                    DEBUGLOG("trigger EPOLLIN event");
                    addTask(fdevent->hander(FdEvent::IN_EVENT));
                } else if (trigger_event.events & EPOLLOUT) {
                    DEBUGLOG("trigger EPOLLOUT event");
                    addTask(fdevent->hander(FdEvent::OUT_EVENT));
                }
            }
        }
    }
}

void EventLoop::addTask(std::function<void()> callback, bool is_wake_up /* =false */) {
    {
        std::lock_guard<std::mutex> locker(mtx_);
        tasks_.push(callback);
    }

    if (is_wake_up) {
        wakeup();
    }
}

void EventLoop::wakeup() {
    INFOLOG("Wake UP");
    wakeup_event_->wakeUp();
}

bool EventLoop::isLocalLoop() {
    return getThreadId() == thread_id_;
}

void EventLoop::addEpollEvent(FdEvent* fdevent) {
    if (isLocalLoop()) {
        ADD_TO_EPOLL();
    } else {
        addTask([fdevent, this](){
            ADD_TO_EPOLL();
        }, true);
    }
}

void EventLoop::delEpollEvent(FdEvent* fdevent) {
    if (isLocalLoop()) {
        DEL_FROM_EPOLL();
    } else {
        addTask([this, fdevent](){
            DEL_FROM_EPOLL();
        }, true);
    }
}


}

