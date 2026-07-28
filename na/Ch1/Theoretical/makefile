
OBJ = $(wildcard *.tex)

doc: ${OBJ}
	@xelatex $^
	@bibtex main.aux
	@xelatex $^
	@xelatex $^

.PHONY:clean
clean:
	rm -f *.aux *.log *.out *.synctex.gz  *.toc *.bbl *.run.xml *.blg