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

    READ_XML_NODE(log, root_node);

    READ_STR_XML_NODE(log_level, log_node);

    m_log_level = log_level_str;
    
}


}


