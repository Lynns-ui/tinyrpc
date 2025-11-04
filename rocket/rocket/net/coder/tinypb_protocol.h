#ifndef ROCKET_TINYPB_PROTOCOL_H
#define ROCKET_TINYPB_PROTOCOL_H

#include "abstract_protocol.h"

namespace rocket {

class TinyPBProtocol : public AbstractProtocol {
public:
    TinyPBProtocol() = default;

    static const char PB_START;
    static const char PB_END;

    int32_t m_pk_len {0};   // 整包长度
    
    int32_t m_msg_id_len {0};
    // msg_id在父类中

    int32_t m_method_name_len {0};   // 方法名长度
    std::string m_method_name;  // 方法名

    // 错误码、错误信息长度、错误信息
    int32_t m_err_code {0};
    int32_t m_err_info_len {0};
    std::string m_err_info;

    std::string m_pb_data;      // Protobuf序列化数据
    int32_t m_check_sum {0};    // 校验

    bool parse_success {false};
};

}



#endif