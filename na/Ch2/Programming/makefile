# 定义编译器
CXX = g++

# 定义源文件目录
SRC_DIR = src

# 定义报告文件目录
REPORT_DIR = report

# 定义目标文件
TARGETS = B C D E F doc

# 定义源文件
SRCS = $(wildcard $(SRC_DIR)/*.cpp)

# 默认目标
all: $(TARGETS)

# 编译B.cpp
B: $(SRC_DIR)/B.cpp $(SRC_DIR)/Interpolation.cpp
	$(CXX) -std=c++17 -o $(SRC_DIR)/B $(SRC_DIR)/B.cpp $(SRC_DIR)/Interpolation.cpp
	cd $(SRC_DIR) && ./B
	cd $(SRC_DIR) && python3 B.py

# 编译C.cpp
C: $(SRC_DIR)/C.cpp $(SRC_DIR)/Interpolation.cpp
	$(CXX) -std=c++17 -o $(SRC_DIR)/C $(SRC_DIR)/C.cpp $(SRC_DIR)/Interpolation.cpp
	cd $(SRC_DIR) && ./C
	cd $(SRC_DIR) && python3 C.py
# 编译D.cpp
D: $(SRC_DIR)/D.cpp $(SRC_DIR)/Interpolation.cpp
	$(CXX) -std=c++17 -o $(SRC_DIR)/D $(SRC_DIR)/D.cpp $(SRC_DIR)/Interpolation.cpp
	cd $(SRC_DIR) && ./D
	cd $(SRC_DIR) && python3 D.py

# 编译E.cpp
E: $(SRC_DIR)/E.cpp $(SRC_DIR)/Interpolation.cpp
	$(CXX) -std=c++17 -o $(SRC_DIR)/E $(SRC_DIR)/E.cpp $(SRC_DIR)/Interpolation.cpp
	cd $(SRC_DIR) && ./E
	cd $(SRC_DIR) && python3 E.py

F: $(SRC_DIR)/F.cpp $(SRC_DIR)/Bezier.cpp
	$(CXX) -std=c++17 -o $(SRC_DIR)/F $(SRC_DIR)/F.cpp $(SRC_DIR)/Bezier.cpp
	cd $(SRC_DIR) && ./F
	cd $(SRC_DIR) && python3 F.py

# 编译报告
doc:
	cd $(REPORT_DIR) && xelatex report.tex
	cd $(REPORT_DIR) && bibtex report
	cd $(REPORT_DIR) && xelatex report.tex
	cd $(REPORT_DIR) && xelatex report.tex

# 清理生成的文件
clean:
	rm -f $(SRC_DIR)/B $(SRC_DIR)/C $(SRC_DIR)/D $(SRC_DIR)/E $(SRC_DIR)/F $(SRC_DIR)/*.txt
	rm -f $(REPORT_DIR)/report.aux $(REPORT_DIR)/report.bbl $(REPORT_DIR)/report.blg $(REPORT_DIR)/report.log 