#ifndef ROCKET_NET_TCP_CONNECTION_H
#define ROCKET_NET_TCP_CONNECTION_H

#include <memory>
#include <queue>
#include <functional>
#include <map>
#include "tcp_buffer.h"
#include "net_addr.h"
#include "../io_thread.h"
#include "../coder/abstract_protocol.h"
#include "../coder/abstract_coder.h"
#include "../coder/tinypb_coder.h"
#include "../coder/tinypb_protocol.h"
#include "../rpc/rpc_dispatcher.h"

namespace rocket {

class RpcDispatcher;

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
    typedef std::shared_ptr<TcpConnection> s_ptr;

    enum TcpState {
        NotConnected = 1, 
        Connected = 2,
        HalfClosing = 3,    // 半连接
        Closed = 4,
    };

    enum TcpConnectionType {
        TcpConnectionByServer = 1, // 作为服务端使用，代表跟对端客户端的连接
        TcpConnectionByClient = 2, // 作为客户端使用，代表跟对端服务端的连接
    };


    TcpConnection(EventLoop* event_loop, int client_fd, int buffer_size, NetAddr::s_ptr local_addr, NetAddr::s_ptr peer_addr, 
        TcpConnectionType type = TcpConnectionByServer);

    ~TcpConnection();

    void onRead();

    void excute();

    void onWrite();

    void setState(const TcpConnection::TcpState state);

    void setConnectionType(const TcpConnection::TcpConnectionType type);

    TcpConnection::TcpState getState();

    void clear();

    void shutdown();    // 服务器主动关闭连接

    // 启动监听可写事件
    void listenWrite();

    void listenRead();

    void pushSendMsg(AbstractProtocol::s_ptr msg, std::function<void(AbstractProtocol::s_ptr)> call_back);

    void pushReadMsg(const std::string& req_id, std::function<void(AbstractProtocol::s_ptr)> call_back);

    NetAddr::s_ptr getLocalAddr();

    NetAddr::s_ptr getPeerAddr();
private:
    NetAddr::s_ptr m_local_addr;
    NetAddr::s_ptr m_peer_addr;

    TcpBuffer::s_ptr m_in_buffer;   // 接受缓冲区
    TcpBuffer::s_ptr m_out_buffer;  // 发送缓冲区

    int m_fd;

    EventLoop* m_event_loop { nullptr };  // 持有该连接的io线程
    
    FdEvent* m_fd_event { nullptr };    // 

    TcpState m_state;

    AbstractCoder::s_ptr m_coder;

    TcpConnectionType m_connection_type {TcpConnectionByServer};

    // std::pair<AbstractProtocol::s_ptr, std::function<void(AbstractProtocol::s_ptr)> >
    std::vector<std::pair<AbstractProtocol::s_ptr, std::function<void(AbstractProtocol::s_ptr)>>> m_write_callbacks;

    // key : req_id
    std::map<std::string, std::function<void(AbstractProtocol::s_ptr)>> m_read_callbacks;

};


}


#endif