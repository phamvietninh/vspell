all: wfst

syllable.o: syllable.cpp dictionary.h config.h
	$(CXX) -c $< -I. -g -o $@
wfst.o: wfst.cpp wfst.h dictionary.h config.h
	$(CXX) -c $< -I. -g -o $@
dictionary.o: dictionary.cpp dictionary.h distance.h config.h
	$(CXX) -c $< -I. -g -o $@
distance.o: distance.cpp distance.h config.h
	$(CXX) -c $< -I. -g -o $@

wfst-test.o: wfst-test.cpp distance.h wfst.h dictionary.h
	$(CXX) -c $< -I. -g -o $@
distance-test.o: distance-test.cpp distance.h
	$(CXX) -c $< -I. -g -o $@
dictionary-test.o: dictionary-test.cpp dictionary.h
	$(CXX) -c $< -I. -g -o $@

wfst: wfst.o distance.o wfst-test.o dictionary.o syllable.o
	$(CXX) $^ -o $@
distance-test: distance-test.o distance.o
	$(CXX) $^ -o $@
dictionary-test: dictionary-test.o dictionary.o distance.o syllable.o
	$(CXX) $^ -o $@
