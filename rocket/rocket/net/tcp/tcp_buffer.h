#ifndef ROCKET_NET_TCPBUFFER_H
#define ROCKET_NET_TCPBUFFER_H

#include <vector>
#include <memory>

namespace rocket {

class TcpBuffer {
public:
    typedef std::shared_ptr<TcpBuffer> s_ptr;

    TcpBuffer(int size = 1024);

    ~TcpBuffer() = default;

    int readBytes();

    int writeBytes();

    int readIndex();

    int writeIndex();

    void writeToBuffer(const char* buff, int size);

    void readFromBuffer(std::vector<char>& re, int size);

    void resizeBuffer(int new_size);

    void adjustBuffer();

    void moveReadIndex(int size);

    void moveWriteIndex(int size);

    char* writePtr();
    char* readPtr();

    int buffSize();

    std::vector<char> getBuffer() {
        return m_buff;
    }

private:
    int m_writePos;
    int m_readPos;
    std::vector<char> m_buff;

};


}

#endif