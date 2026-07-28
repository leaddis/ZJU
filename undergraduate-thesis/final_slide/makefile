# 定义编译器和主文件名
TEX = xelatex
TARGET = test
SRC = $(TARGET).tex

.PHONY: all clean clean-all

# 默认执行目标
all: $(TARGET).pdf

# 编译生成 PDF，执行两次以确保生成完整的目录和导航书签
$(TARGET).pdf: $(SRC)
	$(TEX) $(SRC)
	$(TEX) $(SRC)

# 清理编译产生的辅助文件（不删除 PDF）
clean:
	rm -f *.aux *.log *.nav *.out *.snm *.toc *.vrb *.bbl *.blg

# 清理所有生成的文件（包括 PDF）
clean-all: clean
	rm -f $(TARGET).pdf