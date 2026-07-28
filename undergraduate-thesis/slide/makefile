TEX_FILE = test.tex

doc: $(TEX_FILE)
	@echo "Compiling $(TEX_FILE)..."
	@xelatex $(TEX_FILE) || { echo "LaTeX compilation of $(TEX_FILE) failed!"; exit 1; }
	@xelatex $(TEX_FILE) || { echo "LaTeX compilation of $(TEX_FILE) failed!"; exit 1; }

.PHONY: clean
clean:
	rm -f *.aux *.log *.out *.synctex.gz *.toc *.bbl *.run.xml *.blg *.snm *.nav *.fdb_latexmk *.fls