#ifndef ROCKET_NET_IOTHREAD_H
#define ROCKET_NET_IOTHREAD_H

#include <pthread.h>
#include <semaphore.h>
#include "../common/log.h"
#include "eventloop.h"

namespace rocket {

class IOThread {
public:
    IOThread();

    ~IOThread();

    static void* Main(void* arg);

    EventLoop* getEventloop();

private:
    pid_t m_thread_id {-1};     
    pthread_t m_thread;
    EventLoop* m_eventloop {NULL};

    sem_t m_init_sem;

};


}


#endif