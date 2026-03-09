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
    int call_count;
    int cache_hits;
    std::map<DiagramState, Polynomial> cache;
    bool use_cache; 

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

    // Count circles produced by a complete smoothing assignment
    int count_components(
        const std::vector<std::pair<int,int>>& positions,
        const std::map<int, char>& smoothing,
        const std::vector<int>& signs,
        int n)
    {
        if (n == 0) return 1;

        int gaps = 2 * n;
        std::vector<int> parent(gaps);
        std::iota(parent.begin(), parent.end(), 0);

        for (int c = 0; c < n; c++) {
            auto [p, q] = positions[c];
            int before_p = (p - 1 + gaps) % gaps;
            int before_q = (q - 1 + gaps) % gaps;

            // For negative crossings, A and B smoothings are geometrically swapped
            bool cross_connect = (smoothing.at(c) == 'A') == (signs[c] > 0);

            if (cross_connect) {
                uf_unite(parent, before_p, q);
                uf_unite(parent, before_q, p);
            } else {
                uf_unite(parent, before_p, before_q);
                uf_unite(parent, p, q);
            }
        }

        std::set<int> roots;
        for (int i = 0; i < gaps; i++)
            roots.insert(uf_find(parent, i));
        return roots.size();
    }
    
    // Compute (-A^2 - A^(-2))^n
    Polynomial compute_delta_power(int n) {
        if (n == 0) {
            Polynomial result;
            result.add_term(0, 1);
            return result;
        }

        Polynomial delta;
        delta.add_term(2, -1);
        delta.add_term(-2, -1);

        Polynomial result = delta;
        for (int i = 1; i < n; i++) {
            result = result * delta;
        }

        return result;
    }

    // Base case: compute real circle count then apply delta formula
    Polynomial base_case(
        const std::vector<std::pair<int,int>>& positions,
        const std::map<int, char>& smoothing,
        const std::vector<int>& signs,
        int n)
    {
        int components = count_components(positions, smoothing, signs, n);
        return compute_delta_power(components - 1);
    }

    // Main recursive computation
    Polynomial compute(
        DiagramState state,
        int total_crossings,
        const std::vector<std::pair<int,int>>& positions,
        const std::vector<int>& signs)
    {
        call_count++;

        // CHECK CACHE ONLY IF ENABLED
        if (use_cache) {
            auto it = cache.find(state);
            if (it != cache.end()) {
                cache_hits++;
                return it->second;
            }
        }

        // Base case: all crossings smoothed
        if ((int)state.smoothing_history.size() == total_crossings) {
            return base_case(positions, state.smoothing_history, signs, total_crossings);
        }

        // Find next unsmoothed crossing
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
        Polynomial smooth_A = compute(state_A, total_crossings, positions, signs);
        Polynomial smooth_B = compute(state_B, total_crossings, positions, signs);

        // ⟨K⟩ = A × ⟨A-smooth⟩ + A^(-1) × ⟨B-smooth⟩
        Polynomial result = smooth_A.multiply_by_A(1) + smooth_B.multiply_by_A(-1);

        // STORE IN CACHE ONLY IF ENABLED
        if (use_cache) {
            cache[state] = result;
        }

        return result;
    }

public:
    KauffmanBracket() : call_count(0), cache_hits(0), use_cache(true) {}
    
    // Enable or disable caching
    void enable_cache(bool enable) {
        use_cache = enable;
    }
    
    Polynomial bracket(const KnotDiagram& knot) {
        call_count = 0;
        cache_hits = 0;
        cache.clear();

        DiagramState initial_state;
        const auto& positions = knot.get_crossing_positions();
        const auto signs = knot.get_crossing_signs();
        Polynomial result = compute(initial_state, knot.size(), positions, signs);

        std::cout << "\nPerformance Metrics:" << std::endl;
        std::cout << "  Total calls: " << call_count << std::endl;
        std::cout << "  Cache hits: " << cache_hits << std::endl;
        std::cout << "  Cache hit rate: "
                  << (call_count > 0 ? (100.0 * cache_hits / call_count) : 0)
                  << "%" << std::endl;
        std::cout << "  Unique states: " << cache.size() << std::endl;
        std::cout << "  Cache: " << (use_cache ? "ENABLED" : "DISABLED") << std::endl;

        return result;
    }

    // Compute the Jones polynomial V_K(A) = (-A)^{-3w} * <K>
    // where w is the writhe. Substitute A = t^{1/4} for V_K(t).
    Polynomial jones(const KnotDiagram& knot) {
        Polynomial raw = bracket(knot);
        int w = knot.get_writhe();

        // (-A)^{-3w} = (-1)^w * A^{-3w}
        int sign_factor = (w % 2 == 0) ? 1 : -1;
        Polynomial sign_poly;
        sign_poly.add_term(0, sign_factor);

        return raw.multiply_by_A(-3 * w) * sign_poly;
    }
};

#endif