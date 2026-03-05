#include <iostream>
#include <vector>
#include <chrono>
#include "../include/knot.h"
#include "../include/kauffman.h"

int main() {
    std::cout << "=== Kauffman Bracket - Performance Analysis ===" << std::endl;
    
    // Test different sizes and measure performance
    std::vector<std::pair<std::string, std::vector<int>>> test_cases = {
        {"Unknot (0 crossings)", {}},
        {"1 crossing", {2}},
        {"Trefoil (3 crossings)", {4, 6, 2}},
        {"Figure-8 (4 crossings)", {4, 6, 8, 2}},
        {"5 crossings", {4, 6, 8, 10, 2}}
    };
    
    for (const auto& [name, dt] : test_cases) {
        std::cout << "\\n=== " << name << " ===" << std::endl;
        
        KnotDiagram knot = KnotDiagram::from_dt(dt);
        KauffmanBracket kb;
        
        // Time the computation
        auto start = std::chrono::high_resolution_clock::now();
        Polynomial result = kb.bracket(knot);
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "Result: "; result.print(); std::cout << std::endl;
        std::cout << "Computation time: " << duration.count() << " μs" << std::endl;
    }
    
    return 0;
}