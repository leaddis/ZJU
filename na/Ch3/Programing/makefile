# 定义编译器
CXX = g++
CXXFLAGS = -std=c++17 -I../ -I/usr/include/eigen3

# 定义源文件目录
SRC_DIR = src

# 定义报告文件目录
REPORT_DIR = report

# 定义目标文件
TARGETS = A C D E1 E2 E3 F1 F2 doc

# 定义源文件
SRCS = $(wildcard $(SRC_DIR)/*.cpp)

# 默认目标
all: $(TARGETS)

# 编译A.cpp
A: $(SRC_DIR)/A.cpp $(SRC_DIR)/Polynomial.cpp
	$(CXX) $(CXXFLAGS) -o $(SRC_DIR)/A $(SRC_DIR)/A.cpp $(SRC_DIR)/Polynomial.cpp $(SRC_DIR)/Spline.cpp $(SRC_DIR)/Base_Bspline.cpp
	cd $(SRC_DIR) && ./A
	cd $(SRC_DIR) && python3 A.py

# 编译B.cpp
# B: $(SRC_DIR)/B.cpp $(SRC_DIR)/Polynomial.cpp
# 	$(CXX) $(CXXFLAGS) -o $(SRC_DIR)/B $(SRC_DIR)/B.cpp $(SRC_DIR)/Polynomial.cpp $(SRC_DIR)/Spline.cpp $(SRC_DIR)/Base_Bspline.cpp
# 	cd $(SRC_DIR) && ./B
# 	cd $(SRC_DIR) && python3 B.py

# 编译C.cpp
C: $(SRC_DIR)/C.cpp $(SRC_DIR)/Polynomial.cpp
	$(CXX) $(CXXFLAGS) -o $(SRC_DIR)/C $(SRC_DIR)/C.cpp $(SRC_DIR)/Polynomial.cpp $(SRC_DIR)/Spline.cpp $(SRC_DIR)/Base_Bspline.cpp
	cd $(SRC_DIR) && ./C
	cd $(SRC_DIR) && python3 C.py
# 编译D.cpp
D: $(SRC_DIR)/D.cpp $(SRC_DIR)/Polynomial.cpp
	$(CXX) $(CXXFLAGS) -o $(SRC_DIR)/D $(SRC_DIR)/D.cpp $(SRC_DIR)/Polynomial.cpp $(SRC_DIR)/Spline.cpp $(SRC_DIR)/Base_Bspline.cpp
	cd $(SRC_DIR) && ./D
	cd $(SRC_DIR) && python3 D.py

# 编译E.cpp
E1: $(SRC_DIR)/E1.cpp $(SRC_DIR)/Polynomial.cpp
	$(CXX) $(CXXFLAGS) -o $(SRC_DIR)/E1 $(SRC_DIR)/E1.cpp $(SRC_DIR)/Polynomial.cpp $(SRC_DIR)/Spline.cpp $(SRC_DIR)/Base_Bspline.cpp
	cd $(SRC_DIR) && ./E1
	cd $(SRC_DIR) && python3 E1.py

E2: $(SRC_DIR)/E2.cpp $(SRC_DIR)/Polynomial.cpp
	$(CXX) $(CXXFLAGS) -o $(SRC_DIR)/E2 $(SRC_DIR)/E2.cpp $(SRC_DIR)/Polynomial.cpp $(SRC_DIR)/Spline.cpp $(SRC_DIR)/Base_Bspline.cpp
	cd $(SRC_DIR) && ./E2
	cd $(SRC_DIR) && python3 E2.py

E3: $(SRC_DIR)/E3.cpp $(SRC_DIR)/Polynomial.cpp
	$(CXX) $(CXXFLAGS) -o $(SRC_DIR)/E3 $(SRC_DIR)/E3.cpp $(SRC_DIR)/Polynomial.cpp $(SRC_DIR)/Spline.cpp $(SRC_DIR)/Base_Bspline.cpp
	cd $(SRC_DIR) && ./E3
	cd $(SRC_DIR) && python3 E3.py

F1: $(SRC_DIR)/F1.cpp 
	$(CXX) $(CXXFLAGS) -o $(SRC_DIR)/F1 $(SRC_DIR)/F1.cpp $(SRC_DIR)/Polynomial.cpp $(SRC_DIR)/Spline.cpp $(SRC_DIR)/Base_Bspline.cpp
	cd $(SRC_DIR) && ./F1
	cd $(SRC_DIR) && python3 F1.py

F2: $(SRC_DIR)/F2.cpp 
	$(CXX) $(CXXFLAGS) -o $(SRC_DIR)/F2 $(SRC_DIR)/F2.cpp $(SRC_DIR)/Polynomial.cpp $(SRC_DIR)/Spline.cpp $(SRC_DIR)/Base_Bspline.cpp
	cd $(SRC_DIR) && ./F2
	cd $(SRC_DIR) && python3 F2.py

# 编译报告
doc:
	cd $(REPORT_DIR) && xelatex report.tex
	cd $(REPORT_DIR) && bibtex report
	cd $(REPORT_DIR) && xelatex report.tex
	cd $(REPORT_DIR) && xelatex report.tex

# 清理生成的文件
clean:
	rm -f $(SRC_DIR)/A  $(SRC_DIR)/C $(SRC_DIR)/D $(SRC_DIR)/E1 $(SRC_DIR)/E2 $(SRC_DIR)/E3 $(SRC_DIR)/F1 $(SRC_DIR)/F2 $(SRC_DIR)/*.txt $(SRC_DIR)/*.csv $(SRC_DIR)/*.png
	rm -f $(REPORT_DIR)/report.aux $(REPORT_DIR)/report.bbl $(REPORT_DIR)/report.blg $(REPORT_DIR)/report.log $(REPORT_DIR)/report.synctex.gz $(REPORT_DIR)/report.out $(REPORT_DIR)/report.fls $(REPORT_DIR)/report.run.xml $(REPORT_DIR)/report.fdb_latexmk $(REPORT_DIR)/report-blx.bib 