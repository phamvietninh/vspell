SRILM_LIBS =  -lsrilm #-loolm  -ldstruct -lmisc 
SRILM_CXXFLAGS = 
TRAIN_CXXFLAGS = -Isrilm/include -Isrilm/include/srilm
CXXFLAGS = -I. -g $(TRAIN_CXXFLAGS) $(SRILM_CXXFLAGS)
LDFLAGS = -g -Lsrilm $(SRILM_LIBS)
GTK_CFLAGS = `pkg-config --cflags gtk+-2.0` 
GTK_LDFLAGS = `pkg-config --libs gtk+-2.0`
ifdef WIN32
GTK_CFLAGS += -mms-bitfields
endif

all: wfst-train wfst syllable-test vspell syllable-valid

clean:
	rm *.o wfst wfst-trin distance-test dictionary-test syllable-test -f

gui.o: gui.cpp config.h wfst.h dictionary.h
	$(CXX) $(CXXFLAGS) $(GTK_CFLAGS) -c $< -o $@
spell.o: spell.cpp spell.h wfst.h dictionary.h config.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
syllable.o: syllable.cpp dictionary.h config.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
wfst.o: wfst.cpp wfst.h dictionary.h config.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
dictionary.o: dictionary.cpp dictionary.h distance.h config.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
distance.o: distance.cpp distance.h config.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
sentence.o: sentence.cpp sentence.h dictionary.h config.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
sentence-test.o: sentence-test.cpp sentence.h config.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

wfst-test.o: wfst-test.cpp distance.h wfst.h dictionary.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
wfst-train.o: wfst-train.cpp distance.h wfst.h dictionary.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
distance-test.o: distance-test.cpp distance.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
dictionary-test.o: dictionary-test.cpp dictionary.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
syllable-test.o: syllable-test.cpp dictionary.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
syllable-valid.o: syllable-valid.cpp dictionary.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

wfst: wfst.o distance.o wfst-test.o dictionary.o syllable.o
	$(CXX) $^ -o $@ $(LDFLAGS) 
wfst-train: wfst-train.o distance.o wfst.o dictionary.o syllable.o
	$(CXX) -o $@ $^ $(LDFLAGS)
distance-test: distance-test.o distance.o
	$(CXX) $^ -o $@ $(LDFLAGS)
dictionary-test: dictionary-test.o dictionary.o distance.o syllable.o
	$(CXX) $^ -o $@ $(LDFLAGS)
syllable-test: syllable-test.o dictionary.o syllable.o distance.o
	$(CXX) $^ -o $@ $(LDFLAGS)
syllable-valid: syllable-valid.o dictionary.o syllable.o distance.o
	$(CXX) $^ -o $@ $(LDFLAGS)
sentence-test: sentence-test.o sentence.o dictionary.o syllable.o distance.o
	$(CXX) $^ -o $@ $(LDFLAGS)
vspell: gui.o wfst.o distance.o dictionary.o syllable.o spell.o sentence.o
	$(CXX) $^ -o $@ $(LDFLAGS) $(GTK_LDFLAGS)
