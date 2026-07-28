# 项目参数
MAIN = zjuthesis
COMPILER = xelatex
BIB_TOOL = bibtex
OUTPUT_DIR = out

# 默认目标
all: pdf

# 移除 -use-make，使用基础命令
pdf: $(MAIN).tex
	latexmk -$(COMPILER) -shell-escape $(MAIN).tex

# 纯手动强制编译流程（如果 pdf 命令依然不出参考文献，请运行 make manual）
manual:
	$(COMPILER) -shell-escape $(MAIN)
	$(BIB_TOOL) $(MAIN)
	$(COMPILER) -shell-escape $(MAIN)
	$(COMPILER) -shell-escape $(MAIN)

# 深度清理临时文件
clean:
	latexmk -c
	rm -rf *.aux *.log *.out *.toc *.bbl *.blg *.fdb_latexmk *.fls *.run.xml *.bcf *.pyg *.xdv *.bib
	find . -name "*.aux" -type f -delete 

# 彻底清理（包括 PDF）
distclean: clean
	rm -f $(MAIN).pdf

.PHONY: all pdf manual clean distclean