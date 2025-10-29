CXX = g++

CXXFLAGS = -std=c++11 -g -pthread
LINK = -ltinyxml2

TARGETS = test_time test_iothread

MAIN_IO = test_iothread.cc

SORCS = ../lynns/common/*.cc \
	../lynns/net/*.cc \

all: $(TARGET)

test_iothread: $(MAIN_IO) $(SORCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LINK)
	@echo "编译完成 $@"

clean:
	rm -f $(TARGET)
	@echo "清理完成"