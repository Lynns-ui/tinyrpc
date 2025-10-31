#ifndef ROCKET_NET_STRING_H
#define ROCKET_NET_STRING_H

#include "abstract_coder.h"


namespace rocket {

class StringProtocol : public AbstractProtocol {
public:
    StringProtocol(std::string info) : m_info (info) {

    }

    std::string getInfo() {
        return m_info;
    }

    std::string m_info;
};

class StringCoder : public AbstractCoder {
public:

    // 将message对象转化为字节流，写入到buffer中
    void encode(std::vector<AbstractProtocol::s_ptr>& messages, TcpBuffer::s_ptr out_buffer) {
        for (int i = 0; i < messages.size(); i++) {
            auto msg = std::dynamic_pointer_cast<StringProtocol>(messages[i]);
            out_buffer->writeToBuffer(msg->getInfo().c_str(), msg->getInfo().size());
            // out_buffer->moveWriteIndex(msg->getInfo().size());
        }
    }

    // 将buffer中的字节流转化为message对象
    // 将buffer中的字节组成字符串构造Msg对象，然后放进数组中
    void decode(std::vector<AbstractProtocol::s_ptr>& out_messages, TcpBuffer::s_ptr buffer) {
        
        std::vector<char> re;
        buffer->readFromBuffer(re, buffer->readBytes());
        // buffer->moveReadIndex(buffer->readBytes());

        std::string info;
        for (int i = 0; i < re.size(); i++) {
            info += re[i];
        }

        auto msg = std::make_shared<StringProtocol>(info);
        msg->setReqId("123456");
        out_messages.push_back(msg);
    }

private:

};



}


#endif