#include <iostream>
#include <vector>
#include "../include/knot.h"
#include "../include/kauffman.h"

int main() {
    std::cout << "=== Testing Kauffman Bracket with Component Tracking ===" << std::endl;
    
    // Test 1: Unknot (0 crossings, 1 component)
    std::cout << "\n--- Test 1: Unknot ---" << std::endl;
    KnotDiagram unknot = KnotDiagram::from_dt({});
    KauffmanBracket kb1;
    Polynomial result1 = kb1.bracket(unknot);
    std::cout << "Result: "; result1.print(); std::cout << std::endl;
    std::cout << "Expected: 1" << std::endl;
    
    // Test 2: One crossing
    std::cout << "\n--- Test 2: One Crossing ---" << std::endl;
    KnotDiagram one_cross = KnotDiagram::from_dt({2});
    KauffmanBracket kb2;
    Polynomial result2 = kb2.bracket(one_cross);
    std::cout << "Result: "; result2.print(); std::cout << std::endl;
    std::cout << "Expected: Something with higher powers than before" << std::endl;
    
    // Test 3: Trefoil
    std::cout << "\n--- Test 3: Trefoil ---" << std::endl;
    KnotDiagram trefoil = KnotDiagram::from_dt({4, 6, 2});
    KauffmanBracket kb3;
    Polynomial result3 = kb3.bracket(trefoil);
    std::cout << "Result: "; result3.print(); std::cout << std::endl;
    std::cout << "Expected: Higher powers of A (around A^7, A^5, etc)" << std::endl;
    
    return 0;
}