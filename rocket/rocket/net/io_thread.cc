#include <assert.h>
#include "io_thread.h"
#include "../common/util.h"

namespace rocket {

IOThread::IOThread() {

    int rt = sem_init(&m_init_sem, 0, 0);
    assert(rt == 0);

    rt = sem_init(&m_start_sem, 0, 0);
    assert(rt == 0);
    
    // 创建几个线程，并且在线程里面构造eventloop
    // 传入指针，在Main中还可以取出来 这里新线程会执行Main函数
    pthread_create(&m_thread, NULL, &IOThread::Main, this);

    // 一直wait，直到新县城执行完Main函数的前置
    // 主线程才能
    rt = sem_wait(&m_init_sem);
    assert(rt == 0);
    DEBUGLOG("IOThread %d create sucess", m_thread_id);
}

IOThread::~IOThread() {

    // m_eventloop->stop();
    sem_destroy(&m_init_sem);
    sem_destroy(&m_start_sem);

    pthread_join(m_thread, NULL);

    if (m_eventloop) {
        delete m_eventloop;
        m_eventloop = NULL;
    }

}

void* IOThread::Main(void* arg) {
    IOThread* thread = static_cast<IOThread*>(arg);

    thread->m_eventloop = new EventLoop();
    thread->m_thread_id = getThreadId();


    // 唤醒等待的线程
    // 信号量
    sem_post(&thread->m_init_sem);

    // 让IO线程等待，直到我们主动的启动
    // 新线程必须阻塞在这里，直到主动发起一个启动信号，否则新线程调用完函数之后会销毁
    DEBUGLOG("wait IOthread %d loop", thread->m_thread_id);
    sem_wait(&thread->m_start_sem); // 新的IO线程会阻塞
    DEBUGLOG("start IOthread %d loop", thread->m_thread_id);

    thread->m_eventloop->loop();

    return NULL;
}

void IOThread::start() {
    sem_post(&m_start_sem);
}

EventLoop* IOThread::getEventloop() {
    return m_eventloop;
}

void IOThread::join() { 
    pthread_join(m_thread, NULL);
}


}