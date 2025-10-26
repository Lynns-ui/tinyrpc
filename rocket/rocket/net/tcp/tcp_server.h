#ifndef ROCKET_NET_TCPSERVER_H
#define ROCKET_NET_TCPSERVER_H

#include <set>
#include "tcp_acceptor.h"
#include "net_addr.h"
#include "tcp_buffer.h"
#include "tcp_connection.h"
#include "../eventloop.h"
#include "../io_thread_pool.h"

namespace rocket {

class TcpServer {
public:
    TcpServer(NetAddr::s_ptr local_addr);

    ~TcpServer();

    void start();

private:
    void init();

    // 当有新客户端链接之后，需要执行
    void onAccept();

private:
    TcpAcceptor::s_ptr m_acceptor;

    NetAddr::s_ptr m_local_addr;    // 本地监听地址

    EventLoop* m_main_eventloop { NULL };   // mainReactor

    IOThreadPool* m_io_threadpool {NULL};   // subReactor池

    FdEvent* m_listen_fdevent;

    std::set<TcpConnection::s_ptr> m_client;

    // 当前连接的数量
    int m_client_counts {0};
};



}

#endif