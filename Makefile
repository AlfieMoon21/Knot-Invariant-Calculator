CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2

all: knot_calc

knot_calc: src/main.cpp
	$(CXX) $(CXXFLAGS) -o knot_calc src/main.cpp

knot_calc_debug: src/main.cpp
	$(CXX) -std=c++17 -Wall -g -O0 -o knot_calc_debug src/main.cpp

clean:
	rm -f knot_calc knot_calc_debug callgrind.out.*

run: knot_calc
	./knot_calc

profile: knot_calc_debug
	valgrind --tool=callgrind --callgrind-out-file=callgrind.out ./knot_calc_debug
	callgrind_annotate --auto=yes callgrind.out > callgrind_report.txt
	@echo "Report written to callgrind_report.txt"