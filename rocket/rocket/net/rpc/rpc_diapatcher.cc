#include "rpc_dispatcher.h"
#include "rpc_closure.h"
#include "../../common/log.h"
#include "../../common/error_code.h"

namespace rocket {

RpcDispatcher& RpcDispatcher::GetRpcDispatcher() {
    static RpcDispatcher instance;
    return instance;
}

void RpcDispatcher::dispatch(AbstractProtocol::s_ptr request, AbstractProtocol::s_ptr response, TcpConnection* connection) {
    auto req = std::dynamic_pointer_cast<TinyPBProtocol>(request);
    auto rsp = std::dynamic_pointer_cast<TinyPBProtocol>(response);

    rsp->m_msg_id = req->m_msg_id;
    rsp->m_method_name = req->m_method_name;

    // service.method_name
    std::string method_full_name = req->m_method_name;
    std::string service_name;
    std::string method_name;
    if (!parseServiceFullName(method_full_name, service_name, method_name)) {
        // 没有解析成功...
        setTinyPBError(rsp, ERROR_PARSE_SERVICE_NAME, "parse service name error");
        return;
    }
    auto it = m_services_map.find(service_name);
    if (it == m_services_map.end()) {
        // 没找到服务
        ERRORLOG("%s | service name[%s] not found", req->m_msg_id.c_str(), service_name.c_str());
        setTinyPBError(rsp, ERROR_SERVICE_NOT_FOUND, "service not found");
        return;
    }
    service_s_ptr service = it->second;

    // 从service找方法
    // 根据方法名找到对应的方法文件描述符
    const google::protobuf::MethodDescriptor* method = service->GetDescriptor()->FindMethodByName(method_name);
    if (method == nullptr) {
        // 没有找到方法，从protobuf
        ERRORLOG("%s | method [%s] not found", req->m_msg_id.c_str(), method_name.c_str());
        setTinyPBError(rsp, ERROR_METHOD_NOT_FOUND, "method not found");
        return;
    }

    // 用于获取该方法对应的请求消息原型，并创建一个新的、同类型的消息实例
    std::unique_ptr<google::protobuf::Message> req_msg(service->GetRequestPrototype(method).New());

    // 反序列化，将pbdata反序列化为 req_msg
    // 还原成原型，操控其中的字段
    if (!req_msg->ParseFromString(req->m_pb_data)) {
        ERRORLOG("%s | deserialize failed", req->m_msg_id.c_str());
        setTinyPBError(rsp, ERROR_FAILED_DESERIALIZE, "deserialize failed");
        return;
    }
    INFOLOG("%s | get rpc request[%s]", req->m_msg_id.c_str(), req_msg->ShortDebugString().c_str());

    std::unique_ptr<google::protobuf::Message> rsp_msg(service->GetResponsePrototype(method).New());
    
    RpcController controller;
    controller.SetLocalAddr(connection->getLocalAddr());
    controller.SetPeerAddr(connection->getPeerAddr());
    controller.SetMsgId(req->getMsgId());

    service->CallMethod(method, &controller, req_msg.get(), rsp_msg.get(), NULL);  //根据方法描述符，调用服务的对应方法，用请求消息传参，用响应消息接收结果
    
    if (!rsp_msg->SerializeToString(&(rsp->m_pb_data))) {
        // 没有序列化成功
        ERRORLOG("[%s] | serialize failed", req->m_msg_id.c_str());
        setTinyPBError(rsp, ERROR_FAILED_SERIALIZE, "serialize failed");
        return;
    }

    // 在这里设置rsp的数据
    rsp->m_err_code = 0;
    INFOLOG("[%s] | dispatcher success, request[%s], response[%s]", req->m_msg_id.c_str(), req_msg->ShortDebugString().c_str(), 
        rsp_msg->ShortDebugString().c_str());
}

void RpcDispatcher::registerService(service_s_ptr service) {
    std::string service_name = service->GetDescriptor()->full_name();
    m_services_map[service_name] = service;
    INFOLOG("register service[%s] success", service_name.c_str());
}

bool RpcDispatcher::parseServiceFullName(const std::string& full_name, std::string& service_name, std::string& method_name) {
    if (full_name.empty()) {
        ERRORLOG("full name empty");
        return false;
    }
    // 找点号
    size_t i = full_name.find_first_of(".");
    if (i == full_name.npos) {
        ERRORLOG("not found . in full_name[%s]", full_name.c_str());
        return false;
    }
    service_name = full_name.substr(0, i);
    method_name = full_name.substr(i + 1, full_name.size() - i - 1);
    INFOLOG("full_name[%s], service_name[%s], method_name[%s]", full_name.c_str(), service_name.c_str(), method_name.c_str());
    return true;
}

void RpcDispatcher::setTinyPBError(std::shared_ptr<TinyPBProtocol> response, int32_t err_code, const std::string err_info) {
    response->m_err_code = err_code;
    response->m_err_info = err_info;
    response->m_err_info_len = err_info.size();
}

}