#include <unistd.h>
#include "wakeupevent.h"
#include "../common/log.h"

namespace lynns {

void WakeUpEvent::wakeUp() {
    char buff[8] = {'a'};
    int rt = write(fd_, buff, 8);
    if (rt != 8) {
        ERRORLOG("write to wakeup fd less than 8 bytes, fd[%d]", fd_);
    }
    DEBUGLOG("success read 8 bytes");
}


}