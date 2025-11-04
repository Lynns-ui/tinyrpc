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

    std::string getMsgId() {
        return m_msg_id;
    }

    void setMsgId(const std::string& msg_id) {
        m_msg_id = msg_id;
    }

    std::string m_msg_id;   // 请求id 或者响应

};

}


#endif