# 目标可执行文件
TARGET = main

# 源文件
SRCS = main.cpp map.cpp

# 对象文件
OBJS = $(SRCS:.cpp=.o)

# 编译器
CXX = g++

# 编译选项
CXXFLAGS = -Wall -std=c++11

# 默认目标
all: $(TARGET)

# 链接目标文件生成可执行文件
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 编译每个源文件生成对象文件
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理生成的文件
clean:
	rm -f $(OBJS) $(TARGET)

# 伪目标，防止与同名文件冲突
.PHONY: all clean