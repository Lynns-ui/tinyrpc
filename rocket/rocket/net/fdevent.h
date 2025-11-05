#ifndef ROCKRT_NET_FDEVENT_H
#define ROCKRT_NET_FDEVENT_H

#include <functional>
#include <sys/epoll.h>

namespace rocket {

class FdEvent {
public:
    enum TriggerEvent {
        IN_EVENT = EPOLLIN,
        OUT_EVENT = EPOLLOUT,
        ERR_EVENT = EPOLLERR
    };

    FdEvent(int fd);

    FdEvent();

    ~FdEvent();

    std::function<void()> handler(TriggerEvent event_type);

    void listen(TriggerEvent event_type, std::function<void()> callback, std::function<void()> err_callback = nullptr);

    void cancel(TriggerEvent event_type);    // 取消监听

    int getFd() const {
        return m_fd;
    }

    epoll_event getEpollEvent() const{
        return m_listen_events;
    }

    void setNonBlock();
protected:
    int m_fd {-1};

    epoll_event m_listen_events;

    std::function<void()> m_read_callback;
    std::function<void()> m_write_callback;
    std::function<void()> m_error_callback {nullptr};
};

}


#endif