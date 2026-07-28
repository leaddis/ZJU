# Makefile for Personal Diary CLI application

CXX = g++
CXXFLAGS = -std=c++17

# Source files
DIARY_SRC = Diary.cpp
PDADD_SRC = pdadd.cpp
PDLIST_SRC = pdlist.cpp
PDSHOW_SRC = pdshow.cpp
PDREMOVE_SRC = pdremove.cpp

# Executable names
PDADD_EXE = pdadd
PDLIST_EXE = pdlist
PDSHOW_EXE = pdshow
PDREMOVE_EXE = pdremove

all: $(PDADD_EXE) $(PDLIST_EXE) $(PDSHOW_EXE) $(PDREMOVE_EXE)

$(PDADD_EXE): $(DIARY_SRC) $(PDADD_SRC)
	$(CXX) $(CXXFLAGS) $(DIARY_SRC) $(PDADD_SRC) -o $(PDADD_EXE)

$(PDLIST_EXE): $(DIARY_SRC) $(PDLIST_SRC)
	$(CXX) $(CXXFLAGS) $(DIARY_SRC) $(PDLIST_SRC) -o $(PDLIST_EXE)

$(PDSHOW_EXE): $(DIARY_SRC) $(PDSHOW_SRC)
	$(CXX) $(CXXFLAGS) $(DIARY_SRC) $(PDSHOW_SRC) -o $(PDSHOW_EXE)

$(PDREMOVE_EXE): $(DIARY_SRC) $(PDREMOVE_SRC)
	$(CXX) $(CXXFLAGS) $(DIARY_SRC) $(PDREMOVE_SRC) -o $(PDREMOVE_EXE)

# Find the next available log file number
LOG_FILE=$(shell i=1; while [ -e test_log_$$i.txt ]; do i=$$((i+1)); done; echo test_log_$$i.txt)

# Testfile target with unique log file
testfile: all
	@chmod +x test_script.sh
	@./test_script.sh > $(LOG_FILE)
	@echo "Test results saved to $(LOG_FILE)"

# Clean command to remove executables and test files
clean:
	rm -f $(PDADD_EXE) $(PDLIST_EXE) $(PDSHOW_EXE) $(PDREMOVE_EXE) diary.txt test_log_*.txt
