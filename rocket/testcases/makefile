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
SRCS = test_eventloop.cc \
	../rocket/common/config.cc ../rocket/common/log.cc ../rocket/common/util.cc \
	../rocket/net/eventloop.cc ../rocket/net/fdevent.cc ../rocket/net/timer.cc \
	../rocket/net/timerevent.cc ../rocket/net/wakeupfd.cc

SRCS_IO = test_iothread.cc \
	../rocket/common/config.cc ../rocket/common/log.cc ../rocket/common/util.cc \
	../rocket/net/eventloop.cc ../rocket/net/fdevent.cc ../rocket/net/timer.cc \
	../rocket/net/timerevent.cc ../rocket/net/wakeupfd.cc	../rocket/net/io_thread.cc \
	../rocket/net/io_thread_pool.cc

SRCS_TCP = test_tcp.cc \
	../rocket/common/config.cc ../rocket/common/log.cc ../rocket/common/util.cc \
	../rocket/net/eventloop.cc ../rocket/net/fdevent.cc ../rocket/net/timer.cc \
	../rocket/net/timerevent.cc ../rocket/net/wakeupfd.cc	../rocket/net/io_thread.cc \
	../rocket/net/io_thread_pool.cc ../rocket/net/tcp/net_addr.cc ../rocket/net/tcp/tcp_acceptor.cc \
	../rocket/net/tcp/tcp_server.cc ../rocket/net/tcp/tcp_connection.cc ../rocket/net/tcp/tcp_buffer.cc \
	../rocket/net/fd_event_pool.cc

SRCS_CON = test_client.cc \
	../rocket/common/config.cc ../rocket/common/log.cc ../rocket/common/util.cc \
	../rocket/net/eventloop.cc ../rocket/net/fdevent.cc ../rocket/net/timer.cc \
	../rocket/net/timerevent.cc ../rocket/net/wakeupfd.cc	../rocket/net/io_thread.cc \
	../rocket/net/io_thread_pool.cc ../rocket/net/tcp/net_addr.cc ../rocket/net/tcp/tcp_acceptor.cc \
	../rocket/net/tcp/tcp_server.cc ../rocket/net/tcp/tcp_connection.cc ../rocket/net/tcp/tcp_buffer.cc \
	../rocket/net/fd_event_pool.cc

# 编译所有可执行的文件
all: $(TARGET) $(TARGET_IO) $(TARGET_TCP) $(TARGET_CON)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) 
	@echo "编译完成：$@"

$(TARGET_IO): $(SRCS_IO)
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