CXX = g++
CXXFLAGS = -Wall -O3 -std=c++11 -march=native `pkg-config --cflags opencv`
OPENCVFLAGS = `pkg-config --libs opencv`

all: run-clustering

run-clustering: main.cpp datapoint.o optics_datapoint.o dataheap.o optics.o minioptics.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(OPENCVFLAGS)

datapoint.o: datapoint/datapoint.cpp datapoint/datapoint.h
	$(CXX) $(CXXFLAGS) $(DFLAG) -c -o $@ $< 

optics_datapoint.o: datapoint/optics_datapoint.cpp datapoint/optics_datapoint.h
	$(CXX) $(CXXFLAGS) $(DFLAG) -c -o $@ $<

dataheap.o: heap/dataheap.cpp heap/dataheap.h
	$(CXX) $(CXXFLAGS) $(DFLAG) -c -o $@ $<

optics.o: algorithm/optics.cpp algorithm/optics.h
	$(CXX) $(CXXFLAGS) $(DFLAG) -c -o $@ $<

minioptics.o: algorithm/minioptics.cpp algorithm/minioptics.h
	$(CXX) $(CXXFLAGS) $(DFLAG) -c -o $@ $<

clean:
	rm -f run-clustering minioptics.o optics.o dataheap.o optics_datapoint.o datapoint.o main.o
