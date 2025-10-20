#ifndef LYNNS_NET_FDEVENT_H
#define LYNNS_NET_FDEVENT_H

#include <functional>
#include <sys/epoll.h>


namespace lynns {

class FdEvent {
public:
    enum TriggerEvent {
        IN_EVENT = EPOLLIN,
        OUT_EVENT = EPOLLOUT,
    };

    FdEvent(int fd);

    ~FdEvent();

    std::function<void()> hander(TriggerEvent event_type);

    void listen(TriggerEvent event_type, std::function<void()> callback);

    int getFd() const {
        return fd_;
    }

    epoll_event getEpollEvent() const {
        return listen_events_;
    }


protected:
    int fd_ {-1};

    epoll_event listen_events_;

    std::function<void()> read_callback_;
    std::function<void()> write_callback_;
};



}



#endif