#ifndef KAUFFMAN_H
#define KAUFFMAN_H

/*
 * MEMOIZATION: why the cache key is (connectivity, remaining) not (smoothing history)
 *
 * The naive approach keys on the history of A/B choices (e.g. {0:A, 1:B, ...}).
 * Since crossings are always smoothed in the same order, every path through the
 * recursion tree is unique — the cache never hits.
 *
 * The fix is to key on what the smoothings produced, not how they got there.
 * After partially smoothing a diagram, all that matters for the remaining computation
 * is: (1) which crossings are still unprocessed, and (2) how the arc endpoints are
 * currently connected. Two different sequences of A/B choices that yield the same
 * arc connectivity with the same crossings left will produce identical results —
 * so we cache on that pair and skip the duplicate subtree entirely.
 *
 * Connectivity is stored as a canonical vector: scan arc endpoints 0..2n-1 and
 * assign component IDs 0, 1, 2... in order of first appearance. This normalises
 * away any internal labelling differences from the union-find.
 *
 * Example (3 crossings): smoothings {0:A, 1:B} and {0:B, 1:A} may leave crossing 2
 * with the same arc connectivity. The second path hits the cache and avoids
 * recomputing the entire subtree rooted there.
 */

#include "knot.h"
#include "polynomial.h"
#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <vector>

// Cache key: canonical arc connectivity + bitmask of remaining (unsmoothed) crossings.
//
// OPTIMISATION: remaining crossings are tracked as an int bitmask.
// Testing, clearing, and comparing bits is O(1); std::set would be O(log n).
struct CacheKey {
    std::vector<int> connectivity; // canonical component IDs for each arc endpoint
    int remaining;                 // bitmask: bit i set = crossing i not yet smoothed

    // OPTIMISATION: compare `remaining` first — it's an O(1) int check that
    // filters out most mismatches before the O(n) vector comparison.
    bool operator<(const CacheKey& other) const {
        if (remaining != other.remaining) return remaining < other.remaining;
        return connectivity < other.connectivity;
    }
};

class KauffmanBracket {
private:
    int call_count;
    int cache_hits;
    std::map<CacheKey, Polynomial> cache;
    bool use_cache;

    // Union-find over the 2n arc endpoints, tracking arc connectivity.
    // Path compression in uf_find gives amortised O(α(n)) ≈ O(1) per lookup,
    // which matters because apply_smoothing calls uf_unite twice per crossing
    // and canonicalize calls uf_find 2n times per cache lookup.

    // Path-compressing find: flattens the tree in-place on the way back up.
    int uf_find(std::vector<int>& parent, int x) {
        if (parent[x] != x) parent[x] = uf_find(parent, parent[x]);
        return parent[x];
    }

    // Union by root (no rank heuristic).
    void uf_unite(std::vector<int>& parent, int a, int b) {
        a = uf_find(parent, a);
        b = uf_find(parent, b);
        if (a != b) parent[a] = b;
    }

    // Apply one smoothing to the arc connectivity (in-place union-find update).
    //
    // Each crossing is visited twice around the knot circle; pos = (p, q) are
    // those two arc positions (0-indexed in [0, 2n)).  The arc arriving at
    // position p enters from before_p = p-1, and similarly for q.
    //
    // A-smoothing on a positive crossing joins:  (before_p, q) and (before_q, p)
    //   — the two strands cross over each other.
    // B-smoothing on a positive crossing joins:  (before_p, before_q) and (p, q)
    //   — the two strands run parallel.
    // For a negative crossing the roles of A and B are swapped.
    void apply_smoothing(std::vector<int>& uf, const std::pair<int,int>& pos,
                         int sign, char type, int n) {
        auto [p, q] = pos;
        int gaps = 2 * n;
        int before_p = (p - 1 + gaps) % gaps;
        int before_q = (q - 1 + gaps) % gaps;

        // cross_connect = true  →  A-smoothing on positive (or B on negative)
        bool cross_connect = (type == 'A') == (sign > 0);

        if (cross_connect) {
            uf_unite(uf, before_p, q);
            uf_unite(uf, before_q, p);
        } else {
            uf_unite(uf, before_p, before_q);
            uf_unite(uf, p, q);
        }
    }

    // Normalise the union-find into a canonical vector so structurally identical
    // connectivities compare equal regardless of internal root assignments.
    // Called twice per non-leaf node (lookup + store), so the O(2n) cost relies
    // on uf_find being near-O(1) via path compression.
    std::vector<int> canonicalize(std::vector<int> uf, int n) {
        int gaps = 2 * n;
        std::vector<int> canonical(gaps);
        std::map<int, int> id_map;
        int next_id = 0;
        for (int i = 0; i < gaps; i++) {
            int root = uf_find(uf, i);
            if (id_map.find(root) == id_map.end()) id_map[root] = next_id++;
            canonical[i] = id_map[root];
        }
        return canonical;
    }

    // δ = -A² - A⁻² is the loop value. A fully-smoothed diagram with c circles
    // has bracket δ^(c-1): the first circle contributes 1, each extra one multiplies
    // by δ. Caller passes k = circles - 1.
    Polynomial compute_delta_power(int k) {
        Polynomial result;
        result.add_term(0, 1); // δ^0 = 1
        if (k == 0) return result;

        Polynomial delta;
        delta.add_term(2, -1);   // -A²
        delta.add_term(-2, -1);  // -A⁻²

        result = delta;
        for (int i = 1; i < k; i++) result = result * delta;
        return result;
    }

    // Recursive Kauffman bracket.
    //   uf        — arc connectivity built up incrementally as crossings are smoothed
    //   remaining — bitmask of crossings not yet smoothed
    //   n         — total number of crossings
    Polynomial compute(
        std::vector<int> uf,
        int remaining,
        int n,
        const std::vector<std::pair<int,int>>& positions,
        const std::vector<int>& signs)
    {
        call_count++;

        // Base case: all crossings smoothed. Count distinct circles (union-find roots)
        // and return δ^(circles-1).
        if (remaining == 0) {
            std::set<int> roots;
            for (int i = 0; i < 2 * n; i++) roots.insert(uf_find(uf, i));
            return compute_delta_power((int)roots.size() - 1);
        }

        // MEMOIZATION: look up (canonical connectivity, remaining crossings) in cache.
        // Two different smoothing paths that reach the same state here will produce
        // the same result, so we can skip the duplicate subtree entirely.
        if (use_cache) {
            CacheKey key{canonicalize(uf, n), remaining};
            auto it = cache.find(key);
            if (it != cache.end()) {
                cache_hits++;
                return it->second;
            }
        }

        // OPTIMISATION: bitmask to pick and remove the next crossing in O(1).
        int c = 0;
        while (!(remaining & (1 << c))) c++;
        int next_remaining = remaining & ~(1 << c);

        // OPTIMISATION: incremental union-find — copy current state and apply one
        // smoothing step, rather than rebuilding from scratch. O(2n) copy but avoids
        // reprocessing all previously smoothed crossings.

        // A-smoothing (weight A)
        std::vector<int> uf_A = uf;
        apply_smoothing(uf_A, positions[c], signs[c], 'A', n);
        Polynomial smooth_A = compute(uf_A, next_remaining, n, positions, signs);

        // B-smoothing (weight A^{-1})
        std::vector<int> uf_B = uf;
        apply_smoothing(uf_B, positions[c], signs[c], 'B', n);
        Polynomial smooth_B = compute(uf_B, next_remaining, n, positions, signs);

        // Kauffman skein relation: <K> = A*<A-smooth> + A^{-1}*<B-smooth>
        // OPTIMISATION: multiply_by_A shifts exponents in O(|terms|) rather than
        // full polynomial multiplication O(|terms|²) — valid because we're multiplying
        // by a monomial.
        Polynomial result = smooth_A.multiply_by_A(1) + smooth_B.multiply_by_A(-1);

        if (use_cache) {
            cache[{canonicalize(uf, n), remaining}] = result;
        }

        return result;
    }

public:
    KauffmanBracket() : call_count(0), cache_hits(0), use_cache(true) {}

    void enable_cache(bool enable) { use_cache = enable; }

    int get_call_count() const { return call_count; }
    int get_cache_hits() const { return cache_hits; }

    Polynomial bracket(const KnotDiagram& knot) {
        call_count = 0;
        cache_hits = 0;
        cache.clear();

        int n = knot.size();

        // Unknot: bracket = 1.
        if (n == 0) {
            Polynomial r;
            r.add_term(0, 1);
            std::cout << "\nPerformance Metrics:" << std::endl;
            std::cout << "  Total calls: 1\n  Cache hits: 0\n  Cache hit rate: 0%\n";
            std::cout << "  Cache size: 0\n";
            std::cout << "  Cache: " << (use_cache ? "ENABLED" : "DISABLED") << std::endl;
            return r;
        }

        // OPTIMISATION: std::iota initialises the union-find (parent[i] = i) in one pass.
        std::vector<int> uf(2 * n);
        std::iota(uf.begin(), uf.end(), 0);

        // OPTIMISATION: (1 << n) - 1 sets all n bits in one operation — all crossings unsmoothed.
        int all_remaining = (1 << n) - 1;

        const auto& positions = knot.get_crossing_positions();
        const auto  signs     = knot.get_crossing_signs();

        Polynomial result = compute(uf, all_remaining, n, positions, signs);

        std::cout << "\nPerformance Metrics:" << std::endl;
        std::cout << "  Total calls: "    << call_count << std::endl;
        std::cout << "  Cache hits: "     << cache_hits << std::endl;
        std::cout << "  Cache hit rate: "
                  << (call_count > 0 ? (100.0 * cache_hits / call_count) : 0)
                  << "%" << std::endl;
        std::cout << "  Cache size: "     << cache.size() << std::endl;
        std::cout << "  Cache: " << (use_cache ? "ENABLED" : "DISABLED") << std::endl;

        return result;
    }

    // Jones polynomial V_K(A) = (-A)^{-3w} * <K>
    // Substitute A = t^{1/4} to get V_K(t)
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
