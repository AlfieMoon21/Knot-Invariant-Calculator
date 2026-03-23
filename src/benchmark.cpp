// NOT COMPILED BY THE MAKEFILE.
//
// Early draft of the benchmark: single pass only, no cache-off comparison, and
// the 12-crossing test case is a placeholder. The full benchmark (side-by-side
// cache comparison, hit rates, summary table) is in src/main.cpp.

#include <iostream>
#include <vector>
#include <chrono>
#include "../include/knot.h"
#include "../include/kauffman.h"

void benchmark_knot(const std::string& name, const std::vector<int>& dt, bool with_cache) {
    KnotDiagram knot = KnotDiagram::from_dt(dt);
    KauffmanBracket kb;
    
    auto start = std::chrono::high_resolution_clock::now();
    Polynomial result = kb.bracket(knot);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << name << " (" << knot.size() << " crossings)" << std::endl;
    std::cout << "  Cache: " << (with_cache ? "ON" : "OFF") << std::endl;
    std::cout << "  Time: " << duration.count() << " μs" << std::endl;
}

int main() {
    std::cout << "=== Benchmark: Caching Impact ===" << std::endl;
    
    benchmark_knot("Simple (6 crossings)", {4,6,8,10,12,2}, true);
    benchmark_knot("Medium (12 crossings)", {/* 12-crossing DT */}, true);
    
    return 0;
}