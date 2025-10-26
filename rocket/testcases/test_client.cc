#include "../rocket/common/log.h"
#include "../rocket/net/tcp/net_addr.h"
#include "../rocket/common/config.h"
#include "../rocket/net/tcp/tcp_server.h"

void test_tcp_client() {
    
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

    char buff[100];
    rt = read(fd, buff, 100);
    std::string rsp(buff);
    INFOLOG("success read %d bytes, [%s]", rt, rsp.c_str());
}

int main() {

  rocket::Config::SetGlobalConfiger("../config/rocket.xml");
  rocket::Logger::InitLogger();

  test_tcp_client();

    return 0;
}