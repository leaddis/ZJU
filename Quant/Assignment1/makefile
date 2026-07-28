# 默认目标：编译并运行 main.py
all:
	python3 main.py
	python3 plot.py

# 清理目标：删除当前目录下的所有文件夹
clean:
	@echo "Cleaning up directories..."
	@find . -mindepth 1 -type d -exec rm -rf {} +
	@echo "All directories removed."

# 防止与文件名为 "clean" 的目标冲突
.PHONY: all clean