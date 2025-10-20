#include <string.h>
#include "fdevent.h"

namespace lynns {

FdEvent::FdEvent(int fd) : fd_(fd) { 
    memset(&listen_events_, 0, sizeof(listen_events_));
}

FdEvent::~FdEvent() {

}

std::function<void()> FdEvent::hander(TriggerEvent event_type) {
    if (event_type == IN_EVENT) {
        return read_callback_;
    } else if (event_type == OUT_EVENT) {
        return write_callback_;
    }
}

void FdEvent::listen(TriggerEvent event_type, std::function<void()> callback) {
    if (event_type == IN_EVENT) {
        listen_events_.events |= EPOLLIN;
        read_callback_ = callback;
    } else if (event_type == OUT_EVENT) {
        listen_events_.events |= EPOLLOUT;
        write_callback_ = callback;
    }
    listen_events_.data.ptr = this; // 非常重要，要将event的指针与当前fdevent指针关联
}

}