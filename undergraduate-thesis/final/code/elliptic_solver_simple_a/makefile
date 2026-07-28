# 二维椭圆方程求解器 Makefile
# 用法:
#   make        - 运行主求解程序
#   make quick  - 运行快速启动
#   make clean  - 清理生成的文件
#   make all    - 运行完整求解流程

.PHONY: all quick clean

# 默认目标：运行主求解程序
all: run_simulation

# 运行主求解程序
run_simulation:
	@echo "=========================================="
	@echo "运行二维椭圆方程求解器"
	@echo "=========================================="
	python run_simulation.py

# 清理所有生成的文件
clean:
	@echo "清理生成的文件..."
	
	# 删除 __pycache__ 目录
	@if [ -d "__pycache__" ]; then \
		echo "删除 __pycache__ 目录..."; \
		rm -rf __pycache__; \
	fi
	
	# 删除所有 .pyc 文件
	@echo "删除 .pyc 文件..."
	@find . -name "*.pyc" -delete
	
	# 清理 plots 目录（如果存在）
	@if [ -d "plots" ]; then \
		echo "清理 plots 目录..."; \
		rm -rf plots/*; \
		echo "plots 目录已清空"; \
	else \
		echo "plots 目录不存在，跳过清理"; \
	fi
	
	# 清理可能存在的临时文件
	@echo "清理其他临时文件..."
	@find . -name "*.log" -delete
	@find . -name "*.tmp" -delete
	
	@echo "=========================================="
	@echo "清理完成！"
	@echo "=========================================="

# 显示帮助信息
help:
	@echo "二维椭圆方程求解器 Makefile 命令:"
	@echo ""
	@echo "  make        - 运行主求解程序 (run_simulation.py)"
	@echo "  make clean  - 清理所有生成的文件 (__pycache__, plots/*, 等)"
	@echo "  make help   - 显示此帮助信息"
	@echo ""
	@echo "配置文件: 编辑 user_config.py 来修改求解参数"
	@echo "输出目录: plots/"

# 显示当前配置状态
status:
	@echo "=========================================="
	@echo "当前配置状态"
	@echo "=========================================="
	@echo "Python 文件:"
	@ls -la *.py | wc -l | xargs echo "  数量:"
	@echo ""
	@echo "plots 目录:"
	@if [ -d "plots" ]; then \
		ls plots/*.png 2>/dev/null | wc -l | xargs echo "  图像文件数量:"; \
	else \
		echo "  目录不存在"; \
	fi
	@echo ""
	@echo "__pycache__ 目录:"
	@if [ -d "__pycache__" ]; then \
		echo "  存在"; \
	else \
		echo "  不存在"; \
	fi
	@echo "=========================================="