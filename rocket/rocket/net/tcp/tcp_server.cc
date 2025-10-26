#include "tcp_server.h"
#include "../eventloop.h"
#include "../../common/log.h"
#include "../fdevent.h"

namespace rocket {

TcpServer::TcpServer(NetAddr::s_ptr local_addr) : m_local_addr(local_addr) {
    
    init();

    INFOLOG("rocket TcpServer listen success on [%s]", m_local_addr->toString().c_str());
}

TcpServer::~TcpServer() {
    if (m_main_eventloop) {
        delete m_main_eventloop;
        m_main_eventloop = NULL;
    }

    if(m_io_threadpool) {
        delete m_io_threadpool;
        m_io_threadpool = NULL;
    }
}

void TcpServer::init() {
    m_acceptor = std::make_shared<TcpAcceptor>(m_local_addr);

    m_main_eventloop = EventLoop::GetCurrentEventloop();    // 主loop循环是当前线程的， 全局唯一的

    // 后续写到配置文件中
    m_io_threadpool = new IOThreadPool(2);

    m_listen_fdevent = new FdEvent(m_acceptor->getListenFd());

    m_listen_fdevent->listen(FdEvent::IN_EVENT, std::bind(&TcpServer::onAccept, this));
    m_main_eventloop->addEpollEvent(m_listen_fdevent);

}

void TcpServer::start() {
    m_io_threadpool->start();

    m_main_eventloop->loop();
}

void TcpServer::onAccept() {
    // 需要执行的实际函数，把新连接过来的clientfd，放到iothreadpool中
    int client_fd = m_acceptor->accept();
    // FdEvent client_fd_event(client_fd);
    m_client_counts++;

    // todo... : 把clientfd 添加到io线程里面
    // m_io_threadpool->getIOThread()->getEventloop()->addEpollEvent(&client_fd_event);
    INFOLOG("TcpServer success get client, fd=[%d]", client_fd);
}

}