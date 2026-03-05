#ifndef KAUFFMAN_H
#define KAUFFMAN_H

#include "diagram_state.h"
#include "knot.h"
#include "polynomial.h"
#include <iostream>
#include <numeric>
#include <ostream>
#include <set>
#include <vector>

class KauffmanBracket {
private:
    int call_count;  // Track how many times we recurse
    int cache_hits;
    std::map<DiagramState, Polynomial> cache;
    bool debug;

    // Union-Find: find root with path compression
    int uf_find(std::vector<int>& parent, int x) {
        if (parent[x] != x)
            parent[x] = uf_find(parent, parent[x]);
        return parent[x];
    }

    void uf_unite(std::vector<int>& parent, int a, int b) {
        a = uf_find(parent, a);
        b = uf_find(parent, b);
        if (a != b) parent[a] = b;
    }

    // Count circles produced by a complete smoothing assignment.
    // There are 2n gaps (gap i sits between position i-1 and position i).
    // A-smooth at crossing (p,q): merges gap(p-1) with gap(q), and gap(q-1) with gap(p)
    // B-smooth at crossing (p,q): merges gap(p-1) with gap(p), and gap(q-1) with gap(q)
    int count_components(
        const std::vector<std::pair<int,int>>& positions,
        const std::map<int, char>& smoothing,
        int n)
    {
        if (n == 0) return 1;  // bare unknot = 1 circle

        int gaps = 2 * n;
        std::vector<int> parent(gaps);
        std::iota(parent.begin(), parent.end(), 0);

        for (int c = 0; c < n; c++) {
            auto [p, q] = positions[c];
            int before_p = (p - 1 + gaps) % gaps;
            int before_q = (q - 1 + gaps) % gaps;

            if (smoothing.at(c) == 'A') {
                uf_unite(parent, before_p, q);
                uf_unite(parent, before_q, p);
            } else {
                uf_unite(parent, before_p, p);
                uf_unite(parent, before_q, q);
            }
        }

        std::set<int> roots;
        for (int i = 0; i < gaps; i++)
            roots.insert(uf_find(parent, i));
        return roots.size();
    }
    
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


    // Base case (no crossings left): compute real circle count then apply delta formula
    Polynomial base_case(
        const std::vector<std::pair<int,int>>& positions,
        const std::map<int, char>& smoothing,
        int n)
    {
        int components = count_components(positions, smoothing, n);
        return compute_delta_power(components - 1);
    }

    //main recursive computation
    Polynomial compute(
        DiagramState state,
        int total_crossings,
        const std::vector<std::pair<int,int>>& positions)
    {
        call_count++;

        //check cache
        auto it = cache.find(state);
        if (it != cache.end()) {
            cache_hits++;
            return it->second;
        }

        //base case: all crossings smoothed — count real components
        if ((int)state.smoothing_history.size() == total_crossings) {
            return base_case(positions, state.smoothing_history, total_crossings);
        }

        //find next unsmoothed crossing
        int next_crossing = -1;
        for (int i = 0; i < total_crossings; i++) {
            if (state.smoothing_history.find(i) == state.smoothing_history.end()) {
                next_crossing = i;
                break;
            }
        }

        DiagramState state_A = state;
        state_A.smoothing_history[next_crossing] = 'A';

        DiagramState state_B = state;
        state_B.smoothing_history[next_crossing] = 'B';

        // Recursive calls
        Polynomial smooth_A = compute(state_A, total_crossings, positions);
        Polynomial smooth_B = compute(state_B, total_crossings, positions);

        // ⟨K⟩ = A × ⟨A-smooth⟩ + A^(-1) × ⟨B-smooth⟩
        Polynomial result = smooth_A.multiply_by_A(1) + smooth_B.multiply_by_A(-1);

        cache[state] = result;
        return result;
    }

public:
    KauffmanBracket(bool enable_debug = false)
         : call_count(0), cache_hits(0) {}  // INITIALIZE
    
    Polynomial bracket(const KnotDiagram& knot) {
        call_count = 0;
        cache_hits = 0;
        cache.clear();

        DiagramState initial_state;
        const auto& positions = knot.get_crossing_positions();
        Polynomial result = compute(initial_state, knot.size(), positions);
        
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