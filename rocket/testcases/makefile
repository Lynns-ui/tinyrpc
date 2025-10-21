# 编译器和编译选项
CXX = g++
CXXFLAGS = -std=c++11 -g -pthread
LDFLAGS = -ltinyxml2

# 目标可执行文件
TARGET = test_event

# 源文件
SRCS = test_eventloop.cc \
	../rocket/common/config.cc ../rocket/common/log.cc ../rocket/common/util.cc \
	../rocket/net/eventloop.cc ../rocket/net/fdevent.cc ../rocket/net/timer.cc \
	../rocket/net/timerevent.cc ../rocket/net/wakeupfd.cc

# 生成可执行文件
all:
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)  $(LDFLAGS) 
	@echo "编译完成：$(TARGET)"

clean:
	rm -f $(TARGET)
	@echo "清理完成"