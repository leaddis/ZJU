# ===== LaTeX Makefile =====

TEX = main
LATEX = xelatex
BIB = bibtex
LATEXFLAGS = -interaction=nonstopmode -halt-on-error

.PHONY: all clean distclean

all:
	$(LATEX) $(LATEXFLAGS) $(TEX).tex
	$(BIB) contents/01_Review
	$(BIB) contents/02_Report
	$(LATEX) $(LATEXFLAGS) $(TEX).tex
	$(LATEX) $(LATEXFLAGS) $(TEX).tex

clean:
	rm -f *.aux *.log *.nav *.out *.snm *.toc *.vrb *.fls *.fdb_latexmk *.synctex.gz *.bbl *.blg
	rm -f contents/*.aux contents/*.bbl contents/*.blg contents/*.out

distclean: clean
	rm -f $(TEX).pdf

