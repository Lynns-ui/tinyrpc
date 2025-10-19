#include <unistd.h>
#include "wakeupfd.h"
#include "../common/log.h"

namespace rocket {

WakeUpFdEvenet::WakeUpFdEvenet(int fd): FdEvent(fd) {
    
}

WakeUpFdEvenet::~WakeUpFdEvenet() {
    
}

void WakeUpFdEvenet::wakeup() {
    char buf[8] = {'a'};
    int rt = write(m_fd, buf, 8);
    if (rt != 8) {
        ERRORLOG("write to wakeup fd less than 8 bytes, fd[%d]", m_fd);
    }

}

}