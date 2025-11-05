#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include "rpc_channel.h"
#include "rpc_controller.h"
#include "../coder/tinypb_protocol.h"
#include "../../common/msg_id_util.h"
#include "../../common/log.h"
#include "../tcp/tcp_connection.h"
#include "../../common/error_code.h"

namespace rocket {


RpcChannel::RpcChannel(NetAddr::s_ptr peer_addr) : m_peer_addr(peer_addr) {
    m_client = std::make_shared<TcpClient>(m_peer_addr);
}

RpcChannel::~RpcChannel() {
    printf("RpcChannel destruct\n");
}

void RpcChannel::Init(controller_s_ptr controller, message_s_ptr request, message_s_ptr response, closure_s_ptr closure) {
    if (m_is_init) {
        return;
    }
    m_controller = controller;
    m_request = request;
    m_reponse = response;
    m_closure = closure;
    m_is_init = true;
}

void RpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                           google::protobuf::RpcController* controller, const  google::protobuf::Message* request,
                           google::protobuf::Message* response,  google::protobuf::Closure* done) {

    std::shared_ptr<TinyPBProtocol> req_protocol = std::make_shared<TinyPBProtocol>();
    auto m_rpcController = std::dynamic_pointer_cast<RpcController>(m_controller);
    if (m_rpcController == nullptr) {
        ERRORLOG("failed callmethod, RpcController convert error");
        return;
    }

    if (!m_is_init) {
        ERRORLOG("RpcChannel not init");
        // Init(std::make_shared<google::protobuf::RpcController>(controller), std::make_shared<google::protobuf::Message>(request),
        //     std::make_shared<google::protobuf::Message>(response), std::make_shared<google::protobuf::Closure>(done));
        std::string err_info = "RpcChannel not call init";
        m_rpcController->SetError(ERROR_RPC_CHANNEL_INIT, err_info);
        ERRORLOG("[%s] | [%s], RpcChannel not init", req_protocol->m_msg_id.c_str(), err_info.c_str());
        return;
    }

    if (m_rpcController->GetMsgId().empty()) {
        req_protocol->m_msg_id = MsgIDUtil::GenMsgID();
        m_rpcController->SetMsgId(req_protocol->m_msg_id);
    } else {
        req_protocol->m_msg_id = m_rpcController->GetMsgId();
    }

    req_protocol->m_method_name = method->full_name();
    INFOLOG("[%s] | callmethod name [%s]", req_protocol->m_msg_id.c_str(), req_protocol->m_method_name.c_str());

    // request 序列化
    if (!m_request->SerializeToString(&(req_protocol->m_pb_data))) {
        std::string err_info = "serializeToString error";
        m_rpcController->SetError(ERROR_FAILED_SERIALIZE, err_info);
        ERRORLOG("[%s] | [%s] , origin request [%s]", req_protocol->m_msg_id.c_str(), err_info.c_str(), m_request->ShortDebugString().c_str());
        return;
    }

    // 必须用智能指针去构造
    // 连接
    // std::shared_ptr<TcpClient> client = std::make_shared<TcpClient>(m_peer_addr);
    // auto self = shared_from_this();
    m_client->connect([this, req_protocol](){
        RpcController* my_controller = dynamic_cast<RpcController*>(m_controller.get());
        if (m_client->getConnectErrorCode() != 0) {
            my_controller->SetError(m_client->getConnectErrorCode(), m_client->getConnectErrorInfo());
            ERRORLOG("%s | connect to [%s] error, error code[%d], error info[%s]", req_protocol->getMsgId().c_str(), m_peer_addr->toString().c_str(), 
                my_controller->GetErrorCode(), my_controller->GetErrorInfo().c_str());
            return;
        }

        DEBUGLOG("connect to [%s] success", m_peer_addr->toString().c_str());

        m_client->writeMsg(req_protocol, [this, req_protocol, my_controller](rocket::AbstractProtocol::s_ptr){
            INFOLOG("[%s] | send message success, call method name[%s], request[%s]", req_protocol->m_msg_id.c_str(), 
                req_protocol->m_method_name.c_str(), m_request->ShortDebugString().c_str());

            m_client->readMsg(req_protocol->m_msg_id, [this, my_controller](rocket::AbstractProtocol::s_ptr rsp){
                std::shared_ptr<TinyPBProtocol> rsp_protocol = std::dynamic_pointer_cast<TinyPBProtocol>(rsp);
                INFOLOG("[%s] | success get rpc response, call method name[%s]", rsp_protocol->m_msg_id.c_str(), rsp_protocol->m_method_name.c_str());

                if (!m_reponse->ParseFromString(rsp_protocol->m_pb_data)) {
                    ERRORLOG("desrialize falied");
                    my_controller->SetError(ERROR_FAILED_DESERIALIZE, "desrialize falied");
                    return;
                }
                INFOLOG("send message success, response[%s]", m_reponse->ShortDebugString().c_str());
                if (rsp_protocol->m_err_code != 0) {
                    ERRORLOG("[%d] | call method[%s] failed, error code[%d], error info[%s]", rsp_protocol->m_msg_id.c_str(), 
                        rsp_protocol->m_method_name, rsp_protocol->m_err_code, rsp_protocol->m_err_info.c_str());
                    my_controller->SetError(rsp_protocol->m_err_code, rsp_protocol->m_err_info);
                    return;
                }

                if (m_closure) {
                    m_closure->Run();
                }

            });
        });
    }); 
}

TcpClient* RpcChannel::getTcpClient() {
    return m_client.get();
}

}