main.pdf: main.tex wordlattice.png wordlattice1.png wordlattice2.png syllable.png
	pdflatex $<&& pdflatex $<
%.png: %.dot
	DOTFONTPATH=~/.fonts dot $< -Tpng -o $@
wordlattice.ps: wordlattice.dot
	DOTFONTPATH=~/.fonts dot $< -Tps -o $@
