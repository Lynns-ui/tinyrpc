#include <unistd.h>
#include "../fd_event_pool.h"
#include "tcp_connection.h"
#include "../../common/log.h"

namespace rocket {

TcpConnection::TcpConnection(IOThread* io_thread, int client_fd, int buffer_size, NetAddr::s_ptr peer_addr) : 
    m_peer_addr(peer_addr), m_fd(client_fd), m_io_thread(io_thread), m_state(NotConnected){
    
    m_in_buffer = std::make_shared<TcpBuffer>(buffer_size);
    m_out_buffer = std::make_shared<TcpBuffer>(buffer_size);

    m_fd_event = FdEventPool::GetFdEventPool()->getFdEvent(client_fd);
    m_fd_event->setNonBlock();
    m_fd_event->listen(FdEvent::IN_EVENT, std::bind(&TcpConnection::onRead, this));

    m_io_thread->getEventloop()->addEpollEvent(m_fd_event);
}

TcpConnection::~TcpConnection() {
    DEBUGLOG("destruct");
}

void TcpConnection::onRead() {
    // 1. 从socket缓冲区，调用系统调用read 读取字节到inbuffer中
    if (m_state != Connected) {
        ERRORLOG("onRead error, client has already disconneted, addr[%s], client_fd[%d]", m_peer_addr->toString().c_str(), m_fd);
        return;
    }
    
    bool is_read_all = false;
    bool is_closed = false;
    while (!is_read_all) {
        if (m_in_buffer->writeBytes() == 0) {
            m_in_buffer->resizeBuffer(2 * m_in_buffer->buffSize());
        }
        int read_count = m_in_buffer->writeBytes();
        // int write_index = m_in_buffer->writeIndex();

        int rt = read(m_fd, m_in_buffer->writePtr(), read_count);
        DEBUGLOG("success read %d bytes from addr[%s], client_fd[%d]", rt, m_peer_addr->toString().c_str(), m_fd);
        if (rt > 0) {
            m_in_buffer->moveWriteIndex(rt);
            if (rt == read_count) {
                continue;
            } else if (rt < read_count) {
                // 说明缓冲区的数据已经读完
                is_read_all = true;
                break;
            } 
        } else if (rt == 0) {
            is_closed = true;
            break;
        } else if (rt == -1 && errno == EAGAIN) {
            // 没有数据读
            is_read_all = true;
            break;
        }
    }

    if (is_closed) {
        // 处理关闭连接 todo...
        INFOLOG("peer close, peer addr[%s], client_fd[%d]", m_peer_addr->toString().c_str(), m_fd);
        clear();
    }

    if (!is_read_all) {
        ERRORLOG("not read all data");
    }

    // 执行
    // 需要RPC协议的解析
    excute();
}

void TcpConnection::excute() {
    // 将RPC请求执行业务逻辑，获取RPC响应，再把 RPC 响应发送回去
    std::vector<char> tmp;
    int size = m_in_buffer->readBytes();
    tmp.resize(size);
    m_in_buffer->readFromBuffer(tmp, size);

    std::string msg;
    for (int i = 0; i < tmp.size(); i++) {
        msg += tmp[i];
    }
    m_out_buffer->writeToBuffer(msg.c_str(), msg.length());

    INFOLOG("success get reauest[%s] from client[%s]", msg.c_str(), m_peer_addr->toString().c_str());

    m_fd_event->listen(FdEvent::OUT_EVENT, std::bind(&TcpConnection::onWrite, this));
    m_io_thread->getEventloop()->addEpollEvent(m_fd_event);
}

void TcpConnection::onWrite() {
    // 将当前outbuffer里面的数据全部给client
    if (m_state != Connected) {
        ERRORLOG("onWrite error, client has already disconneted, addr[%s], client_fd[%d]", m_peer_addr->toString().c_str(), m_fd);
        return;
    }

    bool is_write_all = false;
    while (true) {
        if (m_out_buffer->readBytes() == 0) {\
            is_write_all = true;
            DEBUGLOG("no data need to send to client[%s]", m_peer_addr->toString().c_str());
            break;
        }
        int write_size = m_out_buffer->readBytes();
        int rt = write(m_fd, m_out_buffer->readPtr(), write_size);
        
        if (rt >= write_size) {
            printf("rt = %d\n", rt);
            is_write_all = true;
            m_out_buffer->moveReadIndex(write_size);
            INFOLOG("all data has write to client [%s]",  m_peer_addr->toString().c_str());
            break;
        } else if (rt == -1 && errno == EAGAIN) {
            // 发送缓冲区已满，不能再次发送
            // 直接等下次fd可写时，再次发送数据即可
            DEBUGLOG("write data error, errno == EAGAIN and rt == -1");
            break;
        }
    }

    if (is_write_all) {
        m_fd_event->cancel(FdEvent::OUT_EVENT);
        m_io_thread->getEventloop()->addEpollEvent(m_fd_event);
    }
}

void TcpConnection::setState(const TcpConnection::TcpState state) {
    m_state = state;
}

TcpConnection::TcpState TcpConnection::getState() {
    return m_state;
}

void TcpConnection::clear() {
    // 服务器处理关闭连接后的清理动作
    if (m_state == Closed) {
        return;
    }
    m_io_thread->getEventloop()->delEpollEvent(m_fd_event);
    m_state = Closed;
}

void TcpConnection::shutdown() {
    // 当fd长时间不做事情
    if (m_state == Closed || m_state == NotConnected) {
        return;
    }

    // 处于半关闭，四次挥手
    m_state = HalfClosing;
    // 调用shutdown 关闭读写，意味着服务器不会再对这个fd进行读写操作
    // 向客户端发送了 FIN 报文，触发了四次挥手的第一个阶段
    // 当 fd 发生可读事件，但是可读的数据为0时，表示对端没有发送数据，对端发送了FIN报文
    ::shutdown(m_fd, SHUT_RDWR);
}


}