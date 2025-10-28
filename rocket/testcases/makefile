# 编译器和编译选项
CXX = g++
CXXFLAGS = -std=c++11 -g -pthread
LDFLAGS = -ltinyxml2

# 目标可执行文件
TARGET = test_event
TARGET_IO = test_io
TARGET_TCP = test_tcp
TARGET_CON = test_connect



# 源文件
SRCS = test_iothread.cc \
	../rocket/common/*.cc  \
	../rocket/net/*.cc \
	../rocket/net/tcp/*.cc

SRCS_TCP = test_tcp.cc \
	../rocket/common/*.cc  \
	../rocket/net/*.cc \
	../rocket/net/tcp/*.cc

SRCS_CON = test_client.cc \
	../rocket/common/*.cc  \
	../rocket/net/*.cc \
	../rocket/net/tcp/*.cc

# 编译所有可执行的文件
all: $(TARGET) $(TARGET_IO) $(TARGET_TCP) $(TARGET_CON)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) 
	@echo "编译完成：$@"

$(TARGET_IO): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "编译完成：$@"

$(TARGET_TCP): $(SRCS_TCP)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "编译完成：$@"

$(TARGET_CON): $(SRCS_CON)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "编译完成：$@"

clean:
	rm -f $(TARGET) $(TARGET_IO) $(TARGET_TCP) $(TARGET_CON)
	@echo "清理完成"