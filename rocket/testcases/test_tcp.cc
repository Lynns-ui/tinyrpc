#include "../rocket/common/log.h"
#include "../rocket/net/tcp/net_addr.h"
#include "../rocket/common/config.h"
#include "../rocket/net/tcp/tcp_server.h"

void test_tcp_server() {
  rocket::IPNetAddr::s_ptr addr = std::make_shared<rocket::IPNetAddr>("127.0.0.1", 1234);
  INFOLOG("create addr %s", addr->toString().c_str());

  rocket::TcpServer tcp_server(addr);

  tcp_server.start();
}

int main() {

  rocket::Config::SetGlobalConfiger("../config/rocket.xml");
  rocket::Logger::InitLogger();

  test_tcp_server();

}