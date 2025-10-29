#include "../rocket/common/log.h"
#include "../rocket/net/tcp/net_addr.h"
#include "../rocket/common/config.h"
#include "../rocket/net/tcp/tcp_server.h"
#include "../rocket/net/tcp/tcp_client.h"
#include "../rocket/net/string_coder.h"

void test_connect() {
    
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0) {
        ERRORLOG("invalid fd [%d]", fd);
        exit(0);
    }
    // 调用connect连接server
    sockaddr_in server_adr;
    memset(&server_adr, 0, sizeof(server_adr));
    server_adr.sin_family = AF_INET;
    server_adr.sin_port = htons(1234);
    inet_aton("127.0.0.1", &server_adr.sin_addr);

    int rt = connect(fd, reinterpret_cast<sockaddr*>(&server_adr), sizeof(server_adr));
    // write 一个字符串
    std::string msg = "hello rocket!";

    rt = write(fd, msg.c_str(), msg.length());

    // 等待 read 返回结果
    INFOLOG("success write %d bytes, [%s]", msg.length(), msg.c_str());

    char buff[100] = {0};
    rt = read(fd, buff, 100);
    std::string rsp(buff);
    INFOLOG("success read %d bytes, [%s]", rt, rsp.c_str());
}

void test_tcp_client() {
    rocket::IPNetAddr::s_ptr peer_addr = std::make_shared<rocket::IPNetAddr>("127.0.0.1", 1234);
    rocket::TcpClient client(peer_addr);

    client.connect([peer_addr](){
        DEBUGLOG("connect to [%s] success", peer_addr->toString().c_str());
    });
}   

void test_client_encode() {
    rocket::IPNetAddr::s_ptr peer_addr = std::make_shared<rocket::IPNetAddr>("127.0.0.1", 1234);
    rocket::TcpClient client(peer_addr);

    client.connect([peer_addr, &client](){
        DEBUGLOG("connect to [%s] success", peer_addr->toString().c_str());
        auto msg = std::make_shared<rocket::StringProtocol>("this is encode test!");
        msg->setReqId("123456");
        client.writeMsg(msg, [msg](rocket::AbstractProtocol::s_ptr){
            INFOLOG("msg info is %s", msg->getInfo().c_str());
        });

        client.readMsg("123456", [msg](rocket::AbstractProtocol::s_ptr){
            INFOLOG("get req_id:[%s], get response [%s]", msg->getReqId().c_str(), msg->getInfo().c_str());
        });

        client.writeMsg(msg, [msg](rocket::AbstractProtocol::s_ptr){
            INFOLOG("msg info is %s", msg->getInfo().c_str());
        });
        client.writeMsg(msg, [msg](rocket::AbstractProtocol::s_ptr){
            INFOLOG("msg info is %s", msg->getInfo().c_str());
        });
        client.writeMsg(msg, [msg](rocket::AbstractProtocol::s_ptr){
            INFOLOG("msg info is %s", msg->getInfo().c_str());
        });
        client.writeMsg(msg, [msg](rocket::AbstractProtocol::s_ptr){
            INFOLOG("msg info is %s", msg->getInfo().c_str());
        });
        client.writeMsg(msg, [msg](rocket::AbstractProtocol::s_ptr){
            INFOLOG("msg info is %s", msg->getInfo().c_str());
        });
        client.writeMsg(msg, [msg](rocket::AbstractProtocol::s_ptr){
            INFOLOG("msg info is %s", msg->getInfo().c_str());
        });
    });

}

int main() {

    rocket::Config::SetGlobalConfiger("../config/rocket.xml");
    rocket::Logger::InitLogger();

    test_client_encode();

    return 0;
}