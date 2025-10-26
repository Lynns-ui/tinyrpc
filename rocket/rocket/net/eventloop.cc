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
    m_listen_fds.insert(event->getFd());\
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
    m_listen_fds.erase(event->getFd());\
    DEBUGLOG("del epoll_event sucess, fd[%d]", event->getFd());\

namespace rocket{

// 当前线程的eventloop对象
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

    initWakeUpFdEvent();
    initTimer();

    INFOLOG("sucess create event loop in thread %d", thread_id);
    t_current_eventloop = this;
}

EventLoop::~EventLoop() {
    close(m_epoll_fd);
    if (m_wakeup_fd_event) {
        delete m_wakeup_fd_event;
        m_wakeup_fd_event = NULL;
    }
    if (m_timer) {
        delete m_timer;
        m_timer = nullptr;
    }
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
            if (it) {
                DEBUGLOG("start epollloop task!");
                it();
            }
        }
        
        // 如果有定时任务需要执行那么怎么执行
        // 1. 怎么判断一个定时任务需要执行？ now() > TimerEvent.arrive_time
        // 2. 怎么添加定时器放进 怎么arrive_time 准确的返回 loop循环监听arriv_time

        int timeout = g_epoll_max_timeout;
        epoll_event result_events[g_epoll_max_events];

        DEBUGLOG("now begin to epoll_wait");
        int rt = epoll_wait(m_epoll_fd, result_events, g_epoll_max_events, timeout);
        DEBUGLOG("now end epoll_wait, rt = %d", rt);

        if (rt < 0) {
            ERRORLOG("epoll_wait error, errno=%d, error info is [%s]", errno, strerror(errno));
        } else {
            for (int i = 0; i < rt; i++) {
                epoll_event trigger_event = result_events[i];
                FdEvent* fd_event =static_cast<FdEvent*>(trigger_event.data.ptr);
                if (fd_event == NULL) {
                    continue;
                }
                if (trigger_event.events & EPOLLIN) {
                    DEBUGLOG("fd %d trigger EPOLLIN event", fd_event->getFd());
                    addTask(fd_event->handler(FdEvent::IN_EVENT));
                } else if (trigger_event.events & EPOLLOUT) {
                    DEBUGLOG("fd %d trigger EPOLLOUT event", fd_event->getFd());
                    addTask(fd_event->handler(FdEvent::OUT_EVENT));
                }
            }
        }
    }
}

void EventLoop::wakeup() {
    INFOLOG("Wake UP");
    m_wakeup_fd_event->wakeup();
}

void EventLoop::stop() {
    m_stop_flag = true;
    wakeup();
}

void EventLoop::dealWakeup() {

}

void EventLoop::initWakeUpFdEvent() {
    m_wakeup_fd = eventfd(0, EFD_NONBLOCK);
    if (m_wakeup_fd < 0) {
        ERRORLOG("failed to create event loop, eventfd create error, error info[%d]", errno);
        exit(0);
    }

    m_wakeup_fd_event = new WakeUpFdEvenet(m_wakeup_fd);
    m_wakeup_fd_event->listen(FdEvent::TriggerEvent::IN_EVENT, [this]() {
        char buf[8];
        while (read(m_wakeup_fd, buf, 8) != -1 && errno != EAGAIN) {
        
        }
        DEBUGLOG("read full bytes from wakeup fd[%d]", m_wakeup_fd);
    });

    addEpollEvent(m_wakeup_fd_event);
}

void EventLoop::initTimer() {
    m_timer = new Timer();
    addEpollEvent(m_timer);
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

void EventLoop::addTimerEvent(TimerEvent::s_ptr event) {
    m_timer->addTimerEvent(event);
}

EventLoop* EventLoop::GetCurrentEventloop() {
    if (t_current_eventloop) {
        return t_current_eventloop;
    }
    t_current_eventloop = new EventLoop();
    return t_current_eventloop;
}

}
