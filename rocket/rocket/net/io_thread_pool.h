#ifndef ROCKET_NET_IOTHREADPOOL_H
#define ROCKET_NET_IOTHREADPOOL_H

#include <vector>
#include <pthread.h>
#include "../common/log.h"
#include "../common/util.h"
#include "io_thread.h"

namespace rocket {

class IOThreadPool {
public:
    IOThreadPool(size_t poolSize);

    ~IOThreadPool();

    void start();

    IOThread* getIOThread();

    void join();

    // void returnIOThread(IOThread* iothread);

private:
    size_t m_size {0};

    std::vector<IOThread*> m_iothread_pools;
    // std::mutex mtx_;

    // 轮询
    int m_index;    // 下一次可以获取的io线程
};


}


#endif