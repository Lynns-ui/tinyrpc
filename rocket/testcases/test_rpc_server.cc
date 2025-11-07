#include "../rocket/common/log.h"
#include "../rocket/net/tcp/net_addr.h"
#include "../rocket/common/config.h"
#include "../rocket/net/tcp/tcp_server.h"
#include "../rocket/net/tcp/tcp_client.h"
#include "../rocket/net/coder/tinypb_coder.h" 
#include "../rocket/net/rpc/rpc_dispatcher.h"
#include "../rocket/net/rpc/rpc_controller.h"
#include "../rocket/net/rpc/rpc_closure.h"
#include "order.pb.h"

class OrderImpl : public Order {
public:
    void makeOrder(google::protobuf::RpcController* controller,
                       const ::makeOrderRequest* request,
                       ::makeOrderReponse* response,
                       ::google::protobuf::Closure* done) {
        DEBUGLOG("start sleep 5s");
        sleep(5);
        DEBUGLOG("end sleep 5s");
        if (request->price() < 10) {
            response->set_res_code(-1);
            response->set_res_info("short balance");
            return;
        }
        response->set_order_id("20251103");
    }
};

void test_tcp_server(const std::string& ip_port) {
  rocket::IPNetAddr::s_ptr addr = std::make_shared<rocket::IPNetAddr>(ip_port);
  INFOLOG("create addr %s", addr->toString().c_str());

  rocket::TcpServer tcp_server(addr);

  tcp_server.start();
}

int main() {

    rocket::Config::SetGlobalConfiger("../config/rocket.xml");
    rocket::Logger::InitLogger(rocket::Config::GetGlobalCongfiger()->m_log_path, rocket::Config::GetGlobalCongfiger()->m_log_name, 
        rocket::Config::GetGlobalCongfiger()->m_file_size, rocket::Config::GetGlobalCongfiger()->m_async_log);

    rocket::RpcDispatcher::GetRpcDispatcher().registerService(std::make_shared<OrderImpl>());

    std::string ip_port = rocket::Config::GetGlobalCongfiger()->m_ip + ":" + rocket::Config::GetGlobalCongfiger()->m_port;

    test_tcp_server(ip_port);

    return 0;
}