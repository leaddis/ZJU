# 定义编译器
CXX = g++
CXXFLAGS = -std=c++17 -I../ -I/usr/include/eigen3

# 定义目录
SRC_DIR = ..
TEST_DIR = .

# 定义目标文件
TARGETS = $(TEST_DIR)/test_poly $(TEST_DIR)/test_pp3 $(TEST_DIR)/test_pp1 $(TEST_DIR)/test_base	 $(TEST_DIR)/test_B
# 定义目标文件
POLY_TARGET = $(TEST_DIR)/test_poly
PP1_TARGET = $(TEST_DIR)/test_pp1
PP3_TARGET = $(TEST_DIR)/test_pp3
BASE_TARGET = $(TEST_DIR)/test_base
B_TARGET = $(TEST_DIR)/test_B

# 默认目标
all: $(TARGETS)

# 编译 test_poly
poly: $(POLY_TARGET)
$(TEST_DIR)/test_poly: $(TEST_DIR)/test_poly.cpp $(SRC_DIR)/Polynomial.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^
	./test_poly

# 编译 test_pp3
pp3: $(PP3_TARGET)
$(TEST_DIR)/test_pp3: $(TEST_DIR)/test_pp3.cpp $(SRC_DIR)/Spline.cpp $(SRC_DIR)/Polynomial.cpp $(SRC_DIR)/Base_Bspline.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^
	./test_pp3
	python3 test_pp3.py

# 编译 test_pp1
pp1: $(PP1_TARGET)
$(TEST_DIR)/test_pp1: $(TEST_DIR)/test_pp1.cpp $(SRC_DIR)/Spline.cpp $(SRC_DIR)/Polynomial.cpp $(SRC_DIR)/Base_Bspline.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^
	./test_pp1
	python3 test_pp1.py

base: $(BASE_TARGET)
$(TEST_DIR)/test_base: $(TEST_DIR)/test_base.cpp $(SRC_DIR)/Base_Bspline.cpp $(SRC_DIR)/Polynomial.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^
	./test_base
	python3 test_base.py

B: $(B_TARGET)
$(TEST_DIR)/test_B: $(TEST_DIR)/test_B.cpp $(SRC_DIR)/Base_Bspline.cpp $(SRC_DIR)/Polynomial.cpp $(SRC_DIR)/Spline.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^
	./test_B
	python3 test_B.py

# 清理生成的文件
clean:
	rm -f $(TARGETS) *.txt *.png *.csv

.PHONY: all clean run_spline
