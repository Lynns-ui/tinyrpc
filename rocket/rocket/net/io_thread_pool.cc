#include "io_thread_pool.h"

namespace rocket {

IOThreadPool::IOThreadPool(size_t poolSize) : m_size(poolSize), m_index(0){
    m_iothread_pools.reserve(poolSize);
    for (size_t i = 0; i < poolSize; i++) {
        // DEBUGLOG("create")
        m_iothread_pools.emplace_back(new IOThread());
    }
}

IOThreadPool::~IOThreadPool() {

}

void IOThreadPool::start() {
    for (size_t i = 0; i < m_iothread_pools.size(); i++) {
        m_iothread_pools[i]->start();
    }
}

IOThread* IOThreadPool::getIOThread() {
    if (m_iothread_pools.empty()) {
        return NULL;
    }
    if (m_index == m_iothread_pools.size() || m_index == -1) {
        m_index = 0;
    }
    // 每获取一个io线程之后都要更新index
    return m_iothread_pools[m_index++];
}

void IOThreadPool::join() {
    for (size_t i = 0; i < m_iothread_pools.size(); i++) {
        m_iothread_pools[i]->join();
    }
}

// void IOThreadPool::returnIOThread(IOThread* iothread) {

// }

}