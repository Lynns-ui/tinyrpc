#ifndef LYNNS_NET_THREAD_POOL_H
#define LYNNS_NET_THREAD_POOL_H

#include <vector>
#include "io_thread.h"

namespace lynns {

class IOThreadPool {
public:
    IOThreadPool(int size);

    ~IOThreadPool();

    void start();

    IOThread::s_ptr getIOThread();

private:
    std::vector<IOThread::s_ptr> thread_pools_;

    int startIndex;

    int size_;
};


}

#endif