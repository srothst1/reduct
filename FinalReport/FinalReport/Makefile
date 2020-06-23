#
# make: builds ps and pdf versions using latex vs. pdflatex
# make spell: runs ispell on .tex file
#
TARGET=finalreport

all: 
	pdflatex ${TARGET}.tex
	bibtex ${TARGET} 
	pdflatex ${TARGET}
	pdflatex ${TARGET}

spell:
	ispell ${TARGET}.tex

clean: 
	$(RM) *.dvi *.log *.aux *.bbl  *.blg ${TARGET}.pdf ${TARGET}.ps ${TARGET}.out *.bak

