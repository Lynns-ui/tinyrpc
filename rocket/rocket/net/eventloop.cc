#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <string.h>
#include "eventloop.h"
#include "../common/log.h"
#include "../common/config.h"
#include "../common/util.h"

#define ADD_TO_EPOLL() \
    auto it = m_listen_fds.find(event->getFd());\
    int op = EPOLL_CTL_ADD;\
    if (it != m_listen_fds.end()) {\
        op = EPOLL_CTL_MOD;\
    }\
    epoll_event tmp_event = event->getEpollEvent();\
    int rt = epoll_ctl(m_epoll_fd, op, event->getFd(), &tmp_event);\
    if (rt < 0) {\
        ERRORLOG("failed epoll_ctl when add fd %d, errno=%d, error info = %s", event->getFd(), errno, strerror(errno));\
    }\
    DEBUGLOG("add epoll_event sucess, fd[%d]", event->getFd());\

#define DEL_FROM_EPOLL();\
    auto it = m_listen_fds.find(event->getFd());\
    int op = EPOLL_CTL_DEL;\
    if (it == m_listen_fds.end()) {\
        return;\
    }\
    epoll_event tmp_event = event->getEpollEvent();\
    int rt = epoll_ctl(m_epoll_fd, op, event->getFd(), &tmp_event);\
    if (rt < 0) {\
        ERRORLOG("failed epoll_ctl when delete fd %d, errno=%d, error info = %s", event->getFd(), errno, strerror(errno));\
    }\
    DEBUGLOG("del epoll_event sucess, fd[%d]", event->getFd());\

namespace rocket{

static thread_local EventLoop* t_current_eventloop = NULL;
static int g_epoll_max_timeout = 10000;
static int g_epoll_max_events = 10;


EventLoop::EventLoop() {
    if (t_current_eventloop) {
        ERRORLOG("failed to create event loop, this thread has created event loop");
        exit(0);
    }
    thread_id = getThreadId();

    m_epoll_fd = epoll_create(10);
    if (m_epoll_fd == -1) {
        ERRORLOG("failed to create event loop, epoll_create error, error info[%d]", errno);
        exit(0);
    }

    m_wakeup_fd = eventfd(0, EFD_NONBLOCK);
    if (m_wakeup_fd < 0) {
        ERRORLOG("failed to create event loop, eventfd create error, error info[%d]", errno);
        exit(0);
    }

    epoll_event event;
    event.events = EPOLLIN; // 监听读事件
    int rt = epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_wakeup_fd, &event);
    if (rt == -1) {
        ERRORLOG("failed to create event loop, epoll_ctl add error, error info[%d]", errno);
        exit(0);
    }

    INFOLOG("sucess create event loop in thread %d", thread_id);
    t_current_eventloop = this;
}

EventLoop::~EventLoop() {

}

void EventLoop::loop() {
    while (!m_stop_flag) {
        // 先执行任务
        std::unique_lock<std::mutex> locker(task_mtx_);
        std::queue<std::function<void()>> tmp_tasks;
        m_pending_tasks.swap(tmp_tasks);    // 当前的任务队列与临时队列交换
        locker.unlock();
        
        while (!tmp_tasks.empty()) {
            auto it = tmp_tasks.front();
            tmp_tasks.pop();
            it();
        }
        int timeout = g_epoll_max_timeout;
        epoll_event result_events[g_epoll_max_events];
        int rt = epoll_wait(m_epoll_fd, result_events, g_epoll_max_events, timeout);

        if (rt < 0) {
            ERRORLOG("epoll_wait error, errno=%d", errno);
        } else {
            for (int i = 0; i < rt; i++) {
                epoll_event trigger_event = result_events[i];
                FdEvent* fd_event =static_cast<FdEvent*>(trigger_event.data.ptr);
                if (fd_event == NULL) {
                    continue;
                }
                if (trigger_event.events | EPOLLIN) {
                    addTask(fd_event->handler(FdEvent::IN_EVENT));
                } else if (trigger_event.events | EPOLLOUT) {
                    addTask(fd_event->handler(FdEvent::OUT_EVENT));
                }

            }
        }

    }
}

void EventLoop::wakeup() {

}

void EventLoop::stop() {
    m_stop_flag = true;
}

void EventLoop::dealWakeup() {

}

void EventLoop::addEpollEvent(FdEvent* event) {
    if (isInLoopThread()) {
        ADD_TO_EPOLL();
    } else {
        auto cb = [this, event]() {
            ADD_TO_EPOLL();
        };
        addTask(cb, true);
    }
}

void EventLoop::delEpollEvent(FdEvent* event) {
    if (isInLoopThread()) {
        DEL_FROM_EPOLL();
    } else {
        auto cb = [this, event]() {
            DEL_FROM_EPOLL();
        };
        addTask(cb, true);
    }
}

bool EventLoop::isInLoopThread() {
    return getThreadId() == thread_id;
}

void EventLoop::addTask(std::function<void()> cb, bool is_wake_up/*= false*/) {
    {
        std::lock_guard<std::mutex> locker(task_mtx_);
        m_pending_tasks.push(cb);
    }

    if (is_wake_up) {
       wakeup(); 
    }
}


}
