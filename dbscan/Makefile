CXX = g++
CXXFLAGS = -Wall -O3 -std=c++11 -march=native

# comment the following flags if you do not want to use OpenMP
#DFLAG += -DUSEOMP
#CXXFLAGS += -fopenmp

all: run-clustering

run-clustering: main.cpp datapoint.o dbscan.o
	$(CXX) $(CXXFLAGS) -o $@ $^

datapoint.o: datapoint.cpp datapoint.hpp
	$(CXX) $(CXXFLAGS) $(DFLAG) -c -o $@ $<

dbscan.o: dbscan.cpp dbscan.hpp
	$(CXX) $(CXXFLAGS) $(DFLAG) -c -o $@ $<


distpoint.o: distpoint.cpp distpoint.hpp
	$(CXX) $(CXXFLAGS) $(DFLAG) -c -o $@ $<

clean:
	rm -f run-clustering datapoint.o dbscan.o main.o
