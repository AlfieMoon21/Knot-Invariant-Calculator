#include <iostream>
#include <vector>
#include <chrono>
#include "../include/knot.h"
#include "../include/kauffman.h"

void benchmark(const std::string& name, const std::vector<int>& dt, bool with_cache) {
    std::cout << "\n=== " << name << " ===" << std::endl;
    std::cout << "Cache: " << (with_cache ? "ENABLED" : "DISABLED") << std::endl;

    KnotDiagram knot = KnotDiagram::from_dt(dt);
    KauffmanBracket kb;
    kb.enable_cache(with_cache);

    auto start = std::chrono::high_resolution_clock::now();
    Polynomial bracket = kb.bracket(knot);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "Bracket <K>: "; bracket.print(); std::cout << std::endl;
    std::cout << "Writhe: " << knot.get_writhe() << std::endl;

    // Jones polynomial: reuse cached bracket result via a fresh jones() call
    KauffmanBracket kb2;
    kb2.enable_cache(with_cache);
    Polynomial jones = kb2.jones(knot);
    std::cout << "Jones V(A): "; jones.print(); std::cout << std::endl;

    std::cout << "Computation time: " << duration.count() << " μs" << std::endl;
}

int main() {
    std::cout << "=== Kauffman Bracket: Cache Impact Analysis ===" << std::endl;
    
    std::vector<std::pair<std::string, std::vector<int>>> test_cases = {
        {"Unknot", {}},
        {"1 crossing", {2}},
        {"Trefoil (3 crossings)", {4, 6, 2}},
        {"Figure-8 (4 crossings)", {6, -8, 2, -4}},  // 4_1: writhe=0, expected bracket: -A^8+A^4+1+A^-4-A^-8
        {"5 crossings", {4, 6, 8, 10, 2}}
    };
    
    for (const auto& [name, dt] : test_cases) {
        std::cout << "\n========================================" << std::endl;
        std::cout << "Testing: " << name << std::endl;
        std::cout << "========================================" << std::endl;
        
        // WITHOUT cache
        benchmark(name + " (NO CACHE)", dt, false);
        
        // WITH cache
        benchmark(name + " (WITH CACHE)", dt, true);
    }
    
    return 0;
}