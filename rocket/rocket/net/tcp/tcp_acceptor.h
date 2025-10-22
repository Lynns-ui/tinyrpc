#ifndef ROCKT_NET_TCP_ACCEPTOR_H
#define ROCKT_NET_TCP_ACCEPTOR_H

#include "net_addr.h"

namespace rocket {

class TcpAcceptor {
public:
    TcpAcceptor(NetAddr::s_ptr local_addr);

    ~TcpAcceptor();

    int accept();

private:
    // 服务端监听的地址：addr -> ip : port
    NetAddr::s_ptr m_local_addr;
    // listenfd 监听套接字
    int m_listenfd {-1};
    int m_family {-1};

};



}


#endif