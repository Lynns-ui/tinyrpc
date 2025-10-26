#include "fd_event_pool.h"
#include "../common/log.h"

namespace rocket {

FdEventPool* FdEventPool::g_fdEventPool = nullptr;

FdEventPool* FdEventPool::GetFdEventPool() {
    if (g_fdEventPool != nullptr) {
        return g_fdEventPool;
    }
    // 先写死
    g_fdEventPool = new FdEventPool(128);
    return g_fdEventPool;
}

FdEventPool::FdEventPool(int size) : m_size(size) {
    for (int i = 0; i < size; i++) {
        m_fdpool.push_back(new FdEvent(i));
    }
}

FdEventPool::~FdEventPool() {
    for (int i = 0; i < m_size; i++) {
        if (m_fdpool[i] != nullptr) {
            delete m_fdpool[i];
            m_fdpool[i] = nullptr;
        }
    }
}

FdEvent* FdEventPool::getFdEvent(int fd) {
    std::lock_guard<std::mutex> locker(m_mtx);
    if (fd < m_fdpool.size()) {
        return m_fdpool[fd];
    }

    int new_size = (int)(fd * 1.5);
    for (int i = m_fdpool.size(); i < new_size; i++) {
        m_fdpool.push_back(new FdEvent(i));
    }
    return m_fdpool[fd];
}


}