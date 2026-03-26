# Development Notes

Implementation details, optimization analysis, code walkthrough, and development log.

---

## Implementation

### Data structures

| Structure | Storage | Purpose |
|-----------|---------|---------|
| `Polynomial` | `std::map<int,int>` power to coeff | Sparse Laurent polynomial; zero terms auto-erased |
| Union-find | `std::vector<int>` parent array | Arc connectivity; path compression for O(α(n)) find |
| `CacheKey` | `vector<int>` + `int` | Canonical connectivity + remaining-crossings bitmask |
| Cache | `std::map<CacheKey, Polynomial>` | Memoization store |

### Build

```bash
g++ -std=c++17 -O2 -o knot_calc src/main.cpp
```

All headers are in `include/`. The Makefile also provides `make run` and `make clean`.

### Timing

`std::chrono::high_resolution_clock` measures bracket computation alone, separately from Jones normalisation. Cached and uncached runs each use a fresh `KauffmanBracket` instance so neither warms the other's state.

---

## Optimization Strategies

### 1. Topology-based memoization

**Problem:** Naive recursion processes crossings in order 0, 1, 2, ... forming a perfect binary tree. Every path is unique, so a history-based cache gets zero hits.

**Solution:** Cache key = (canonical arc connectivity, remaining-crossings bitmask). Two different A/B choice sequences that reach the same connectivity with the same crossings remaining share a cached result.

**Impact:** 32.5% call reduction for T(2,21). See the cache performance table in [`ALGORITHM.md`](ALGORITHM.md).

**Wall-clock result:** For torus knots, cache overhead (canonicalization + `std::map` lookups) exceeds the savings because hit rates are only 20–24%. The algorithm is correct but the implementation overhead dominates for this knot family.

### 2. Sparse polynomial storage

`std::map<int,int>` stores only non-zero terms. `add_term` erases a term when its coefficient reaches zero. T(2,15)'s bracket has only 10 non-zero terms spanning powers −19 to +17.

`multiply_by_A(k)` shifts all map keys by k in O(terms), much faster than full polynomial multiplication.

### 3. Incremental union-find

Rather than recomputing arc connectivity from scratch, the union-find state is built incrementally. Each recursion level copies its parent's state and applies one more smoothing, avoiding reprocessing all previous crossings.

```cpp
std::vector<int> uf_A = uf;                            // copy parent state
apply_smoothing(uf_A, positions[c], signs[c], 'A', n); // apply one step
Polynomial smooth_A = compute(uf_A, ...);
```

### 4. Bitmask for remaining crossings

The set of unsmoothed crossings is a single `int` bitmask: crossing i is unsmoothed iff bit i is set.

```cpp
int remaining = (1 << n) - 1;  // all n bits set

int c = 0;
while (!(remaining & (1 << c))) c++;         // find lowest unsmoothed
int next_remaining = remaining & ~(1 << c); // clear bit c
```

Bitmask operations are O(1). Safe limit: 30 crossings with 32-bit signed int (shifting into bit 31 is undefined behaviour in C++; use `unsigned int` or `int64_t` to extend).

### Profiling results (Valgrind callgrind)

Profiled with `valgrind --tool=callgrind` on the debug binary (`-g -O0`) up to T(2,13).
Full report in `callgrind_report.txt`. Top hotspots by instruction count:

| % of instructions | Function | Where |
|---|---|---|
| 8.98% | `vector::operator[]` | UF path traversal in `uf_find` |
| 6.21% | `vector::size()` | Called throughout `compute` and `canonicalize` |
| **5.43%** | **`lexicographical_compare`** | **Cache key vector comparison on every lookup** |
| **~20%+** | **`_Rb_tree` / `std::map` internals** | **`id_map` inside `canonicalize()`, allocated and destroyed per call** |
| 4.80% | `uf_find` (two inlined variants) | Path compression |
| 1.15% | `malloc` / `free` | Heap churn from temporary `id_map` per call |

**Key finding:** the `std::map<int,int> id_map` inside `canonicalize()` is created and
destroyed on every recursive call when the cache is enabled. The red-black tree
operations (insert, find, lower_bound, destroy) account for over 20% of all instructions.
On top of that, the cache key comparison (`vector<int>` lexicographic compare) adds
another ~5.4%. Together these account for roughly 25–30% of all work — directly
explaining why the cached version is 2–3× slower despite making 32.5% fewer recursive calls.

The fix is clear from the data: replacing `std::map<int,int> id_map` with a plain
array (possible since arc IDs are bounded by `2n`) eliminates all tree allocation overhead,
and replacing the outer `std::map<CacheKey, Polynomial>` with an `unordered_map` with a
custom hash removes the O(n) key comparison on every lookup.

### Possible future optimisations

- **`std::unordered_map` with custom hash**: replaces O(log n) tree traversal with O(1) average lookup, directly addressing the ~20% instruction overhead identified by callgrind
- **Array-based canonicalization**: replace `std::map<int,int> id_map` in `canonicalize()` with a `vector<int>` of size `2n` (IDs are bounded), eliminating heap allocation per call
- **Lazy canonicalization**: canonicalize only when storing to cache, not on every lookup check
- **More cache-friendly knot families**: torus knots are too regular; knot families with denser crossing interactions may produce more coincident subproblems, though hit rates are also sensitive to DT encoding — the same knot with a different labeling can go from 14.7% hits to 0%

---

## Code Walkthrough

### `crossing.h`

```cpp
struct Crossing { int index; int sign; };
```

A crossing is its position (0-indexed) and sign (+1 or −1, derived from DT sign). The sign is used in two places: writhe calculation and smoothing direction.

### `polynomial.h`

Sparse Laurent polynomial over `std::map<int,int>`. See [`ALGORITHM.md`](ALGORITHM.md#laurent-polynomials) for the maths. The `print()` method iterates in reverse (highest power first), suppresses coefficient 1 when there is a variable, and handles leading negatives separately from interior ` − ` separators.

### `knot.h`: `KnotDiagram`

Parses DT notation into two parallel arrays: `crossings` (signs) and `crossing_positions` (arc position pairs).

```cpp
int first_visit  = 2 * i;
int second_visit = std::abs(value) - 1;
```

`num_components` is initialised to 1 but not updated during computation; component counting is done via union-find in `kauffman.h`.

### `kauffman.h`: `KauffmanBracket`

The core class. Contains the union-find helpers, `apply_smoothing`, `canonicalize`, `compute_delta_power`, `compute` (the recursive core), `bracket` (public entry), and `jones` (normalisation).

**`uf_find`**: uses path compression. After finding the root, every node on the path is pointed directly at it, making future lookups O(1).

**`apply_smoothing`**: the geometric heart. Determines whether to cross-connect or parallel-connect the four arc endpoints at a crossing based on `(type == 'A') == (sign > 0)`. See [`ALGORITHM.md`](ALGORITHM.md#arc-endpoints-and-smoothing) for the truth table.

**`compute`**: picks the lowest unsmoothed crossing (consistent choice ensures different branches converge to the same next subproblem), copies the union-find for each branch, recurses, and combines via `A·<A> + A⁻¹·<B>`. Cache lookup happens before branching; cache store happens after combining.

**`bracket`**: resets call count, cache hits, and cache before each run. Initialises union-find with `std::iota` (each endpoint is its own component) and bitmask with all n bits set.

**`jones`**: calls `bracket()` internally and applies the writhe correction. Note: in `main.cpp` the benchmark calls `bracket()` and then `jones()` separately, so the bracket is computed twice per benchmark pass. This is a minor inefficiency in the benchmark, not in the algorithm itself.

### End-to-end example: trefoil `{4, 6, 2}`

1. `from_dt` creates 3 crossings (all +1), arc positions `[(0,3),(2,5),(4,1)]`, `n=3`
2. `bracket()` sets up `uf=[0,1,2,3,4,5]`, `remaining=0b111=7`
3. `compute` picks crossing 0 (`p=0, q=3`):
   - `before_p=(0-1+6)%6=5`, `before_q=(3-1+6)%6=2`
   - A-smooth (cross): unite `(5,3)` and `(2,0)`, giving endpoints 0,2 connected and 3,5 connected
   - B-smooth (parallel): unite `(5,2)` and `(0,3)`, giving endpoints 2,5 connected and 0,3 connected
4. Each branch recurses with `remaining=0b110=6`, then `=0b100=4`, then `=0`
5. Leaves count circles via `std::set` of roots; result bubbles up via skein relation

Expected output:
```
Bracket <K>  = -A^5 - A^-3 + A^-7
Writhe       = 3
Jones V_K(A) = A^-4 + A^-12 - A^-16
```

---

## Development Log

### Week 2, Thursday
- Project proposed: Jones polynomial calculator
- Initial repository structure created
- Researching Kauffman bracket algorithm
- Setting up C++ development environment

### Thursday Evening, Week 2
- Implemented Laurent polynomial class
- Basic arithmetic operations working (addition, multiplication)
- Implemented first version of Kauffman bracket recursion

### First Implementation Results

| Test | Result | Status |
|------|--------|--------|
| Unknot | 1 | ✓ |
| 1 crossing | A + A⁻¹ | ✓ structural |
| Trefoil | A³ + 3A + 3A⁻¹ + A⁻³ | ✗ wrong (no component tracking) |

**Problem:** Simplified version didn't track component changes during smoothing. Bracket structure was correct but numerically wrong because the delta factors weren't applied.

**Fix:** Implemented proper arc endpoint model with union-find. Each smoothing explicitly merges the correct arc endpoint pairs; base case counts distinct components via `std::set` of roots.

### After component tracking fix
- Unknot: 1 ✓
- Trefoil: `−A⁵ − A⁻³ + A⁻⁷` ✓ (matches KnotInfo)
- Figure-8: `A⁸ − A⁴ + 1 − A⁻⁴ + A⁻⁸` ✓

### Cache development
- First attempt: history-based cache key (`DiagramState` with smoothing map): 0 cache hits
- Diagnosed: deterministic traversal order means every path is unique; history-based keys are structurally incapable of matching
- Solution: topology-based key (canonical arc connectivity + remaining bitmask)
- Result: hit rates grow from 10% at 7 crossings to 24% at 21 crossings
- Finding: for torus knots, cache overhead (std::map lookups + canonicalization) exceeds savings; wall-clock time is 2–3× worse with cache enabled despite call count reduction
