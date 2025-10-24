#include "util.h"

namespace lynns {
    
static int g_pid = 0;
static thread_local int g_threadid = 0;

pid_t getPid() {
    if (g_pid == 0) {
        g_pid = getpid();
    }
    return g_pid;
}

pid_t getThreadId() {
    if (g_threadid == 0) {
        g_threadid = syscall(SYS_gettid);
    }
    return g_threadid;
}

int64_t getNowMs() {
    timeval val;
    gettimeofday(&val, NULL);
    return val.tv_sec * 1000 + val.tv_usec / 1000;
}

}