#ifndef ROCKET_TINYPB_CODER_H
#define ROCKET_TINYPB_CODER_H

#include "abstract_coder.h"
#include "tinypb_protocol.h"

namespace rocket {

class TinyPBCoder : public AbstractCoder {

public:
    void encode(std::vector<AbstractProtocol::s_ptr>& messages, TcpBuffer::s_ptr out_buffer);

    // 将buffer中的字节流转化为message对象
    void decode(std::vector<AbstractProtocol::s_ptr>& messages, TcpBuffer::s_ptr buffer);
private:
    // 将单个的包转化为字节流
    const char* encodeToTinyPB(std::shared_ptr<TinyPBProtocol> message, int& len);
};

}


#endif