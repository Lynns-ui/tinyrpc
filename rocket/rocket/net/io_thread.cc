#include <assert.h>
#include "io_thread.h"
#include "../common/util.h"

namespace rocket {

IOThread::IOThread() {

    int rt = sem_init(&m_init_sem, 0, 0);
    assert(rt == 0);
    // 创建几个线程，并且在线程里面构造eventloop
    // 传入指针，在Main中还可以取出来
    pthread_create(&m_thread, NULL, &IOThread::Main, this);

    // 一直wait，直到新县城执行完Main函数的前置
    // 主线程才能
    rt = sem_wait(&m_init_sem);
    assert(rt == 0);
    DEBUGLOG("IOThread %d create sucess", m_thread_id);
}

IOThread::~IOThread() {

    m_eventloop->stop();
    sem_destroy(&m_init_sem);

    pthread_join(m_thread, NULL);

    if (m_eventloop) {
        delete m_eventloop;
        m_eventloop = NULL;
    }

}


void* IOThread::Main(void* arg) {
    IOThread* thread = static_cast<IOThread*>(arg);

    thread->m_eventloop = new EventLoop();
    // thread->m_thread_id = getThreadId();


    // 唤醒等待的线程
    // 信号量
    sem_post(&thread->m_init_sem);
    thread->m_eventloop->loop();

    return NULL;
}

EventLoop* IOThread::getEventloop() {
    return m_eventloop;
}


}