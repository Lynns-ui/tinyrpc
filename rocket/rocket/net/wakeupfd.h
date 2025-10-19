#ifndef ROCKET_NET_WAKEUPFD_H
#define ROCKET_NET_WAKEUPFD_H

#include "fdevent.h"

namespace rocket {

class WakeUpFdEvenet : public FdEvent {
public:
    WakeUpFdEvenet(int fd);

    ~WakeUpFdEvenet();

    void wakeup();

private:



};




}

#endif