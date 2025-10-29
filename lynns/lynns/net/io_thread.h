#ifndef LYNNS_NET_IOTHREAD_H
#define LYNNS_NET_IOTHREAD_H

#include <thread>
#include <semaphore.h>
#include <memory>
#include "eventloop.h"

namespace lynns {

class IOThread {
public:
    typedef std::shared_ptr<IOThread> s_ptr;

    IOThread();

    ~IOThread();

    void Main();

    void start();

    EventLoop::s_ptr getEventLoop();

private:
    EventLoop::s_ptr eventloop_;

    pid_t thread_id_;
    
    std::thread thread_;

    sem_t semid_;
    sem_t sem_init_id_;

};


}



#endif