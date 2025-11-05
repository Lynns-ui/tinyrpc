#include <unistd.h>
#include <string.h>
#include "tcp_client.h"
#include "../../common/log.h"
#include "../../common/error_code.h"
#include "../fd_event_pool.h"

namespace rocket {

TcpClient::TcpClient(NetAddr::s_ptr peer_addr) : m_peer_addr(peer_addr){
    // 这是一个新的eventloop对象，在一个新的进程，还没有开启loop
    m_event_loop = EventLoop::GetCurrentEventloop();

    m_fd = socket(m_peer_addr->getFamily(), SOCK_STREAM, 0);

    if (m_fd < 0) {
        ERRORLOG("TcpClient create fd error, failed to create fd");
        exit(0);
    }

    m_fd_event = FdEventPool::GetFdEventPool()->getFdEvent(m_fd);
    m_fd_event->setNonBlock();

    // IOThread* io_thread, int client_fd, int buffer_size, NetAddr::s_ptr peer_addr
    m_connection = std::make_shared<TcpConnection>(m_event_loop, m_fd, 128, nullptr, m_peer_addr, TcpConnection::TcpConnectionByClient);

}

TcpClient::~TcpClient() {
    if (m_fd > 0) {
        close(m_fd);
    }
}

// 异步进行connect, connect成功，执行回调函数
void TcpClient::connect(std::function<void()> done) {
    int rt = ::connect(m_fd, m_peer_addr->getSockAddr(), m_peer_addr->getSocklen());
    if (rt == 0) {
        DEBUGLOG("connect connect [%s] success", m_peer_addr->toString().c_str());
        m_connection->setState(TcpConnection::Connected);
        if (done) {
            done();
        }
    } else if (rt == -1) {
        if (errno == EINPROGRESS) {
            // 监听epoll监听可写事件，然后判断错误码
            // 当connect成功之后，套接字应该变为“可写状态”
            m_fd_event->listen(FdEvent::OUT_EVENT, 
                [this, done](){
                    int error = 0;
                    socklen_t error_len = sizeof(error);
                    getsockopt(m_fd, SOL_SOCKET, SO_ERROR, &error, &error_len);
                    bool is_connected = false;
                    if (error == 0) {
                        INFOLOG("connect [%s] success", m_peer_addr->toString().c_str());
                        m_connection->setState(TcpConnection::Connected);
                        is_connected = true;
                    } else {
                        m_connect_errorcode = ERROR_FAILED_CONNECT;
                        m_error_info = "connected error, sys error = " + std::string(strerror(errno));
                        ERRORLOG("connect error, errno=%d, error info =%s", errno, strerror(errno));
                    }
                    
                    // 当连接成功，需要把fd事件从eventloop循环中，移除，否则会一直触发可写事件
                    m_event_loop->delEpollEvent(m_fd_event);
                    
                    // 如果监听成功才会执行回调函数
                    // 在回调函数中调用writeMsg，将StringCoder对象转化为字节流，放入out_buffer中
                    // 然后再将out_buffer中的数据写入到fd缓冲区中，让对端可以收到数据
                    if (done) {
                        done();
                    }
                }, 
                [this,done](){
                    if (errno == ECONNREFUSED) {
                        m_connect_errorcode = ERROR_FAILED_CONNECT;
                        m_error_info = "connected refused, sys error = " + std::string(strerror(errno));
                        ERRORLOG("connect refused, errno=%d, error info =%s", errno, strerror(errno));
                    } else {
                        m_connect_errorcode = ERROR_FAILED_CONNECT;
                        m_error_info = "connected unkonw error, sys error = " + std::string(strerror(errno));
                        ERRORLOG("connect unkonw error, errno=%d, error info =%s", errno, strerror(errno));
                    }
                });
            m_event_loop->addEpollEvent(m_fd_event);
        } else {
            m_connect_errorcode = ERROR_FAILED_CONNECT;
            m_error_info = "connected error, sys error = " + std::string(strerror(errno));
            ERRORLOG("connect error, errno=%d, error info =%s", errno, strerror(errno));
            if (done) {
                done();
            }
        }
    }
    
    m_event_loop->loop();
}

// 异步发送msg  
// 如果发送成功，调用done 函数的入参就是message
void TcpClient::writeMsg(AbstractProtocol::s_ptr message, std::function<void(AbstractProtocol::s_ptr)> done) {
    // 1. message对象写入到connection的buffer中，done也写入
    m_connection->pushSendMsg(message, done);
    // 启动connection的可写事件
    m_connection->listenWrite();
}

// 异步读取message 读取回包
void TcpClient::readMsg(const std::string msg_id, std::function<void(AbstractProtocol::s_ptr)> done) {
    // 1. 监听可读事件
    m_connection->pushReadMsg(msg_id, done);
    // 2. 从buffer中decoder出msg对象，判断是否msg_id相等，相等则成功，执行其回调
    m_connection->listenRead();
}

void TcpClient::stop() {
    m_event_loop->stop();
}

int TcpClient::getConnectErrorCode() {
    return m_connect_errorcode;
}
    
std::string TcpClient::getConnectErrorInfo() {
    return m_error_info;
}


}