#ifndef ROCKT_NET_TCP_CLIENT_H
#define ROCKT_NET_TCP_CLIENT_H

#include "../abstract_protocol.h"
#include "net_addr.h"
#include "../eventloop.h"
#include "tcp_connection.h"

namespace rocket {

class TcpClient{
public:
    TcpClient(NetAddr::s_ptr peer_local);

    ~TcpClient();

    // 异步进行connect, connect成功，执行回调函数
    void connect(std::function<void()> done);

    // 异步发送msg  
    // 如果发送成功，调用done 函数的入参就是message
    void writeMsg(AbstractProtocol::s_ptr message, std::function<void(AbstractProtocol::s_ptr)> done);

    // 异步读取message
    void readMsg(AbstractProtocol::s_ptr message, std::function<void(AbstractProtocol::s_ptr)> done);

private:
    NetAddr::s_ptr m_peer_addr;
    EventLoop* m_event_loop {NULL};

    int m_fd {-1};
    FdEvent* m_fd_event {NULL};

    TcpConnection::s_ptr m_connection;
};



}

#endif