#ifndef RPC_DISPATCHER_H
#define RPC_DISPATCHER_H

#include <map>
#include <memory>
#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include "../coder/abstract_protocol.h"
#include "../coder/tinypb_protocol.h"
#include "rpc_controller.h"
#include "../tcp/tcp_connection.h"

namespace rocket {

typedef std::shared_ptr<google::protobuf::Service> service_s_ptr;
class TcpConnection;

class RpcDispatcher {

public:
    static RpcDispatcher& GetRpcDispatcher();

    void dispatch(AbstractProtocol::s_ptr request, AbstractProtocol::s_ptr response, TcpConnection* connection);

    void registerService(service_s_ptr service);

    void setTinyPBError(std::shared_ptr<TinyPBProtocol> response, int32_t err_code, const std::string err_info);

private:
    bool parseServiceFullName(const std::string& full_name, std::string& service_name, std::string& method_name);

    std::map<std::string, service_s_ptr> m_services_map;

};

}

#endif