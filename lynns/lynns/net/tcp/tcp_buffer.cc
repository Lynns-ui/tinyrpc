#include <string.h>
#include <assert.h>
#include "tcp_buffer.h"
#include "../../common/log.h"

namespace lynns {

TcpBuffer::TcpBuffer(int initSize) : buffer_(initSize), writePos_(0), readPos_(0) {

}

TcpBuffer::~TcpBuffer() {

}

int TcpBuffer::writeBytes() {
    return buffer_.size() - writePos_;
}

int TcpBuffer::readBytes() {
    return writePos_ - readPos_;
}

int TcpBuffer::prependBytes() {
    return readPos_;
}

void TcpBuffer::writeToBuffer(const char* buff, int len) {
    if (writeBytes() < len) {
        DEBUGLOG("write to buffer size[%d] > writeableBytes[%d]", len, writeBytes);
        int newsize = (int)((writePos_ + len) * 1.5);
        resizeBuffer(newsize);
    }
    memcpy(&buffer_[writePos_], buff, len);
    writePos_ += len;
}

void TcpBuffer::readFrmoBuffer(std::vector<char>& res, int len) {
    if (readBytes() == 0) {
        ERRORLOG("buffer readsize is 0");
        return;
    }
    int read_size = readBytes() > len ? len : readBytes();
    std::vector<char> tmp(read_size);
    memcpy(&tmp[0], &buffer_[readPos_], read_size);
    res.swap(tmp);
    readPos_ += read_size;
    adjustBuffer();
}

void TcpBuffer::resizeBuffer(int newsize) {
    std::vector<char> tmp(newsize);
    int read_size = readBytes();
    memcpy(tmp.data(), &buffer_[readPos_], read_size);
    buffer_.swap(tmp);
    readPos_ = 0;
    writePos_ = read_size;
}

void TcpBuffer::moveWriteIndex(int len) {
    int j = len + writePos_;
    if (j > buffer_.size()) {
        ERRORLOG("move writeIndex error, invalid size %d, old_write_index %d, buffer size %d", len, writePos_, buffer_.size());
        return;
    }
    writePos_ = j;
    adjustBuffer();
}

void TcpBuffer::moveReadIndex(int len) {
    int j = len + readPos_;
    if (j > writePos_) {
        ERRORLOG("move readIndex error, invalid size %d, old_read_index %d, buffer writeIndex %d", len, readPos_, writePos_);
        return;
    }
    readPos_ = j;
    adjustBuffer();
}

void TcpBuffer::adjustBuffer() {
    if (prependBytes() < buffer_.size()) {
        return;
    }
    int read_size = readBytes();
    memcpy(&buffer_[0], &buffer_[readPos_], read_size);
    readPos_ = 0;
    writePos_ = read_size;
    assert(readBytes() == read_size);
}

int TcpBuffer::bufferSize() {
    return buffer_.size();
}

const char* TcpBuffer::bufferBeginPtr() {
    return buffer_.data();
}

}