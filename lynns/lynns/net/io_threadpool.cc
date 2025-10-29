#include "io_threadpool.h"
#include "../common/log.h"

namespace lynns{

IOThreadPool::IOThreadPool(int size) : startIndex(0), size_(size) {
    thread_pools_.reserve(size);
    for (int i = 0; i < size; i++) {
        // auto iothread = std::make_shared<IOThread>();
        thread_pools_.emplace_back(std::make_shared<IOThread>());
    }
}

IOThreadPool::~IOThreadPool() {

}

void IOThreadPool::start() {
    for (int i = 0; i < size_; i++) {
        thread_pools_[i]->start();
    }
}

IOThread::s_ptr IOThreadPool::getIOThread() {
    if (thread_pools_.empty()) {
        return nullptr;
    }
    if (startIndex == thread_pools_.size()) {
        startIndex = 0;
    }
    return thread_pools_[startIndex++];
}


}