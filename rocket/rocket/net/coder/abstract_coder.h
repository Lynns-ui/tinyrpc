#ifndef ROCKET_ABSTRACT_CODER_H
#define ROCKET_ABSTRACT_CODER_H

#include <string>
#include <vector>
#include <memory>
#include "../tcp/tcp_buffer.h"
#include "abstract_protocol.h"

namespace rocket {

class AbstractCoder {
public:
    typedef std::shared_ptr<AbstractCoder> s_ptr;

    virtual ~AbstractCoder() {}

    // 将message对象转化为字节流，写入到buffer中
    virtual void encode(std::vector<AbstractProtocol::s_ptr>& messages, TcpBuffer::s_ptr out_buffer) = 0;

    // 将buffer中的字节流转化为message对象
    virtual void decode(std::vector<AbstractProtocol::s_ptr>& messages, TcpBuffer::s_ptr buffer) = 0;


};



}


#endif