# 编译器和编译选项
CXX = g++
CXXFLAGS = -std=c++11 -g -pthread
LDFLAGS = -ltinyxml2 -lprotobuf

# 目标可执行文件
TARGETS = test_event test_io test_tcp test_connect \
	test_rpc_client test_rpc_server \
	test_log

# main函数入口文件
MAIN_LOG = test_log.cc
MAIN_EVENT = test_eventloop.cc
MAIN_IO = test_iothread.cc
MAIN_TCP = test_tcp.cc
MAIN_CON = test_client.cc
MAIN_RPC_CLI = test_rpc_client.cc
MAIN_RPC_SER = test_rpc_server.cc

# 源文件
SRCS = ../rocket/common/*.cc \
	../rocket/net/*.cc \
	../rocket/net/tcp/*.cc \
	../rocket/net/coder/*.cc \
	../rocket/net/rpc/*.cc \
	order.pb.cc 

# 编译所有可执行的文件
all: $(TARGETS)

test_log: $(MAIN_LOG) $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) 
	@echo "编译完成：$@"

test_event: $(MAIN_EVENT) $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) 
	@echo "编译完成：$@"

test_io: $(MAIN_IO) $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "编译完成：$@"

test_tcp: $(MAIN_TCP) $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "编译完成：$@"

test_connect: $(MAIN_CON) $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "编译完成：$@"

test_rpc_client: $(MAIN_RPC_CLI) $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "编译完成：$@"

test_rpc_server: $(MAIN_RPC_SER) $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "编译完成：$@"

clean:
	rm -f $(TARGETS)
	@echo "清理完成"