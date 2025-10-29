#include <string.h>
#include "tcp_buffer.h"
#include "../../common/log.h"

namespace rocket {

TcpBuffer::TcpBuffer(int size /*=1024*/) : m_writePos(0), m_readPos(0), m_buff(size) {

}

int TcpBuffer::readBytes() {
    return m_writePos - m_readPos;
}

int TcpBuffer::writeBytes() {
    return m_buff.size() - m_writePos;
}

int TcpBuffer::readIndex() {
    return m_readPos;
}

int TcpBuffer::writeIndex() {
    return m_writePos;
}

void TcpBuffer::writeToBuffer(const char* buff, int size) {
    // 当写入的数据大于可写区域时
    if (size > writeBytes()) {
        DEBUGLOG("write size[%d] > m_buff's writeable[%d]", size, writeBytes());
        int new_size = (int)(m_writePos + size + 1);
        resizeBuffer(new_size);
    }
    memcpy(&m_buff[m_writePos], buff, size);
    m_writePos += size;
}

void TcpBuffer::readFromBuffer(std::vector<char>& re, int size) {
    if (readBytes() == 0) {
        return;
    }
    int read_size = readBytes() > size ? size : readBytes();
    std::vector<char> tmp(read_size);
    memcpy(&tmp[0], &m_buff[m_readPos], read_size);
    re.swap(tmp);
    m_readPos += read_size;
    adjustBuffer();
}

void TcpBuffer::resizeBuffer(int new_size) {
    if (new_size <= 0) {
        return;
    }
    int read_size = readBytes();

    std::vector<char> tmp(new_size);
    // 将后面的数据拷贝count个到前面
    memcpy(&tmp[0], &m_buff[m_readPos], read_size);
    // m_buff.resize(new_size);
    m_buff.swap(tmp);

    m_readPos = 0;
    m_writePos = read_size;
}

// 调整数组
void TcpBuffer::adjustBuffer() {
    if (m_readPos < (int) (m_buff.size() / 3)) {
        return;
    }
    std::vector<char> tmp(m_buff.size());
    int readsize = readBytes();
    memcpy(&tmp[0], &m_buff[m_readPos], readsize);
    m_buff.swap(tmp);
    m_readPos = 0;
    m_writePos = readsize;
}

void TcpBuffer::moveReadIndex(int size) {
    size_t j = m_readPos + size;
    // 禁止读指针超过写指针?
    if (j > m_writePos) {
        ERRORLOG("move readIndex error, invalid size %d, old_read_index %d, buffer size %d",size, m_readPos, m_buff.size());
        return;
    }
    m_readPos = j;
    adjustBuffer();
}

void TcpBuffer::moveWriteIndex(int size) {
    size_t j = m_writePos + size;
    if (j > m_buff.size()) {
        ERRORLOG("move writeIndex error, invalid size %d, old_read_index %d, buffer size %d", size, m_writePos, m_buff.size());
        return;
    }
    m_writePos = j;
    adjustBuffer();
}

char* TcpBuffer::writePtr() {
    return m_buff.data() + m_writePos;
}

int TcpBuffer::buffSize() {
    return m_buff.size();
}

char* TcpBuffer::readPtr() {
    return m_buff.data() + m_readPos;
}

}