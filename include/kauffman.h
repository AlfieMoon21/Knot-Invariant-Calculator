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
    int cache_hits;
    std::map<DiagramState, Polynomial> cache;
    bool debug;
    
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


    // Base case (no crossings left)
    Polynomial base_case(int num_components) {
        Polynomial result = compute_delta_power(num_components - 1);
        // std::cout << "  → Base case (" << num_components << " components): ";
        // result.print();
        // std::cout << std::endl;
        // std::cout << "  → Returning: ";  
        // result.print();                   
        // std::cout << std::endl;           
        return result;
}

    //main recursive computation
    Polynomial compute(DiagramState state, int total_crossings) {
        call_count++;

        //check cache 
        auto it = cache.find(state);
        if (it != cache.end()) {
            cache_hits++;
            if (debug) {
                // std::cout << " [CACHE HIT] " << state << std::endl;
            }
            return it->second; //return cached result
        }

        if (debug) {
            // std::cout << "Computing " << state << std::endl;
        }

        //BAse case: have all corssings been smoothed 
        if (state.smoothing_history.size() == total_crossings) {
            return base_case(state.num_components);
        }

        //find next crossing to smooth 
        int next_crossing = -1;
        for (int i = 0; i < total_crossings; i++) {
            if (state.smoothing_history.find(i) == state.smoothing_history.end()) {
                next_crossing = i;
                break;
            }
        }

        //create state after a smoothing 
        DiagramState state_A = state;
        state_A.smoothing_history[next_crossing] = 'A';

        //update components {heuristics: even crossing = A adds components}
        if (next_crossing % 2 == 0) {
            state_A.num_components++;
        }

        //create state after B-smoothing
        DiagramState state_B = state;
        state_B.smoothing_history[next_crossing] = 'B';

        if (next_crossing % 2 == 1) {
            state_B.num_components++;
        }

        // Recursive calls
        Polynomial smooth_A = compute(state_A, total_crossings);
        Polynomial smooth_B = compute(state_B, total_crossings);

        // Apply the formula: ⟨K⟩ = A × ⟨A-smooth⟩ + A^(-1) × ⟨B-smooth⟩
        Polynomial result_A = smooth_A.multiply_by_A(1);
        Polynomial result_B = smooth_B.multiply_by_A(-1);

        Polynomial result = result_A + result_B;

        //store in cache 
        cache[state] = result;

        // NEW DEBUG OUTPUT
        //std::cout << "  → Returning: ";
        //result.print();
        //std::cout << std::endl;
    
        return result;
    }

public:
    KauffmanBracket(bool enable_debug = false)
         : call_count(0), cache_hits(0) {}  // INITIALIZE
    
    Polynomial bracket(const KnotDiagram& knot) {
        call_count = 0;
        cache_hits = 0;  // RESET
        cache.clear();
        
        DiagramState initial_state(1);
        Polynomial result = compute(initial_state, knot.size());
        
        std::cout << "\\nPerformance Metrics:" << std::endl;
        std::cout << "  Total calls: " << call_count << std::endl;
        std::cout << "  Cache hits: " << cache_hits << std::endl;
        std::cout << "  Cache hit rate: " 
                  << (call_count > 0 ? (100.0 * cache_hits / call_count) : 0) 
                  << "%" << std::endl;
        std::cout << "  Unique states: " << cache.size() << std::endl;
        
        return result;
    }
};

#endif