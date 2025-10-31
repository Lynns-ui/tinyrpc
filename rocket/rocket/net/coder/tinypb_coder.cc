#include <string.h>
#include "tinypb_coder.h"
#include "tinypb_protocol.h"
#include "../../common/util.h"
#include "../../common/log.h"

namespace rocket {

void TinyPBCoder::encode(std::vector<AbstractProtocol::s_ptr>& messages, TcpBuffer::s_ptr out_buffer) {
    for (int i = 0; i < messages.size(); i++) {
        auto message = std::dynamic_pointer_cast<TinyPBProtocol>(messages[i]);
        int len = 0;
        const char* buf = encodeToTinyPB(message, len);
        if (buf != NULL && len != 0) {
            out_buffer->writeToBuffer(buf, len);
        }

        if (buf) {
            free((void*)buf);
            buf = NULL;
        }
    }
}

// 将buffer中的字节流转化为message对象
void TinyPBCoder::decode(std::vector<AbstractProtocol::s_ptr>& messages, TcpBuffer::s_ptr buffer) {
    while (true) {
        // 遍历buffer，一直找到 PB_START PB_END 找到之后解析整包的长度
        // 根据开始符位置和整包长度，找结束符位置，校验结束符是否0x03
        // 如果是，说明是完成的包，然后解析protobuf序列化的对象
        std::vector<char> tmp = buffer->getBuffer();

        int start_index = buffer->readIndex();
        int end_index = -1;
        int pk_len = 0;
        bool prase_success = false;
        int i = 0;
        for (int i = start_index; i < buffer->writeIndex(); i++) {
            if (tmp[i] == TinyPBProtocol::PB_START) {
                // 从下取四个字节。由于是网络字节序，需要转化为主机字节序
                if (i + 1 < buffer->writeIndex()) {
                    // pk_len = 往后四个字节转化为，主机字节序
                    pk_len = getInt32FromNetByte(&tmp[i + 1]);
                    DEBUGLOG("get pk_len = %d", pk_len);
                    
                    // 结束符的索引
                    int j = i + pk_len - 1;
                    if (j >= buffer->writeIndex()) {
                        continue;
                    } 
                    if (tmp[j] == TinyPBProtocol::PB_END) {
                        start_index = i;
                        end_index = j;
                        prase_success = true;
                        break;
                    }
                }
            }
        }
        if (i >= buffer->writeIndex()) {
            DEBUGLOG("decode end, read all buffer data");
            return;
        }
        if (prase_success) {
            buffer->moveReadIndex(end_index - start_index + 1);
            // buffer->moveReadIndex(pk_len);

            std::shared_ptr<TinyPBProtocol> message = std::make_shared<TinyPBProtocol>();
            message->m_pk_len = pk_len;
            
            // req_id的长度
            int req_id_len_index = start_index + sizeof(char) + sizeof(message->m_pk_len);
            if (req_id_len_index >= end_index) {
                message->parse_success = false;
                ERRORLOG("prase pk error, req_id_len_index[%d] > end_index[%d]", req_id_len_index, end_index);
            }
            // 都是int32_t字节
            message->m_req_id_len = getInt32FromNetByte(&tmp[req_id_len_index]);
            DEBUGLOG("parse req_id_len=[%d]", message->m_req_id_len);

            // req_id
            int req_id_index = req_id_len_index + sizeof(message->m_req_id_len);
            char req_id[100] = {0};
            memcpy(&req_id[0], &tmp[req_id_index], message->m_req_id_len);
            message->m_req_id = std::string(req_id);
            DEBUGLOG("parse req_id=[%s]", message->m_req_id.c_str());

            // method长度
            int method_len_index = req_id_index + message->m_req_id_len;
            if (method_len_index >= end_index) {
                message->parse_success = false;
                ERRORLOG("prase pk error, method_len_index[%d] > end_index[%d]", method_len_index, end_index);
            }
            message->m_method_name_len = getInt32FromNetByte(&tmp[method_len_index]);
            // DEBUGLOG("parse method_len=[%d]", message->m_method_name_len);

            // 方法名
            int method_name_index = method_len_index + sizeof(message->m_method_name_len);
            char method_name[512] = {0};
            memcpy(&method_name[0], &tmp[method_name_index], message->m_method_name_len);
            message->m_method_name = std::string(method_name);
            DEBUGLOG("parse method_name=[%s]", message->m_method_name.c_str());

            // 错误码 ..
            int err_code_index = method_name_index + message->m_method_name_len;
            if (err_code_index >= end_index) {
                message->parse_success = false;
                ERRORLOG("prase pk error, err_code_index[%d] > end_index[%d]", err_code_index, end_index);
            }
            message->m_err_code = getInt32FromNetByte(&tmp[err_code_index]);
            DEBUGLOG("parse error_code=[%d]", message->m_err_code);

            // error_info 长度信息
            int err_info_len_index = err_code_index + sizeof(message->m_err_code);
            if (err_info_len_index >= end_index) {
                message->parse_success = false;
                ERRORLOG("prase pk error, err_info_len_index[%d] > end_index[%d]", err_info_len_index, end_index);
            }
            message->m_err_info_len = getInt32FromNetByte(&tmp[err_info_len_index]);
            // DEBUGLOG("parse err_info_len=[%d]", message->m_err_info_len);

            // 错误信息
            int err_info_index = err_info_len_index + sizeof(message->m_err_info_len);
            char err_info[512] = {0};
            memcpy(&err_info[0], &tmp[err_info_index], message->m_err_info_len);
            message->m_err_info = std::string(err_info);
            DEBUGLOG("parse error_info=[%s]", message->m_err_info.c_str());

            // 主要的protobuf序列化数据
            int pb_data_len = message->m_pk_len - message->m_req_id_len - message->m_method_name_len - message->m_err_info_len - 26;
            int pb_data_index = err_info_index + message->m_err_info_len;
            message->m_pb_data = std::string(&tmp[pb_data_index], pb_data_len);

            // 校验和去解析

            message->parse_success = true;

            messages.push_back(message);
        }
    }
}

const char* TinyPBCoder::encodeToTinyPB(std::shared_ptr<TinyPBProtocol> message, int& len) {
    if (message->m_req_id.empty()) {
        message->setReqId("123456");
    }

    DEBUGLOG("req_id = %s", message->m_req_id.c_str());
    // 开始字段 + 结束字段 + 6 * 4字节
    int pk_len = 2 + 24 + message->m_req_id.size() + message->m_method_name.size() + message->m_err_info.size() + message->m_pb_data.size();
    DEBUGLOG("pk_data_len = [%d]", pk_len);

    char* buf = reinterpret_cast<char*>(malloc(pk_len));
    char* tmp = buf;

    *tmp = TinyPBProtocol::PB_START;
    tmp++;

    int32_t pk_len_net = htonl(pk_len);
    memcpy(tmp, &pk_len_net, sizeof(pk_len_net));
    tmp += sizeof(pk_len_net);

    int req_id_len = message->m_req_id.size();
    int32_t req_id_len_net = htonl(req_id_len);
    memcpy(tmp, &req_id_len_net, sizeof(req_id_len_net));
    tmp += sizeof(req_id_len_net);

    if (!message->m_req_id.empty()) {
        memcpy(tmp, message->m_req_id.c_str(), req_id_len);
        tmp += req_id_len;
    }

    int method_name_len = message->m_method_name.size();
    int32_t method_name_len_net = htonl(method_name_len);
    memcpy(tmp, &method_name_len_net, sizeof(method_name_len_net));
    tmp += sizeof(method_name_len_net);

    if (!message->m_method_name.empty()) {
        memcpy(tmp, message->m_method_name.c_str(), method_name_len);
        tmp += method_name_len;
    }

    int32_t error_code_net = htonl(message->m_err_code);
    memcpy(tmp, &error_code_net, sizeof(error_code_net));
    tmp += sizeof(error_code_net);

    int error_info_len = message->m_err_info.size();
    int32_t error_info_len_net = htonl(error_info_len);
    memcpy(tmp, &error_info_len_net, sizeof(error_info_len_net));
    tmp += sizeof(error_info_len_net);

    if (!message->m_err_info.empty()) {
        memcpy(tmp, message->m_err_info.c_str(), error_info_len);
        tmp += error_info_len;
    }

    if (!message->m_pb_data.empty()) {
        memcpy(tmp, message->m_pb_data.c_str(), message->m_pb_data.size());
        tmp += message->m_pb_data.size();
    }

    int32_t check_sum_net = htonl(1);
    memcpy(tmp, &check_sum_net, sizeof(check_sum_net));
    tmp += sizeof(check_sum_net);

    *tmp = TinyPBProtocol::PB_END;
    
    message->m_pk_len = pk_len;
    message->m_req_id_len = req_id_len;
    message->m_method_name_len = method_name_len;
    message->m_err_info_len = error_info_len;
    message->parse_success = true;

    len = pk_len;

    DEBUGLOG("encode messagge[%s] success", message->m_req_id.c_str());
}

}