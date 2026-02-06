#ifndef KAUFFMAN_H
#define KAUFFMAN_H

#include "knot.h"
#include "polynomial.h"

class KauffmanBracket {
private:
    int call_count;  // Track how many times we recurse
    
    // Compute bracket for a knot with given number of crossings
    Polynomial compute(int num_crossings) {
        call_count++;
        
        // Base case: no crossings left
        if (num_crossings == 0) {
            Polynomial result;
            result.add_term(0, 1);  // Just return 1
            return result;
        }
        
        // Recursive case: smooth one crossing
        // Simplified: both smoothings reduce crossings by 1
        Polynomial smooth_A = compute(num_crossings - 1);
        Polynomial smooth_B = compute(num_crossings - 1);
        
        // ⟨K⟩ = A × ⟨A-smooth⟩ + A⁻¹ × ⟨B-smooth⟩
        Polynomial result_A = smooth_A.multiply_by_A(1);   // Multiply by A
        Polynomial result_B = smooth_B.multiply_by_A(-1);  // Multiply by A⁻¹
        
        return result_A + result_B;
    }
    
public:
    KauffmanBracket() : call_count(0) {}
    
    Polynomial bracket(const KnotDiagram& knot) {
        call_count = 0;
        Polynomial result = compute(knot.size());
        std::cout << "Recursive calls: " << call_count << std::endl;
        return result;
    }
};

#endif