#ifndef KAUFFMAN_H
#define KAUFFMAN_H

#include "diagram_state.h"
#include "knot.h"
#include "polynomial.h"
#include <iostream>
#include <ostream>

class KauffmanBracket {
private:
    int call_count;  // Track how many times we recurse
    
    //compute (-A^2 - A^(-2))^n
    Polynomial compute_delta_power(int n) {
        if (n == 0) {
            Polynomial result;
            result.add_term(0, 1); // return 1
            return  result;
        }

        //create delta = (-A^2 - A^(-2))
        Polynomial delta;
        delta.add_term(2, -1); //-A^2
        delta.add_term(-2, -1); // -A^(-2)

        //compute delta^n by repeated multiplication
        Polynomial result = delta;
        for (int i = 1; i < n; i++) {
            result = result * delta;
        }

        return result;
    }


    //Base case (no crossings left)
    Polynomial base_case(int num_components) {
        //n circles = (-A^2 - A^(-2))^(n-1)
        return compute_delta_power(num_components -1);
    }

    //main recursive computation
    Polynomial compute(DiagramState state) {
        call_count++;

        std::cout << "Computing " << state << std::endl; //debug output

        //Base case: no crossings left
        if (state.num_crossings == 0) {
            Polynomial result = base_case(state.num_components);
            std::cout << " Base case: "; result.print(); std::cout << std::endl;
            return result;
        }

        //recursice case: smooth one crossing
        //SIMPLIFIED: Assume A-smoothing adds a component, B-smoothing doesnt
        DiagramState state_A(state.num_crossings -1, state.num_components + 1);
        DiagramState state_B(state.num_crossings - 1, state.num_components);

        Polynomial smooth_A = compute(state_A);
        Polynomial smooth_B = compute(state_B);

        // ⟨K⟩ = A × ⟨A-smooth⟩ + A^(-1) × ⟨B-smooth⟩
        Polynomial result_A = smooth_A.multiply_by_A(1);
        Polynomial result_B = smooth_B.multiply_by_A(-1);

        return result_A + result_B;
    }

public:
    KauffmanBracket() : call_count(0) {}

    Polynomial bracket(const KnotDiagram& knot) {
        call_count = 0;

        //start with all crossings, 1 component
        DiagramState initial_state(knot.size(), 1);

        Polynomial result = compute(initial_state);

        std::cout << "\nTotal recursive calls: " << call_count << std::endl;
        return  result;
    }
};

#endif