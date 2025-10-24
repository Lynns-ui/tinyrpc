CXX = g++

CXXFLAGS = -std=c++11 -g -pthread
LINK = -ltinyxml2

TARGET = test_time

SORCS = test_eventloop.cc ../lynns/common/config.cc ../lynns/common/log.cc ../lynns/common/util.cc \
	../lynns/net/eventloop.cc ../lynns/net/fdevent.cc ../lynns/net/timer_event.cc \
	../lynns/net/timer.cc ../lynns/net/wakeupevent.cc

all: $(TARGET)

$(TARGET) : $(SORCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LINK)
	@echo "编译完成 $@"

clean:
	rm -f $(TARGET)