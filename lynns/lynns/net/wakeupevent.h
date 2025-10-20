#ifndef LYNNS_NET_WAKEUP_EVENT_H
#define LYNNS_NET_WAKEUP_EVENT_H

#include "fdevent.h"

namespace lynns{

class WakeUpEvent : public FdEvent {
public:
    WakeUpEvent(int fd) : FdEvent(fd) { }
    ~WakeUpEvent() = default;

    void wakeUp();

private:

};

}




#endif