EXT = .png
main$(EXT): main.tex wordlattice$(EXT) wordlattice1$(EXT) wordlattice2$(EXT) syllable$(EXT) overall.png test.tex
	pdflatex $<&& pdflatex $<
test.tex:../tests/data.test
	perl ../scripts/testdoc-extract.pl < $< > $@
%.png: %.dot
	DOTFONTPATH=. dot -Tpng -o $@ $<
%.mp: %.dot
	(echo "beginfig(1);";DOTFONTPATH=~/.fonts dot $< -Tmp;echo "endfig;";echo "end;") > $@
%.mps: %.mp
	TEX=latex mpost $< && (F="$<";mv $${F/mp/1} $@)
%.pdf: %.mps
	pdfetex -progname=context -efmt=mptopdf \\relax $<
wordlattice.ps: wordlattice.dot
	DOTFONTPATH=~/.fonts dot $< -Tps -o $@
%.png: %.dia
	dia -e $@ -t png $<
