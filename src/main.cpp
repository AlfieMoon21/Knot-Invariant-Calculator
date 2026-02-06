#include <iostream>
#include <vector>
#include "../include/knot.h"
#include "../include/kauffman.h"

int main() {
    std::cout << "=== Testing Kauffman Bracket (Simplified) ===" << std::endl;
    
    // Test 1: Unknot (0 crossings)
    std::cout << "\nTest 1: Unknot (0 crossings)" << std::endl;
    KnotDiagram unknot = KnotDiagram::from_dt({});
    KauffmanBracket kb1;
    Polynomial result1 = kb1.bracket(unknot);
    std::cout << "Result: "; result1.print(); std::cout << std::endl;
    std::cout << "Expected: 1" << std::endl;
    
    // Test 2: 1 crossing
    std::cout << "\nTest 2: 1 crossing" << std::endl;
    KnotDiagram one_cross = KnotDiagram::from_dt({2});
    KauffmanBracket kb2;
    Polynomial result2 = kb2.bracket(one_cross);
    std::cout << "Result: "; result2.print(); std::cout << std::endl;
    std::cout << "Expected: A + A^-1" << std::endl;
    
    // Test 3: Trefoil (3 crossings)
    std::cout << "\nTest 3: Trefoil (3 crossings)" << std::endl;
    KnotDiagram trefoil = KnotDiagram::from_dt({4, 6, 2});
    KauffmanBracket kb3;
    Polynomial result3 = kb3.bracket(trefoil);
    std::cout << "Result: "; result3.print(); std::cout << std::endl;
    
    return 0;
}