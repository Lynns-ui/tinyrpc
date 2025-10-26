#ifndef ROCKT_NET_TCP_ACCEPTOR_H
#define ROCKT_NET_TCP_ACCEPTOR_H

#include <memory>
#include "net_addr.h"

namespace rocket {

class TcpAcceptor {
public:
    typedef std::shared_ptr<TcpAcceptor> s_ptr;

    TcpAcceptor(NetAddr::s_ptr local_addr);

    ~TcpAcceptor();

    std::pair<int, NetAddr::s_ptr> accept();

    int getListenFd();
private:
    // 服务端监听的地址：addr -> ip : port
    NetAddr::s_ptr m_local_addr;
    // listenfd 监听套接字
    int m_listenfd {-1};
    int m_family {-1};

};



}


#endif