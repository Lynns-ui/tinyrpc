#include "config.h"

namespace rocket {

using namespace tinyxml2;

#define READ_XML_NODE(name, parent)                                             \
    XMLElement* name##_node = parent->FirstChildElement(#name);                 \
    if (name##_node == nullptr) {                                               \
        printf("Start rocket server error, failed to file [%s]\n", xmlfile);  \
        exit(0);                                                                \
    }                                                                           \

#define READ_STR_XML_NODE(name, parent) \
    XMLElement* name##_node = parent->FirstChildElement(#name);\
    if (!name##_node || !name##_node->GetText()) {   \
        printf("Start rocket server error, failed to read node [%s]\n", xmlfile); \
        exit(0); \
    } \
    std::string name##_str = std::string(name##_node->GetText());\
    

Config* Config::g_configer  = nullptr;
Config* Config::GetGlobalCongfiger() {
    return g_configer;
}

void Config::SetGlobalConfiger(const char* xmlfile) {
    if (g_configer == nullptr) {
        g_configer = new Config(xmlfile);
    }
}

Config::Config(const char* xmlfile) {
    XMLDocument* xml_document = new XMLDocument();
    // xml_document->FirstChild()
    bool rt = xml_document->LoadFile(xmlfile);
    if (rt) {
        printf("Start rocket server error, failed to read config file %s\n", xmlfile);
        printf("LoadFile failed! TinyXML2 error: %s\n", xml_document->ErrorName());
        exit(0);
    }
    READ_XML_NODE(root, xml_document);

    // 日志配置
    READ_XML_NODE(server_log, root_node);
    READ_STR_XML_NODE(server_log_level, server_log_node);
    READ_STR_XML_NODE(server_async_log, server_log_node);
    READ_STR_XML_NODE(server_log_path, server_log_node);
    READ_STR_XML_NODE(server_log_name, server_log_node);
    READ_STR_XML_NODE(server_file_count, server_log_node);

    m_log_level = server_log_level_str;
    m_async_log = server_async_log_str;
    m_log_path = server_log_path_str;
    m_log_name = server_log_name_str;
    m_file_size = server_file_count_str;

    // ip地址
    READ_XML_NODE(server_IP, root_node);
    READ_STR_XML_NODE(server_ip, server_IP_node);
    m_ip = server_ip_str;

    // 端口号
    READ_XML_NODE(server_Port, root_node);
    READ_STR_XML_NODE(server_port, server_Port_node);
    m_port = server_port_str;

    // 客户端日志配置
    READ_XML_NODE(client_log, root_node);
    READ_STR_XML_NODE(client_async_log, client_log_node);
    READ_STR_XML_NODE(client_log_path, client_log_node);
    READ_STR_XML_NODE(client_log_name, client_log_node);
    READ_STR_XML_NODE(client_file_count, client_log_node);

    m_client_async_log = client_async_log_str;
    m_client_log_path = client_log_path_str;
    m_client_log_name = client_log_name_str;
    m_client_file_size = client_file_count_str;
}


}


