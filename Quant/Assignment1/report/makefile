OBJ = $(wildcard *.tex)

doc: ${OBJ}
	@xelatex $^

.PHONY:clean
clean:
	rm -f *.aux *.log *.out *.synctex.gz  *.toc *.bbl *.run.xml *.blg report-blx.bib *.fdb_latexmk *.fls *.bcf