#ifndef LYNNS_COMMON_UTIL_H
#define LYNNS_COMMON_UTIL_H

#include <unistd.h>
#include <sys/syscall.h>

namespace lynns {

pid_t getPid();

pid_t getThreadId();

}

#endif