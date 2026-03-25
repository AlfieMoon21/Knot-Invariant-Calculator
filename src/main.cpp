#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>
#include "../include/knot.h"
#include "../include/kauffman.h"

struct BenchmarkResult {
    std::string name;
    int crossings;
    long long calls_no_cache;  // recursive calls without memoization
    long long calls_cached;    // recursive calls with memoization
    long long hits_cached;     // cache hits during the cached pass
    double time_no_cache_us;   // wall-clock time without cache (microseconds)
    double time_cached_us;     // wall-clock time with cache (microseconds)
};

// Result of a single timed pass (cache enabled or disabled).
// Both polynomials are computed here so the caller can verify that the cached
// and uncached passes agree on the mathematical output.
struct PassResult {
    long long calls;        // total recursive calls made
    long long hits;         // cache hits (0 when cache is disabled)
    double time_us;         // wall-clock time in microseconds
    Polynomial bracket_poly;
    Polynomial jones_poly;
};

// Run one pass with cache on or off. Jones polynomial is derived inline so the
// bracket is only computed once (kb.jones() would recompute it internally).
PassResult run_pass(const KnotDiagram& knot, bool use_cache) {
    KauffmanBracket kb;
    kb.enable_cache(use_cache);

    auto start = std::chrono::high_resolution_clock::now();
    Polynomial br = kb.bracket(knot);
    auto end   = std::chrono::high_resolution_clock::now();
    double us  = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    // V_K(A) = (-A)^{-3w} * <K>
    int w = knot.get_writhe();
    int sign_factor = (w % 2 == 0) ? 1 : -1;
    Polynomial sign_poly;
    sign_poly.add_term(0, sign_factor);
    Polynomial jo = br.multiply_by_A(-3 * w) * sign_poly;

    return { kb.get_call_count(), kb.get_cache_hits(), us, br, jo };
}

// Run both passes (no-cache then cached) for one knot and return the results.
BenchmarkResult benchmark(const std::string& name, const std::vector<int>& dt) {
    KnotDiagram knot = KnotDiagram::from_dt(dt);
    int n = knot.size();

    std::cout << "\n========================================\n";
    std::cout << "Knot: " << name << "  (" << n << " crossings)\n";
    std::cout << "Writhe: " << knot.get_writhe() << "\n";
    std::cout << "========================================\n";

    PassResult no_cache = run_pass(knot, false);
    PassResult cached   = run_pass(knot, true);

    // Print polynomials from the cached pass (results must be identical).
    std::cout << "  Bracket <K>: "; cached.bracket_poly.print(); std::cout << "\n";
    std::cout << "  Jones  V(A): "; cached.jones_poly.print();   std::cout << "\n";

    std::cout << "\n  Without cache: " << no_cache.calls << " calls"
              << "  |  " << (long long)no_cache.time_us << " μs\n";
    std::cout << "  With    cache: " << cached.calls << " calls"
              << "  (" << cached.hits << " hits, "
              << std::fixed << std::setprecision(1)
              << (cached.calls > 0 ? 100.0 * cached.hits / cached.calls : 0.0)
              << "% hit rate)"
              << "  |  " << (long long)cached.time_us << " μs\n";

    double call_reduction = (no_cache.calls > 0)
        ? 100.0 * (no_cache.calls - cached.calls) / no_cache.calls : 0.0;
    double speedup = (cached.time_us > 0) ? no_cache.time_us / cached.time_us : 0.0;
    std::cout << "  Call reduction: " << std::setprecision(1) << call_reduction << "%"
              << "  |  Time speedup: " << std::setprecision(2) << speedup << "x\n";

    return { name, n, no_cache.calls, cached.calls, cached.hits,
             no_cache.time_us, cached.time_us };
}

// Summary table: crossing count, calls with/without cache, hit rate, speedup.
void print_summary(const std::vector<BenchmarkResult>& results) {
    std::cout << "\n\n";
    std::cout << "==================================================================="
                 "======================\n";
    std::cout << "  SUMMARY TABLE\n";
    std::cout << "==================================================================="
                 "======================\n";

    // Column header: "$" is shorthand for "cache" (explained in the legend below)
    std::cout << std::left
              << std::setw(18) << "Knot"
              << std::right
              << std::setw(4)  << "n"
              << std::setw(12) << "Calls (no$)"
              << std::setw(12) << "Calls ($)"
              << std::setw(10) << "Hit rate"
              << std::setw(10) << "Call red."
              << std::setw(12) << "Time (no$)"
              << std::setw(10) << "Time ($)"
              << std::setw(9)  << "Speedup"
              << "\n";
    std::cout << std::string(97, '-') << "\n";

    for (const auto& r : results) {
        double hit_rate = (r.calls_cached > 0)
            ? 100.0 * r.hits_cached / r.calls_cached : 0.0;
        double call_red = (r.calls_no_cache > 0)
            ? 100.0 * (r.calls_no_cache - r.calls_cached) / r.calls_no_cache : 0.0;
        double speedup  = (r.time_cached_us > 0)
            ? r.time_no_cache_us / r.time_cached_us : 0.0;

        std::cout << std::left  << std::setw(18) << r.name
                  << std::right << std::setw(4)  << r.crossings
                  << std::setw(12) << r.calls_no_cache
                  << std::setw(12) << r.calls_cached
                  << std::setw(9)  << std::fixed << std::setprecision(1)
                                   << hit_rate << "%"
                  << std::setw(9)  << call_red  << "%"
                  << std::setw(10) << (long long)r.time_no_cache_us
                  << std::setw(8)  << (long long)r.time_cached_us
                  << std::setw(8)  << std::setprecision(2) << speedup << "x"
                  << "\n";
    }
    std::cout << std::string(97, '-') << "\n";
    std::cout << "  $ = cache   |   times in microseconds   |   speedup = time_no_cache / time_cached\n";
}

int main() {
    std::cout << "=== Kauffman Bracket: Cache Impact Analysis ===\n";

    // T(2,n) torus knots: DT = {4, 6, 8, ..., 2n, 2}, all positive crossings.
    // Scaling tests: 2^n states without cache, measurably fewer with cache.
    //
    // Other knots included to show cache behaviour on varied topology:
    //   Figure-8 (4_1): amphicheiral, alternating, mixed crossing signs
    std::vector<std::pair<std::string, std::vector<int>>> test_cases = {
        {"Unknot",          {}},
        {"T(2,3) trefoil",  {4, 6, 2}},
        {"Figure-8 4_1",    {6, -8, 2, -4}},
        {"T(2,5)",          {4, 6, 8, 10, 2}},
        {"T(2,7)",          {4, 6, 8, 10, 12, 14, 2}},
        {"T(2,9)",          {4, 6, 8, 10, 12, 14, 16, 18, 2}},
        {"T(2,11)",         {4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 2}},
        {"T(2,13)",         {4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 2}},
        {"T(2,15)",         {4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 2}},
        {"T(2,17)",         {4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 2}},
        {"T(2,19)",         {4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 2}},
        {"T(2,21)",         {4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 2}},
    };

    std::vector<BenchmarkResult> results;
    for (const auto& [name, dt] : test_cases) {
        results.push_back(benchmark(name, dt));
    }

    print_summary(results);
    return 0;
}
