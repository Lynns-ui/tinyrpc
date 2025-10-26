#include <assert.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <string.h>
#include "tcp_acceptor.h"
#include "../../common/log.h"

namespace rocket {
TcpAcceptor::TcpAcceptor(NetAddr::s_ptr local_addr) : m_local_addr(local_addr){
    if (!m_local_addr->checkValid()) {
        ERRORLOG("invalid local addr %s", local_addr->toString().c_str());
        exit(0);
    }
    m_family = m_local_addr->getFamily();
    
    m_listenfd = socket(m_family, SOCK_STREAM, 0);
    if(m_listenfd < 0) {
        ERRORLOG("invalid listenfd %d", m_listenfd);
        exit(0);
    }
    // 设置fd为非阻塞，
    // 因为我们的架构整体是Reactor模型，一直在一个循环的，当fd中的数据过大时，往缓冲区中读数据或者写数据的时候
    // 缓冲区的空间不足那么就会阻塞，但我们不希望这种事件发生因此设置fd为非阻塞，让fd异步的去读写
    int val = 1;
    // SO_REUSEADDR 是为了让服务器端口复用，不会当服务器主动关闭时，原来的fd会处于time_wait状态，仍然绑定，出现端口占用的情况
    // bind不了
    if (setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) != 0) {
        ERRORLOG("setsockopt RESUEADDR error, errno=%d, error info=%s", errno, strerror(errno));
    }
    socklen_t len = m_local_addr->getSocklen();
    if (bind(m_listenfd, m_local_addr->getSockAddr(), len) != 0) {
        ERRORLOG("socket bind error, errno=%d, error info=%s", errno, strerror(errno));
        exit(0);
    }

    if (listen(m_listenfd, 1000) != 0) {
        ERRORLOG("socket listen error, errno=%d, error info=%s", errno, strerror(errno));
        exit(0);
    }
}

std::pair<int, NetAddr::s_ptr> TcpAcceptor::accept() {
    if (m_family == AF_INET) {
        sockaddr_in client_addr;
        memset(&client_addr, 0, sizeof(client_addr));
        socklen_t client_addr_len = sizeof(client_addr);

        int client_fd = ::accept(m_listenfd, reinterpret_cast<sockaddr*>(&client_addr), &client_addr_len);
        if (client_fd < 0) {
            ERRORLOG("socket accept error, errno=%d, error info=%s", errno, strerror(errno));
            return {client_fd, nullptr};
        }
        
        IPNetAddr::s_ptr peer_addr = std::make_shared<IPNetAddr>(client_addr);
        INFOLOG("client have accepted sucess, peer addr [%s]", peer_addr->toString().c_str());
        return {client_fd, peer_addr};
    } else {
        /* another protocol */
    }
}

TcpAcceptor::~TcpAcceptor() {

}

int TcpAcceptor::getListenFd() {
    return m_listenfd;
}

}