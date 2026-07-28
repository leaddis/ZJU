# 定义编译器
CXX = g++
CXXFLAGS = -std=c++17

# 定义源文件目录
TEST_DIR = test

# 定义目标文件
TARGETS = PTA stl

# 默认目标
all: $(TARGETS)

# PTA
PTA: $(TEST_DIR)/PTA_test.cpp
	$(CXX) $(CXXFLAGS) -o $(TEST_DIR)/PTA_test $(TEST_DIR)/PTA_test.cpp
	./$(TEST_DIR)/PTA_test

stl: $(TEST_DIR)/PTA_stl_test.cpp
	$(CXX) $(CXXFLAGS) -o $(TEST_DIR)/PTA_stl_test $(TEST_DIR)/PTA_stl_test.cpp
	./$(TEST_DIR)/PTA_stl_test


allocator: $(TEST_DIR)/test_allocator.cpp
	$(CXX) $(CXXFLAGS) -o $(TEST_DIR)/test_allocator $(TEST_DIR)/test_allocator.cpp
	./$(TEST_DIR)/test_allocator

buffer: $(TEST_DIR)/test_buffer.cpp
	$(CXX) $(CXXFLAGS) -o $(TEST_DIR)/test_buffer $(TEST_DIR)/test_buffer.cpp
	./$(TEST_DIR)/test_buffer

mp: $(TEST_DIR)/test_memory_pool.cpp
	$(CXX) $(CXXFLAGS) -o $(TEST_DIR)/test_memory_pool $(TEST_DIR)/test_memory_pool.cpp
	./$(TEST_DIR)/test_memory_pool

clean:
	rm -f $(TEST_DIR)/PTA_test $(TEST_DIR)/test_allocator $(TEST_DIR)/test_buffer $(TEST_DIR)/test_memory_pool $(TEST_DIR)/PTA_stl_test

