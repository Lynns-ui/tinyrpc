#include <string.h>
#include "util.h"

namespace rocket {

static int g_pid = 0;
static thread_local int g_threadid = 0;

pid_t getPid() {
    if (g_pid != 0) {
        return g_pid;
    }
    g_pid = getpid();
    return g_pid;
}

pid_t getThreadId() {
    if (g_threadid != 0) {
        return g_threadid;
    }
    g_threadid = syscall(SYS_gettid);
    return g_threadid;
}

int64_t getNowMs() {
    timeval val;
    gettimeofday(&val, NULL);
    return val.tv_sec * 1000 + val.tv_usec / 1000;
}

int32_t getInt32FromNetByte(const char* buff) {
    int32_t re;
    memcpy(&re, buff, sizeof(int32_t)); // sizeof(int32_t)是 4 个字节的
    // 将网络字节序转化为主机字节序
    return ntohl(re);
}
}
