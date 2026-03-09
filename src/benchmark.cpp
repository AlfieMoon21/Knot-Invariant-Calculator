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
    
    // Test cases matching paper's complexity
    benchmark_knot("Simple (6 crossings)", {4,6,8,10,12,2}, true);
    benchmark_knot("Medium (12 crossings)", {/* 12-crossing DT */}, true);
    
    return 0;
}