PATH_BIN = bin
PATH_LIB = lib
PATH_OBJ = obj

PATH_ROCKET = rocket
PATH_COMM = $(PATH_ROCKET)/comm
PATH_NET = $(PATH_ROCKET)/net

PATH_TESTCASES = testcases

# will install lib to /usr/lib/librocket.a
PATH_INSTALL_LIB_ROOT = /usr/lib

# will install all header file to /usr/include/tinyrpc
PATH_INSTALL_INC_ROOT = /usr/include

PATH_INSTALL_INC_COMM = $(PATH_INSTALL_INC_ROOT)/$(PATH_COMM)
PATH_INSTALL_INC_NET = $(PATH_INSTALL_INC_ROOT)/$(PATH_NET)

# PATH_PROTOBUF = /usr/include/google
# PATH_TINYXML = /usr/include/tinyxml

CXX := g++

CXXFLAGS += -g -O0 -std=c++11 -Wall -Wno-deprecated -Wno-unused-but-set-variable
# add lib plugin
# CXXFLAGS += -g -O0 -std=c++11 -Wall -Wno-deprecated -Wno-unused-but-set-variable -D DECLARE_MYSQL_PLUGIN
CXXFLAGS += -I./ -I$(PATH_ROCKET) -I$(PATH_COMM) -I$(PATH_NET)
# CXXFLAGS += -I./ -I$(PATH_TINYRPC)	-I$(PATH_COMM) -I$(PATH_COROUTINE) -I$(PATH_NET) -I$(PATH_HTTP) -I$(PATH_TCP) -I$(PATH_TINYPB)

LIBS += /usr/lib/libprotobuf.a	/usr/lib/libtinyxml.a

COMM_OBJ := $(patsubst $(PATH_COMM)/%.cc, $(PATH_OBJ)/%.o, $(wildcard $(PATH_COMM)/*.cc))
NET_OBJ := $(patsubst $(PATH_NET)/%.cc, $(PATH_OBJ)/%.o, $(wildcard $(PATH_NET)/*.cc))


ALL_TESTS : $(PATH_BIN)/test_log

TEST_CASE_OUT := $(PATH_BIN)/test_log

LIB_OUT := $(PATH_LIB)/librocket.a

$(PATH_BIN)/test_log: $(LIB_OUT)
	$(CXX) $(CXXFLAGS) $(PATH_TESTCASES)/test_log.cc -o $@ $(LIB_OUT) $(LIBS) -ldl -pthread

# print something test
# like this: make PRINT-PATH_BIN, and then will print variable PATH_BIN
PRINT-% : ; @echo $* = $($*)

# 清理编译生成的文件
clean:
	rm -f $(COMM_OBJ) $(NET_OBJ)  # 清理目标文件
	rm -f $(LIB_OUT)             # 清理静态库
	rm -f $(TEST_CASE_OUT)       # 清理测试程序
	rm -rf $(PATH_BIN)/* $(PATH_OBJ)/*  # 清理bin和obj目录下的内容（可选）

# 安装库文件和头文件
install: $(LIB_OUT)
	# 创建安装目录
	mkdir -p $(PATH_INSTALL_LIB_ROOT)
	mkdir -p $(PATH_INSTALL_INC_COMM)
	mkdir -p $(PATH_INSTALL_INC_NET)
	
	# 复制头文件到对应目录
	cp $(PATH_COMM)/*.h $(PATH_INSTALL_INC_COMM)/
	cp $(PATH_NET)/*.h $(PATH_INSTALL_INC_NET)/
	
	# 复制静态库到系统库目录
	cp $(LIB_OUT) $(PATH_INSTALL_LIB_ROOT)/
	@echo "Install completed: librocket.a installed to $(PATH_INSTALL_LIB_ROOT), headers to $(PATH_INSTALL_INC_ROOT)"

# 卸载已安装的文件
uninstall:
	# 删除安装的头文件目录
	rm -rf $(PATH_INSTALL_INC_COMM)
	rm -rf $(PATH_INSTALL_INC_NET)
	
	# 删除安装的静态库
	rm -f $(PATH_INSTALL_LIB_ROOT)/librocket.a
	@echo "Uninstall completed: removed librocket.a and headers"