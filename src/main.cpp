#include <iostream>
#include "../include/union_find.h"

int main() {
    std::cout << "=== Testing Union-Find ===" << std::endl;
    
    // Test 1: Start with 6 separate elements
    std::cout << "\\nTest 1: Initial State" << std::endl;
    UnionFind uf(6);
    std::cout << "Components: " << uf.count_components() << std::endl;
    std::cout << "Expected: 6" << std::endl;
    
    // Test 2: Merge some elements
    std::cout << "\\nTest 2: After union(0, 1)" << std::endl;
    uf.union_sets(0, 1);
    std::cout << "Components: " << uf.count_components() << std::endl;
    std::cout << "Expected: 5 (sets: {0,1}, {2}, {3}, {4}, {5})" << std::endl;
    
    // Test 3: Merge more
    std::cout << "\\nTest 3: After union(2, 3)" << std::endl;
    uf.union_sets(2, 3);
    std::cout << "Components: " << uf.count_components() << std::endl;
    std::cout << "Expected: 4 (sets: {0,1}, {2,3}, {4}, {5})" << std::endl;
    
    // Test 4: Merge across sets
    std::cout << "\\nTest 4: After union(1, 3) - joins two existing sets!" << std::endl;
    uf.union_sets(1, 3);
    std::cout << "Components: " << uf.count_components() << std::endl;
    std::cout << "Expected: 3 (sets: {0,1,2,3}, {4}, {5})" << std::endl;
    
    // Test 5: Verify find works
    std::cout << "\\nTest 5: Verify find()" << std::endl;
    std::cout << "find(0) == find(3)? " << (uf.find(0) == uf.find(3) ? "Yes" : "No") << std::endl;
    std::cout << "Expected: Yes (same set)" << std::endl;
    std::cout << "find(4) == find(5)? " << (uf.find(4) == uf.find(5) ? "Yes" : "No") << std::endl;
    std::cout << "Expected: No (different sets)" << std::endl;
    
    return 0;
}