# 编译器
CXX = g++

# 编译选项
CXXFLAGS = -std=c++11 -Wall

# 源文件目录
SRC_DIR = src

# 可执行文件目录
BIN_DIR = bin

# 源文件列表
SRCS = $(wildcard $(SRC_DIR)/*.cpp)

# 可执行文件列表
BINS = $(patsubst $(SRC_DIR)/%.cpp, $(BIN_DIR)/%, $(filter-out $(SRC_DIR)/EquationSolver.cpp, $(SRCS)))

# 默认目标
all: $(BINS)

# 生成可执行文件的规则
$(BIN_DIR)/%: $(SRC_DIR)/%.cpp $(SRC_DIR)/EquationSolver.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(SRC_DIR)/EquationSolver.cpp

# 清理目标
clean:
	rm -rf $(BIN_DIR)/*
	rm -f report/*.aux report/*.log report/*.out report/*.synctex.gz  report/*.toc *.bbl report/*.run.xml report/*.blg

# 文档生成目标
doc:
	cd report && xelatex report.tex
	cd report && bibtex report
	cd report && xelatex report.tex
	cd report && xelatex report.tex

# 创建目录
$(shell mkdir -p $(BIN_DIR))