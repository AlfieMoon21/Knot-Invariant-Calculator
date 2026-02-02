#include <iostream>
#include <vector>
#include "../include/knot.h"

int main() {
    std::cout << "=== Testing DT Notation Parser ===" << std::endl;
    
    // Test 1: Trefoil [4, 6, 2]
    std::cout << "\nTest 1: Trefoil [4, 6, 2]" << std::endl;
    std::vector<int> trefoil_dt = {4, 6, 2};
    KnotDiagram trefoil = KnotDiagram::from_dt(trefoil_dt);
    trefoil.print();
    
    // Test 2: Figure-eight [4, 6, 8, 2]
    std::cout << "\nTest 2: Figure-eight [4, 6, 8, 2]" << std::endl;
    std::vector<int> figure8_dt = {4, 6, 8, 2};
    KnotDiagram figure8 = KnotDiagram::from_dt(figure8_dt);
    figure8.print();
    
    // Test 3: Mixed signs [4, -6, 2]
    std::cout << "\nTest 3: Mixed signs [4, -6, 2]" << std::endl;
    std::vector<int> mixed_dt = {4, -6, 2};
    KnotDiagram mixed = KnotDiagram::from_dt(mixed_dt);
    mixed.print();
    
    std::cout << "\n=== All Tests Complete ===" << std::endl;
    
    return 0;
}