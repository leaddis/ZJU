OBJ = $(wildcard *.tex)

doc: ${OBJ}
	@xelatex $^
	@bibtex HW6_3220104074_QianZhouyue.aux
	@xelatex $^
	@xelatex $^

.PHONY:clean
clean:
	rm -f *.aux *.log *.out *.synctex.gz  *.toc *.bbl *.run.xml *.blg