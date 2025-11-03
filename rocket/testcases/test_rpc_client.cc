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


void test_client() {
    rocket::IPNetAddr::s_ptr peer_addr = std::make_shared<rocket::IPNetAddr>("127.0.0.1", 1234);
    rocket::TcpClient client(peer_addr);

    client.connect([peer_addr, &client](){
        DEBUGLOG("connect to [%s] success", peer_addr->toString().c_str());
        auto msg = std::make_shared<rocket::TinyPBProtocol>();
        msg->m_req_id = "123456789";

        makeOrderRequest request;
        request.set_price(100);
        request.set_goods("apple");
        if (!request.SerializeToString(&(msg->m_pb_data))) {
            ERRORLOG("SerializeToString falied");
            return;
        }

        msg->m_method_name = "Order.makeOrder";

        client.writeMsg(msg, [request](rocket::AbstractProtocol::s_ptr){
            INFOLOG("send message success, request[%s]", request.ShortDebugString().c_str());
        });

        client.readMsg("123456789", [](rocket::AbstractProtocol::s_ptr reponse){
            std::shared_ptr<rocket::TinyPBProtocol> read_msg = std::dynamic_pointer_cast<rocket::TinyPBProtocol>(reponse);
            INFOLOG("get req_id:[%s], get response[%s]", read_msg->m_req_id.c_str(), read_msg->m_pb_data.c_str());

            makeOrderReponse rpc_rsp;
            if (!rpc_rsp.ParseFromString(read_msg->m_pb_data)) {
                ERRORLOG("desrialize falied");
                return;
            }
            INFOLOG("send message success, response[%s]", rpc_rsp.ShortDebugString().c_str());
        });

    });
}

int main() {

    rocket::Config::SetGlobalConfiger("../config/rocket.xml");
    rocket::Logger::InitLogger();

    test_client();

    return 0;
}