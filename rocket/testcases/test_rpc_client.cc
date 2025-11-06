#include "../rocket/common/log.h"
#include "../rocket/net/tcp/net_addr.h"
#include "../rocket/common/config.h"
#include "../rocket/net/tcp/tcp_server.h"
#include "../rocket/net/tcp/tcp_client.h"
#include "../rocket/net/coder/tinypb_coder.h" 
#include "../rocket/net/rpc/rpc_dispatcher.h"
#include "../rocket/net/rpc/rpc_controller.h"
#include "../rocket/net/rpc/rpc_closure.h"
#include "../rocket/net/rpc/rpc_channel.h"
#include "order.pb.h"

void test_client() {
    rocket::IPNetAddr::s_ptr peer_addr = std::make_shared<rocket::IPNetAddr>("127.0.0.1", 1234);
    rocket::TcpClient client(peer_addr);

    client.connect([peer_addr, &client](){
        DEBUGLOG("connect to [%s] success", peer_addr->toString().c_str());
        auto msg = std::make_shared<rocket::TinyPBProtocol>();
        msg->m_msg_id = "123456789";

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
            INFOLOG("get msg_id:[%s], get response[%s]", read_msg->m_msg_id.c_str(), read_msg->m_pb_data.c_str());

            makeOrderReponse rpc_rsp;
            if (!rpc_rsp.ParseFromString(read_msg->m_pb_data)) {
                ERRORLOG("desrialize falied");
                return;
            }
            INFOLOG("send message success, response[%s]", rpc_rsp.ShortDebugString().c_str());
        });

    });
}

void test_rpc_channel() {
    // rocket::IPNetAddr::s_ptr peer_addr = std::make_shared<rocket::IPNetAddr>("127.0.0.1", 1234);
    // std::shared_ptr<rocket::RpcChannel> channel = std::make_shared<rocket::RpcChannel>(peer_addr);
    NEWCHANNEL(channel, "127.0.0.1:1234");
    
    // std::shared_ptr<makeOrderRequest> request = std::make_shared<makeOrderRequest>();
    NEWMESSAGE(makeOrderRequest, request)
    request->set_price(100);
    request->set_goods("apple");
    // std::shared_ptr<makeOrderReponse> response = std::make_shared<makeOrderReponse>();
    NEWMESSAGE(makeOrderReponse, response)

    // std::shared_ptr<rocket::RpcController> controller = std::make_shared<rocket::RpcController>();
    NEWCONTROLLER(controller);
    controller->SetMsgId("99998888");

    std::shared_ptr<rocket::RpcClosure> closure = std::make_shared<rocket::RpcClosure>([request, response, channel, controller]() mutable {
        if (controller->GetErrorCode() == 0) {
            INFOLOG("call rpc success, request[%s], request[%s]", request->ShortDebugString().c_str(), response->ShortDebugString().c_str());
        } else {
            ERRORLOG("call rpc falied, request[%s], error_info[%s]", request->ShortDebugString().c_str(), controller->GetErrorInfo().c_str());
        }
        channel->getTcpClient()->stop();
        channel.reset();
    });
        
    // channel->Init(controller, request, response, closure);

    // Order_Stub stub(channel.get());
    controller->SetTimeOut(10000);
    // stub.makeOrder(controller.get(), request.get(), response.get(), closure.get());

    CALLRPC(channel, "127.0.0.1:1234" ,makeOrder, controller, request, response, closure)
}

void test_package() {
    NEWCHANNEL(channel, "127.0.0.1:1234")

    NEWMESSAGE(makeOrderRequest, request)
    request->set_price(100);
    request->set_goods("apple");

    NEWMESSAGE(makeOrderReponse, response)

    NEWCONTROLLER(controller);
    controller->SetMsgId("99998888");
    controller->SetTimeOut(10000);

    std::shared_ptr<rocket::RpcClosure> closure = std::make_shared<rocket::RpcClosure>([request, response, channel, controller]() mutable {
        if (controller->GetErrorCode() == 0) {
            INFOLOG("call rpc success, request[%s], request[%s]", request->ShortDebugString().c_str(), response->ShortDebugString().c_str());
        } else {
            ERRORLOG("call rpc falied, request[%s], error_info[%s]", request->ShortDebugString().c_str(), controller->GetErrorInfo().c_str());
        }
        channel->getTcpClient()->stop();
        channel.reset();
    });

    CALLRPC(channel, "127.0.0.1:1234" ,makeOrder, controller, request, response, closure)
}

int main() {

    rocket::Config::SetGlobalConfiger("../config/rocket.xml");
    rocket::Logger::InitLogger();

    test_package();

    return 0;
}