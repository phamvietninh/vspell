all: wfst
wfst.o: wfst.cpp wfst.h distance.h
	$(CXX) -c $< -I. -g -o $@
distance.o: distance.cpp distance.h
	$(CXX) -c $< -I. -g -o $@
wfst: wfst.o distance.o wfst-test.o
	$(CXX) $^ -o $@
wfst-test.o: wfst-test.cpp distance.h
	$(CXX) -c $< -I. -g -o $@
distance-test.o: distance-test.cpp distance.h
	$(CXX) -c $< -I. -g -o $@
distance-test: distance-test.o distance.o
	$(CXX) $^ -o $@
