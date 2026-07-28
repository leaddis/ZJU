# 定义编译器
CXX = g++

# 定义源文件目录
SRC_DIR = src

# 定义报告文件目录
REPORT_DIR = report

# 定义目标文件
TARGETS = A B doc

# 定义源文件
SRCS = $(wildcard $(SRC_DIR)/*.cpp)

# 默认目标
all: $(TARGETS)

A: $(SRC_DIR)/A.cpp 
	$(CXX) -std=c++17 -o $(SRC_DIR)/A $(SRC_DIR)/A.cpp
	cd $(SRC_DIR) && ./A
	cd $(SRC_DIR) && python3 A.py

B: $(SRC_DIR)/B.cpp 
	$(CXX) -std=c++17 -o $(SRC_DIR)/B $(SRC_DIR)/B.cpp
	cd $(SRC_DIR) && ./B
	cd $(SRC_DIR) && python3 B.py

# 编译报告
doc:
	cd $(REPORT_DIR) && xelatex report.tex
	cd $(REPORT_DIR) && bibtex report
	cd $(REPORT_DIR) && xelatex report.tex
	cd $(REPORT_DIR) && xelatex report.tex

# 清理生成的文件
clean:
	rm -f $(SRC_DIR)/A $(SRC_DIR)/B $(SRC_DIR)/*.txt $(SRC_DIR)/*.csv
	rm -f $(REPORT_DIR)/report.aux $(REPORT_DIR)/report.bbl $(REPORT_DIR)/report.blg $(REPORT_DIR)/report.log 
	rm -f $(REPORT_DIR)/B.png $(REPORT_DIR)/C.png $(REPORT_DIR)/D.png $(REPORT_DIR)/E.png $(REPORT_DIR)/F.png