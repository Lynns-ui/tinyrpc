#include <iostream>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <memory>
#include "../rocket/common/log.h"
#include "../rocket/common/config.h"
#include "../rocket/net/eventloop.h"
#include "../rocket/net/timerevent.h"
#include "../rocket/net/io_thread.h"
#include "../rocket/net/io_thread_pool.h"

void test() {
    
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        ERRORLOG("listen = -1");
        return;
    }
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_port = htons(1317);
    addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &addr.sin_addr);

    int rt = bind(listenfd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    if (rt == -1) {
        ERRORLOG("bind = -1");
        return;
    }
    rt = listen(listenfd, 100);
    if (rt == -1) {
        ERRORLOG("listen = -1");
        return;
    }

    rocket::FdEvent event(listenfd);
    event.listen(rocket::FdEvent::IN_EVENT, [&listenfd](){
        sockaddr_in peer_addr;
        socklen_t addr_len = sizeof(peer_addr);
        memset(&peer_addr, 0, sizeof(peer_addr));
        int clientfd = accept(listenfd, reinterpret_cast<sockaddr*>(&peer_addr), &addr_len);

        INFOLOG("sucess get client fd:[%d], peer addr: [%s:%d]", clientfd, inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port));
    });
    // eventloop->addEpollEvent(&event);

    int i = 0;
    std::shared_ptr<rocket::TimerEvent> timer_event = std::make_shared<rocket::TimerEvent>(
        1000, true, [&i](){
            INFOLOG("trigger timer event, count = %d", i++);
        }
    );
    // eventloop->addTimerEvent(timer_event);
    // eventloop->loop();

    // rocket::IOThread io_thead;
    // rocket::EventLoop* eventloop = io_thead.getEventloop();
    rocket::IOThreadPool io_threadpool(2);
    rocket::IOThread* io_thread = io_threadpool.getIOThread();
    rocket::EventLoop* eventloop = io_thread->getEventloop();
    eventloop->addEpollEvent(&event);
    eventloop->addTimerEvent(timer_event);

    rocket::IOThread* io_thread2 = io_threadpool.getIOThread();
    rocket::EventLoop* eventloop2 = io_thread2->getEventloop();
    eventloop2->addTimerEvent(timer_event);
    // io_thread2->start();

    // io_thread2->join();    // 让该线程执行完再退出，否在会在该对象析构的时候执行
    io_threadpool.start();
    io_threadpool.join();
}

int main() {
    rocket::Config::SetGlobalConfiger("../config/rocket.xml");
    rocket::Logger::InitLogger();

    // rocket::EventLoop* eventloop = new rocket::EventLoop();
    // int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    // if (listenfd == -1) {
    //     ERRORLOG("listen = -1");
    //     return 0;
    // }
    // sockaddr_in addr;
    // memset(&addr, 0, sizeof(addr));

    // addr.sin_port = htons(1316);
    // addr.sin_family = AF_INET;
    // inet_aton("127.0.0.1", &addr.sin_addr);

    // int rt = bind(listenfd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    // if (rt == -1) {
    //     ERRORLOG("bind = -1");
    //     return 0;
    // }
    // rt = listen(listenfd, 100);
    // if (rt == -1) {
    //     ERRORLOG("listen = -1");
    //     return 0;
    // }

    // rocket::FdEvent event(listenfd);
    // event.listen(rocket::FdEvent::IN_EVENT, [&listenfd](){
    //     sockaddr_in peer_addr;
    //     socklen_t addr_len = 0;
    //     memset(&peer_addr, 0, sizeof(peer_addr));
    //     int clientfd = accept(listenfd, reinterpret_cast<sockaddr*>(&peer_addr), &addr_len);

    //     DEBUGLOG("sucess get client fd:[%d], peer addr: [%s:%d]", clientfd, inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port));
    // });
    // eventloop->addEpollEvent(&event);
    test();
}