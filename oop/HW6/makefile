# 指定编译器
CXX = g++
# 编译器选项
CXXFLAGS = -std=c++11 -Wall
# 目标文件名
TARGET = main
# 源文件
SRC = main.cpp

# 默认目标
all: $(TARGET)

# 链接和生成可执行文件
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)
	./$(TARGET)

# 清理生成的文件
clean:
	rm -f $(TARGET)


