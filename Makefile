CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2

all: knot_calc

knot_calc: src/main.cpp
	$(CXX) $(CXXFLAGS) -o knot_calc src/main.cpp

clean:
	rm -f knot_calc

run: knot_calc
	./knot_calc