#include <assert.h>
#include "io_thread.h"
#include "../common/log.h"
#include "../common/util.h"

namespace lynns {

IOThread::IOThread() {
    int rt = sem_init(&semid_, 0, 0);
    assert(rt == 0);

    // 增加初始化的信号量
    rt = sem_init(&sem_init_id_, 0 ,0);
    assert(rt == 0);

    thread_ = std::thread(&IOThread::Main, this);

    // 信号量阻塞在这里，原因是让每个io线程构造函数完成
    // 都要保证在Main函数中有eventloop构造完成
    sem_wait(&sem_init_id_);
    DEBUGLOG("IOThread %d create success", thread_id_);
}

IOThread::~IOThread() {
    sem_destroy(&semid_);

    if (thread_.joinable()) {
        thread_.join();
    }
}

void IOThread::Main() {
    eventloop_ = std::make_shared<EventLoop>();
    thread_id_ = getThreadId();

    sem_post(&sem_init_id_);

    // 信号量为0，此时-1之后为负数，线程阻塞到当前不能往下执行，直到调用start
    // 信号量重新加一，
    sem_wait(&semid_);

    eventloop_->loop();
}

void IOThread::start() {
    // 调用post，信号量+1， 当信号量
    sem_post(&semid_);
}

EventLoop::s_ptr IOThread::getEventLoop() {
    return eventloop_;
}

}