#ifndef ROCKET_NET_ABSTACT_PROTOCOL_H
#define ROCKET_NET_ABSTACT_PROTOCOL_H

#include <memory>
#include <string>
#include "../tcp/tcp_buffer.h"

namespace rocket {

class AbstractProtocol {
public:
    virtual ~AbstractProtocol() = 0;

    typedef std::shared_ptr<AbstractProtocol> s_ptr;

    std::string getReqId() {
        return m_req_id;
    }

    void setReqId(const std::string& req_id) {
        m_req_id = req_id;
    }

    std::string m_req_id;   // 请求id 或者响应

};

}


#endif