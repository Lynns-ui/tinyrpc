#ifndef ROCKET_RPC_CHANNEL_H
#define ROCKET_RPC_CHANNEL_H

#include <google/protobuf/service.h>
#include <memory>
#include "../tcp/net_addr.h"
#include "../tcp/tcp_client.h"
#include "../timerevent.h"

namespace rocket {

#define NEWMESSAGE(type, var_name) \
    std::shared_ptr<type> var_name = std::make_shared<type>(); \

#define NEWCONTROLLER(var_name) \
    std::shared_ptr<rocket::RpcController> var_name = std::make_shared<rocket::RpcController>(); \

#define NEWCHANNEL(var_name, addr) \
    std::shared_ptr<rocket::RpcChannel> var_name = std::make_shared<rocket::RpcChannel>(std::make_shared<rocket::IPNetAddr>(addr)); \

#define CALLRPC(var_name, addr, method_name, controller, request, response, closure) \
{\
    NEWCHANNEL(var_name, addr)\
    var_name->Init(controller, request, response, closure);\
    Order_Stub(var_name.get()).method_name(controller.get(), request.get(), response.get(), closure.get());\
}\
    


class RpcChannel : public google::protobuf::RpcChannel , public std::enable_shared_from_this<RpcChannel> {
public:
    typedef std::shared_ptr<RpcChannel> s_ptr;
    typedef std::shared_ptr<google::protobuf::RpcController> controller_s_ptr;
    typedef std::shared_ptr<google::protobuf::Message> message_s_ptr;
    typedef std::shared_ptr<google::protobuf::Closure> closure_s_ptr;

    RpcChannel(NetAddr::s_ptr peer_addr);
    ~RpcChannel();

    void Init(controller_s_ptr controller, message_s_ptr request, message_s_ptr response, closure_s_ptr closure);

    void CallMethod(const google::protobuf::MethodDescriptor* method,
                           google::protobuf::RpcController* controller, const  google::protobuf::Message* request,
                           google::protobuf::Message* response,  google::protobuf::Closure* done);

    TcpClient* getTcpClient();
private:
    NetAddr::s_ptr m_local_addr {nullptr};
    NetAddr::s_ptr m_peer_addr;

    controller_s_ptr m_controller {nullptr};
    message_s_ptr m_request {nullptr};
    message_s_ptr m_reponse {nullptr};
    closure_s_ptr m_closure {nullptr};
    TimerEvent::s_ptr m_timer_event {nullptr};

    bool m_is_init {false};

    std::shared_ptr<TcpClient> m_client {nullptr};
};

}


#endif