TRAIN_CXXFLAGS = -DUSE_EXACT_MATCH -DTRAINING
CXXFLAGS = -I. -g $(TRAIN_CXXFLAGS) -pg
LDFLAGS = -g -pg

all: wfst-train

clean:
	rm *.o wfst wfst-trin distance-test dictionary-test syllable-test -f

syllable.o: syllable.cpp dictionary.h config.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
wfst.o: wfst.cpp wfst.h dictionary.h config.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
dictionary.o: dictionary.cpp dictionary.h distance.h config.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
distance.o: distance.cpp distance.h config.h
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

wfst: wfst.o distance.o wfst-test.o dictionary.o syllable.o
	$(CXX) $(LDFLAGS) $^ -o $@
wfst-train: wfst-train.o distance.o wfst.o dictionary.o syllable.o
	$(CXX) $(LDFLAGS) $^ -o $@
distance-test: distance-test.o distance.o
	$(CXX) $(LDFLAGS) $^ -o $@
dictionary-test: dictionary-test.o dictionary.o distance.o syllable.o
	$(CXX) $(LDFLAGS) $^ -o $@
syllable-test: syllable-test.o dictionary.o syllable.o distance.o
	$(CXX) $(LDFLAGS) $^ -o $@
