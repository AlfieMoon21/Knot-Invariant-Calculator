#include <iostream>
#include "../include/knot.h"

int main() {
    std::cout << "=== Testing Knot Structure ===" << std::endl;
    
    // Create a trefoil knot
    // DT notation: [4, 6, 2]
    // This means 3 crossings, all positive
    KnotDiagram trefoil;
    trefoil.add_crossing(0, 1);  // Crossing 1, positive
    trefoil.add_crossing(1, 1);  // Crossing 2, positive
    trefoil.add_crossing(2, 1);  // Crossing 3, positive
    
    std::cout << "\nTrefoil knot:" << std::endl;
    trefoil.print();
    
    std::cout << "\nNumber of crossings: " << trefoil.size() << std::endl;
    std::cout << "Is empty? " << (trefoil.is_empty() ? "Yes" : "No") << std::endl;
    
    return 0;
}