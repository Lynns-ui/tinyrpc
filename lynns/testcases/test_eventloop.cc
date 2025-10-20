#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include "../lynns/common/config.h"
#include "../lynns/common/log.h"
#include "../lynns/net/eventloop.h"


int main() {
    lynns::Configer::setGlobalConfiger("../config/lynns.xml");
    lynns::Logger::initGlobalLogger();

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        ERRORLOG("create socket fd error!");
        return 0;
    }

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_port = htons(1316);
    addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &addr.sin_addr);

    int rt = bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    if (rt < 0) {
        ERRORLOG("bind error");
        return 0;
    }

    rt = listen(fd, 100);
    if (rt < 0) {
        ERRORLOG("listen error");
        return 0;
    }

    lynns::EventLoop* eventloop = new lynns::EventLoop();
    lynns::FdEvent event(fd);
    event.listen(lynns::FdEvent::IN_EVENT, [&fd](){
        sockaddr_in peer_addr;
        socklen_t addr_len = 0;
        memset(&peer_addr, 0, sizeof(peer_addr));
        int clientfd = accept(fd, reinterpret_cast<sockaddr*>(&peer_addr), &addr_len);
        DEBUGLOG("sucess get client fd:[%d], peer addr: [%s:%d]", clientfd, inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port));
    });

    eventloop->addEpollEvent(&event);
    eventloop->loop();
}