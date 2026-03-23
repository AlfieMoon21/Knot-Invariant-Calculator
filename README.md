# Knot Invariant Calculator

Computes the Jones polynomial of mathematical knots from Dowker–Thistlethwaite (DT) notation using the Kauffman bracket algorithm.

## Quick Start

```bash
make
./knot_calc
```

## What It Does

Takes a knot diagram encoded as DT notation and computes two polynomials:

- **Kauffman bracket** `<K>`: a state-sum over all 2ⁿ smoothings of the crossings
- **Jones polynomial** `V_K(A)`: the writhe-normalised bracket, a true knot invariant

The benchmark runs a series of torus knots T(2,n) with and without the memoization cache, printing call counts, cache hit rates, and wall-clock times.

Example output for the trefoil T(2,3):
```
Bracket <K>: -A^5 - A^-3 + A^-7
Writhe: 3
Jones  V(A): A^-4 + A^-12 - A^-16
```

## File Structure

| Path | Purpose |
|------|---------|
| `src/main.cpp` | Benchmark entry point |
| `include/knot.h` | `KnotDiagram`: parses DT notation, stores crossings and arc positions |
| `include/kauffman.h` | `KauffmanBracket`: recursive bracket, memoization cache, Jones normalization |
| `include/polynomial.h` | `Polynomial`: sparse Laurent polynomial arithmetic |
| `include/crossing.h` | `Crossing` struct: index and sign |
| `include/union_find.h` | Standalone `UnionFind` class (not used by kauffman.h, kept for reference) |
| `include/diagram_state.h` | `DiagramState`: the old history-based cache key (replaced, kept for reference) |
| `data/test_knots.txt` | Input data placeholder |
| `poster/poster.tex` | Conference poster (LaTeX, beamerposter + gemini theme) |

## Build

```bash
make          # build
make run      # build and run
make clean    # remove binary
```

Requires `g++` with C++17 support (`-std=c++17 -O2`).

## Documentation

- [`docs/ALGORITHM.md`](docs/ALGORITHM.md): mathematical background, the Kauffman bracket, arc endpoint model, Jones normalisation, and the memoization cache
- [`docs/DEVELOPMENT.md`](docs/DEVELOPMENT.md): implementation details, optimization strategies, full code walkthrough, and development log

## References

- Kauffman, L. H. (1987). State models and the Jones polynomial. *Topology*, 26(3), 395–407.
- Jones, V. F. R. (1985). A polynomial invariant for knots via von Neumann algebras. *Bull. Amer. Math. Soc.*, 12(1), 103–111.
- KnotInfo database: https://knotinfo.math.indiana.edu
