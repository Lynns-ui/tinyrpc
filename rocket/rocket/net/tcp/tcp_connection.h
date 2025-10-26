#ifndef ROCKET_NET_TCP_CONNECTION_H
#define ROCKET_NET_TCP_CONNECTION_H

#include <memory>
#include "tcp_buffer.h"
#include "net_addr.h"
#include "../io_thread.h"

namespace rocket {

class TcpConnection {
public:
    typedef std::shared_ptr<TcpConnection> s_ptr;

    enum TcpState {
        NotConnected = 1, 
        Connected = 2,
        HalfClosing = 3,    // 半连接
        Closed = 4,
    };

    TcpConnection(IOThread* io_thread, int client_fd, int buffer_size, NetAddr::s_ptr peer_addr);

    ~TcpConnection();

    void onRead();

    void excute();

    void onWrite();

    void setState(const TcpConnection::TcpState state);

    TcpConnection::TcpState getState();

    void clear();

    void shutdown();    // 服务器主动关闭连接
private:
    NetAddr::s_ptr m_local_addr;
    NetAddr::s_ptr m_peer_addr;

    TcpBuffer::s_ptr m_in_buffer;   // 接受缓冲区
    TcpBuffer::s_ptr m_out_buffer;  // 发送缓冲区

    int m_fd;

    IOThread* m_io_thread { nullptr };  // 持有该连接的io线程
    
    FdEvent* m_fd_event { nullptr };    // 

    TcpState m_state;

};


}


#endif