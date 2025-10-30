#ifndef LYNNS_TCP_BUFFER_H
#define LYNNS_TCP_BUFFER_H

#include <vector>
#include <atomic>

namespace lynns {

class TcpBuffer {
public:
    TcpBuffer(int initSize = 1024);

    ~TcpBuffer();

    int writeBytes();

    int readBytes();

    int prependBytes();

    void writeToBuffer(const char* buff, int len);

    void readFrmoBuffer(std::vector<char>& buff, int len);
    
    void resizeBuffer(int newsize);

    void moveWriteIndex(int len);

    void moveReadIndex(int len);

    int bufferSize();

    const char* bufferBeginPtr();

private:
    void adjustBuffer();

    std::vector<char> buffer_;

    std::atomic<int> writePos_;
    
    std::atomic<int> readPos_;

};



}



#endif