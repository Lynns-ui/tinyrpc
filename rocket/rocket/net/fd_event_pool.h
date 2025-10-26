#ifndef ROCKRT_FD_EVENTPOOL_H
#define ROCKRT_FD_EVENTPOOL_H

#include <vector>
#include <mutex>
#include "fdevent.h"

namespace rocket {

/* 相当于一个池子，对端关闭的时候把fd关闭后还回去  */
class FdEventPool {
public:
    FdEventPool(int size);

    ~FdEventPool();

    FdEvent* getFdEvent(int fd);    // fd关联的fdevent对象

    static FdEventPool* g_fdEventPool;
    static FdEventPool* GetFdEventPool();
private:
    int m_size {0};
    std::vector<FdEvent*> m_fdpool;
    std::mutex m_mtx;
};




}

#endif